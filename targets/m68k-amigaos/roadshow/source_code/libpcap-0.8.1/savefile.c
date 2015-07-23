/*
 * Copyright (c) 1993, 1994, 1995, 1996, 1997
 *	The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that: (1) source code distributions
 * retain the above copyright notice and this paragraph in its entirety, (2)
 * distributions including binary code include the above copyright notice and
 * this paragraph in its entirety in the documentation or other materials
 * provided with the distribution, and (3) all advertising materials mentioning
 * features or use of this software display the following acknowledgement:
 * ``This product includes software developed by the University of California,
 * Lawrence Berkeley Laboratory and its contributors.'' Neither the name of
 * the University nor the names of its contributors may be used to endorse
 * or promote products derived from this software without specific prior
 * written permission.
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 * savefile.c - supports offline use of tcpdump
 *	Extraction/creation by Jeffrey Mogul, DECWRL
 *	Modified by Steve McCanne, LBL.
 *
 * Used to save the received packet headers, after filtering, to
 * a file, and then read them later.
 * The first record in the file contains saved values for the machine
 * dependent values so we can print the dump file on any architecture.
 */

#ifndef lint
static const char rcsid[] _U_ =
    "@(#) $Header: /Users/olsen/Code/cvs2svn-2010/os4-cvs-repository/contrib/roadshow/client-software/libpcap-0.8.1/savefile.c,v 1.2 2004-09-22 15:28:25 olsen Exp $ (LBL)";
#endif

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <errno.h>
#ifndef AMIGA
#include <memory.h>
#endif /* AMIGA */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pcap-int.h"

#ifdef HAVE_OS_PROTO_H
#include "os-proto.h"
#endif

#define TCPDUMP_MAGIC 0xa1b2c3d4
#define PATCHED_TCPDUMP_MAGIC 0xa1b2cd34

/*
 * We use the "receiver-makes-right" approach to byte order,
 * because time is at a premium when we are writing the file.
 * In other words, the pcap_file_header and pcap_pkthdr,
 * records are written in host byte order.
 * Note that the bytes of packet data are written out in the order in
 * which they were received, so multi-byte fields in packets are not
 * written in host byte order, they're written in whatever order the
 * sending machine put them in.
 *
 * ntoh[ls] aren't sufficient because we might need to swap on a big-endian
 * machine (if the file was written in little-end order).
 */
#define	SWAPLONG(y) \
((((y)&0xff)<<24) | (((y)&0xff00)<<8) | (((y)&0xff0000)>>8) | (((y)>>24)&0xff))
#define	SWAPSHORT(y) \
	( (((y)&0xff)<<8) | ((u_short)((y)&0xff00)>>8) )

#define SFERR_TRUNC		1
#define SFERR_BADVERSION	2
#define SFERR_BADF		3
#define SFERR_EOF		4 /* not really an error, just a status */

/*
 * We don't write DLT_* values to the capture file header, because
 * they're not the same on all platforms.
 *
 * Unfortunately, the various flavors of BSD have not always used the same
 * numerical values for the same data types, and various patches to
 * libpcap for non-BSD OSes have added their own DLT_* codes for link
 * layer encapsulation types seen on those OSes, and those codes have had,
 * in some cases, values that were also used, on other platforms, for other
 * link layer encapsulation types.
 *
 * This means that capture files of a type whose numerical DLT_* code
 * means different things on different BSDs, or with different versions
 * of libpcap, can't always be read on systems other than those like
 * the one running on the machine on which the capture was made.
 *
 * Instead, we define here a set of LINKTYPE_* codes, and map DLT_* codes
 * to LINKTYPE_* codes when writing a savefile header, and map LINKTYPE_*
 * codes to DLT_* codes when reading a savefile header.
 *
 * For those DLT_* codes that have, as far as we know, the same values on
 * all platforms (DLT_NULL through DLT_FDDI), we define LINKTYPE_xxx as
 * DLT_xxx; that way, captures of those types can still be read by
 * versions of libpcap that map LINKTYPE_* values to DLT_* values, and
 * captures of those types written by versions of libpcap that map DLT_
 * values to LINKTYPE_ values can still be read by older versions
 * of libpcap.
 *
 * The other LINKTYPE_* codes are given values starting at 100, in the
 * hopes that no DLT_* code will be given one of those values.
 *
 * In order to ensure that a given LINKTYPE_* code's value will refer to
 * the same encapsulation type on all platforms, you should not allocate
 * a new LINKTYPE_* value without consulting "tcpdump-workers@tcpdump.org".
 * The tcpdump developers will allocate a value for you, and will not
 * subsequently allocate it to anybody else; that value will be added to
 * the "pcap.h" in the tcpdump.org CVS repository, so that a future
 * libpcap release will include it.
 *
 * You should, if possible, also contribute patches to libpcap and tcpdump
 * to handle the new encapsulation type, so that they can also be checked
 * into the tcpdump.org CVS repository and so that they will appear in
 * future libpcap and tcpdump releases.
 */
#define LINKTYPE_NULL		DLT_NULL
#define LINKTYPE_ETHERNET	DLT_EN10MB	/* also for 100Mb and up */
#define LINKTYPE_EXP_ETHERNET	DLT_EN3MB	/* 3Mb experimental Ethernet */
#define LINKTYPE_AX25		DLT_AX25
#define LINKTYPE_PRONET		DLT_PRONET
#define LINKTYPE_CHAOS		DLT_CHAOS
#define LINKTYPE_TOKEN_RING	DLT_IEEE802	/* DLT_IEEE802 is used for Token Ring */
#define LINKTYPE_ARCNET		DLT_ARCNET	/* BSD-style headers */
#define LINKTYPE_SLIP		DLT_SLIP
#define LINKTYPE_PPP		DLT_PPP
#define LINKTYPE_FDDI		DLT_FDDI

/*
 * LINKTYPE_PPP is for use when there might, or might not, be an RFC 1662
 * PPP in HDLC-like framing header (with 0xff 0x03 before the PPP protocol
 * field) at the beginning of the packet.
 *
 * This is for use when there is always such a header; the address field
 * might be 0xff, for regular PPP, or it might be an address field for Cisco
 * point-to-point with HDLC framing as per section 4.3.1 of RFC 1547 ("Cisco
 * HDLC").  This is, for example, what you get with NetBSD's DLT_PPP_SERIAL.
 *
 * We give it the same value as NetBSD's DLT_PPP_SERIAL, in the hopes that
 * nobody else will choose a DLT_ value of 50, and so that DLT_PPP_SERIAL
 * captures will be written out with a link type that NetBSD's tcpdump
 * can read.
 */
#define LINKTYPE_PPP_HDLC	50		/* PPP in HDLC-like framing */

#define LINKTYPE_PPP_ETHER	51		/* NetBSD PPP-over-Ethernet */

#define LINKTYPE_ATM_RFC1483	100		/* LLC/SNAP-encapsulated ATM */
#define LINKTYPE_RAW		101		/* raw IP */
#define LINKTYPE_SLIP_BSDOS	102		/* BSD/OS SLIP BPF header */
#define LINKTYPE_PPP_BSDOS	103		/* BSD/OS PPP BPF header */
#define LINKTYPE_C_HDLC		104		/* Cisco HDLC */
#define LINKTYPE_IEEE802_11	105		/* IEEE 802.11 (wireless) */
#define LINKTYPE_ATM_CLIP	106		/* Linux Classical IP over ATM */
#define LINKTYPE_FRELAY		107		/* Frame Relay */
#define LINKTYPE_LOOP		108		/* OpenBSD loopback */
#define LINKTYPE_ENC		109		/* OpenBSD IPSEC enc */

#define LINKTYPE_LINUX_SLL	113		/* Linux cooked socket capture */
#define LINKTYPE_LTALK		114		/* Apple LocalTalk hardware */
#define LINKTYPE_ECONET		115		/* Acorn Econet */

#define LINKTYPE_PFLOG		117		/* OpenBSD DLT_PFLOG */
#define LINKTYPE_CISCO_IOS	118		/* For Cisco-internal use */
#define LINKTYPE_PRISM_HEADER	119		/* 802.11+Prism II monitor mode */
#define LINKTYPE_AIRONET_HEADER	120		/* FreeBSD Aironet driver stuff */
#define LINKTYPE_IP_OVER_FC	122		/* RFC 2625 IP-over-Fibre Channel */
#define LINKTYPE_SUNATM		123		/* Solaris+SunATM */

#define LINKTYPE_IEEE802_11_RADIO 127		/* 802.11 plus WLAN header */

#define LINKTYPE_TZSP		128		/* Tazmen Sniffer Protocol */

#define LINKTYPE_ARCNET_LINUX	129		/* Linux-style headers */

#define LINKTYPE_JUNIPER_MLPPP  130		/* Juniper-internal chassis encapsulation */
#define LINKTYPE_JUNIPER_MLFR   131
#define LINKTYPE_JUNIPER_ES     132
#define LINKTYPE_JUNIPER_GGSN   133
#define LINKTYPE_JUNIPER_MFR    134
#define LINKTYPE_JUNIPER_ATM2   135
#define LINKTYPE_JUNIPER_SERVICES 136
#define LINKTYPE_JUNIPER_ATM1   137

#define LINKTYPE_APPLE_IP_OVER_IEEE1394 138	/* Apple IP-over-IEEE 1394 cooked header */

#define LINKTYPE_RAWSS7         139             /* see rawss7.h for */
#define LINKTYPE_RAWSS7_MTP2    140	        /* information  on these */
#define LINKTYPE_RAWSS7_MTP3    141             /* definitions */
#define LINKTYPE_RAWSS7_SCCP    142

#define LINKTYPE_LINUX_IRDA	144		/* Linux-IrDA */

/*
 * These types are reserved for future use.
 */
#define LINKTYPE_LANE8023	110		/* ATM LANE + 802.3 */
#define LINKTYPE_HIPPI		111		/* NetBSD HIPPI */
#define LINKTYPE_HDLC		112		/* NetBSD HDLC framing */
#define LINKTYPE_IPFILTER	116		/* IP Filter capture files */
#define LINKTYPE_HHDLC		121		/* Siemens HiPath HDLC */
#define LINKTYPE_RIO		124		/* RapidIO */
#define LINKTYPE_PCI_EXP	125		/* PCI Express */
#define LINKTYPE_AURORA		126		/* Xilinx Aurora link layer */
#define LINKTYPE_DOCSIS		143		/* DOCSIS MAC frames */
#define LINKTYPE_IBM_SP		145		/* IBM SP switch */
#define LINKTYPE_IBM_SN		146		/* IBM Next Federation switch */

static struct linktype_map {
	int	dlt;
	int	linktype;
} map[] = {
	/*
	 * These DLT_* codes have LINKTYPE_* codes with values identical
	 * to the values of the corresponding DLT_* code.
	 */
	{ DLT_NULL,		LINKTYPE_NULL },
	{ DLT_EN10MB,		LINKTYPE_ETHERNET },
	{ DLT_EN3MB,		LINKTYPE_EXP_ETHERNET },
	{ DLT_AX25,		LINKTYPE_AX25 },
	{ DLT_PRONET,		LINKTYPE_PRONET },
	{ DLT_CHAOS,		LINKTYPE_CHAOS },
	{ DLT_IEEE802,		LINKTYPE_TOKEN_RING },
	{ DLT_ARCNET,		LINKTYPE_ARCNET },
	{ DLT_SLIP,		LINKTYPE_SLIP },
	{ DLT_PPP,		LINKTYPE_PPP },
	{ DLT_FDDI,	 	LINKTYPE_FDDI },

	/*
	 * These DLT_* codes have different values on different
	 * platforms; we map them to LINKTYPE_* codes that
	 * have values that should never be equal to any DLT_*
	 * code.
	 */
#ifdef DLT_FR
	/* BSD/OS Frame Relay */
	{ DLT_FR,		LINKTYPE_FRELAY },
#endif
	{ DLT_ATM_RFC1483, 	LINKTYPE_ATM_RFC1483 },
	{ DLT_RAW,		LINKTYPE_RAW },
	{ DLT_SLIP_BSDOS,	LINKTYPE_SLIP_BSDOS },
	{ DLT_PPP_BSDOS,	LINKTYPE_PPP_BSDOS },

	/* BSD/OS Cisco HDLC */
	{ DLT_C_HDLC,		LINKTYPE_C_HDLC },

	/*
	 * These DLT_* codes are not on all platforms, but, so far,
	 * there don't appear to be any platforms that define
	 * other codes with those values; we map them to
	 * different LINKTYPE_* values anyway, just in case.
	 */

	/* Linux ATM Classical IP */
	{ DLT_ATM_CLIP,		LINKTYPE_ATM_CLIP },

	/* NetBSD sync/async serial PPP (or Cisco HDLC) */
	{ DLT_PPP_SERIAL,	LINKTYPE_PPP_HDLC },

	/* NetBSD PPP over Ethernet */
	{ DLT_PPP_ETHER,	LINKTYPE_PPP_ETHER },

	/* IEEE 802.11 wireless */
	{ DLT_IEEE802_11,	LINKTYPE_IEEE802_11 },

	/* Frame Relay */
	{ DLT_FRELAY,		LINKTYPE_FRELAY },

	/* OpenBSD loopback */
	{ DLT_LOOP,		LINKTYPE_LOOP },

	/* Linux cooked socket capture */
	{ DLT_LINUX_SLL,	LINKTYPE_LINUX_SLL },

	/* Apple LocalTalk hardware */
	{ DLT_LTALK,		LINKTYPE_LTALK },

	/* Acorn Econet */
	{ DLT_ECONET,		LINKTYPE_ECONET },

	/* OpenBSD DLT_PFLOG */
	{ DLT_PFLOG,		LINKTYPE_PFLOG },

	/* For Cisco-internal use */
	{ DLT_CISCO_IOS,	LINKTYPE_CISCO_IOS },

	/* Prism II monitor-mode header plus 802.11 header */
	{ DLT_PRISM_HEADER,	LINKTYPE_PRISM_HEADER },

	/* FreeBSD Aironet driver stuff */
	{ DLT_AIRONET_HEADER,	LINKTYPE_AIRONET_HEADER },

	/* Siemens HiPath HDLC */
	{ DLT_HHDLC,		LINKTYPE_HHDLC },

	/* RFC 2625 IP-over-Fibre Channel */
	{ DLT_IP_OVER_FC,	LINKTYPE_IP_OVER_FC },

	/* Solaris+SunATM */
	{ DLT_SUNATM,		LINKTYPE_SUNATM },

	/* RapidIO */
	{ DLT_RIO,		LINKTYPE_RIO },

	/* PCI Express */
	{ DLT_PCI_EXP,		LINKTYPE_PCI_EXP },

	/* Xilinx Aurora link layer */
	{ DLT_AURORA,		LINKTYPE_AURORA },

	/* 802.11 plus WLAN header */
	{ DLT_IEEE802_11_RADIO,	LINKTYPE_IEEE802_11_RADIO },

	/* Tazmen Sniffer Protocol */
	{ DLT_TZSP,		LINKTYPE_TZSP },

	/* Arcnet with Linux-style link-layer headers */
	{ DLT_ARCNET_LINUX,	LINKTYPE_ARCNET_LINUX },

        /* Juniper-internal chassis encapsulation */
        { DLT_JUNIPER_MLPPP,    LINKTYPE_JUNIPER_MLPPP },
        { DLT_JUNIPER_MLFR,     LINKTYPE_JUNIPER_MLFR },
        { DLT_JUNIPER_ES,       LINKTYPE_JUNIPER_ES },
        { DLT_JUNIPER_GGSN,     LINKTYPE_JUNIPER_GGSN },
        { DLT_JUNIPER_MFR,      LINKTYPE_JUNIPER_MFR },
        { DLT_JUNIPER_ATM2,     LINKTYPE_JUNIPER_ATM2 },
        { DLT_JUNIPER_SERVICES, LINKTYPE_JUNIPER_SERVICES },
        { DLT_JUNIPER_ATM1,     LINKTYPE_JUNIPER_ATM1 },

	/* Apple IP-over-IEEE 1394 cooked header */
	{ DLT_APPLE_IP_OVER_IEEE1394, LINKTYPE_APPLE_IP_OVER_IEEE1394 },

	/* DOCSIS MAC frames */
	{ DLT_DOCSIS,		LINKTYPE_DOCSIS },

	/* IrDA IrLAP packets + Linux-cooked header */
	{ DLT_LINUX_IRDA,	LINKTYPE_LINUX_IRDA },

	/* IBM SP and Next Federation switches */
	{ DLT_IBM_SP,		LINKTYPE_IBM_SP },
	{ DLT_IBM_SN,		LINKTYPE_IBM_SN },

	/*
	 * Any platform that defines additional DLT_* codes should:
	 *
	 *	request a LINKTYPE_* code and value from tcpdump.org,
	 *	as per the above;
	 *
	 *	add, in their version of libpcap, an entry to map
	 *	those DLT_* codes to the corresponding LINKTYPE_*
	 *	code;
	 *
	 *	redefine, in their "net/bpf.h", any DLT_* values
	 *	that collide with the values used by their additional
	 *	DLT_* codes, to remove those collisions (but without
	 *	making them collide with any of the LINKTYPE_*
	 *	values equal to 50 or above; they should also avoid
	 *	defining DLT_* values that collide with those
	 *	LINKTYPE_* values, either).
	 */
	{ -1,			-1 }
};

static int
dlt_to_linktype(int dlt)
{
	int i;

	for (i = 0; map[i].dlt != -1; i++) {
		if (map[i].dlt == dlt)
			return (map[i].linktype);
	}

	/*
	 * If we don't have a mapping for this DLT_ code, return an
	 * error; that means that the table above needs to have an
	 * entry added.
	 */
	return (-1);
}

static int
linktype_to_dlt(int linktype)
{
	int i;

	for (i = 0; map[i].linktype != -1; i++) {
		if (map[i].linktype == linktype)
			return (map[i].dlt);
	}

	/*
	 * If we don't have an entry for this link type, return
	 * the link type value; it may be a DLT_ value from an
	 * older version of libpcap.
	 */
	return linktype;
}

static int
sf_write_header(FILE *fp, int linktype, int thiszone, int snaplen)
{
	struct pcap_file_header hdr;

	hdr.magic = TCPDUMP_MAGIC;
	hdr.version_major = PCAP_VERSION_MAJOR;
	hdr.version_minor = PCAP_VERSION_MINOR;

	hdr.thiszone = thiszone;
	hdr.snaplen = snaplen;
	hdr.sigfigs = 0;
	hdr.linktype = linktype;

	if (fwrite((char *)&hdr, sizeof(hdr), 1, fp) != 1)
		return (-1);

	return (0);
}

static void
swap_hdr(struct pcap_file_header *hp)
{
	hp->version_major = SWAPSHORT(hp->version_major);
	hp->version_minor = SWAPSHORT(hp->version_minor);
	hp->thiszone = SWAPLONG(hp->thiszone);
	hp->sigfigs = SWAPLONG(hp->sigfigs);
	hp->snaplen = SWAPLONG(hp->snaplen);
	hp->linktype = SWAPLONG(hp->linktype);
}

static int
sf_getnonblock(pcap_t *p, char *errbuf)
{
	/*
	 * This is a savefile, not a live capture file, so never say
	 * it's in non-blocking mode.
	 */
	return (0);
}

static int
sf_setnonblock(pcap_t *p, int nonblock, char *errbuf)
{
	/*
	 * This is a savefile, not a live capture file, so ignore
	 * requests to put it in non-blocking mode.
	 */
	return (0);
}

static int
sf_stats(pcap_t *p, struct pcap_stat *ps)
{
	snprintf(p->errbuf, PCAP_ERRBUF_SIZE,
	    "Statistics aren't available from savefiles");
	return (-1);
}

static void
sf_close(pcap_t *p)
{
	if (p->sf.rfile != stdin)
		(void)fclose(p->sf.rfile);
	if (p->sf.base != NULL)
		free(p->sf.base);
}

pcap_t *
pcap_open_offline(const char *fname, char *errbuf)
{
	register pcap_t *p;
	register FILE *fp;
	struct pcap_file_header hdr;
	bpf_u_int32 magic;
	int linklen;

	p = (pcap_t *)malloc(sizeof(*p));
	if (p == NULL) {
		strlcpy(errbuf, "out of swap", PCAP_ERRBUF_SIZE);
		return (NULL);
	}

	memset((char *)p, 0, sizeof(*p));

	if (fname[0] == '-' && fname[1] == '\0')
		fp = stdin;
	else {
#ifndef WIN32
		fp = fopen(fname, "r");
#else
		fp = fopen(fname, "rb");
#endif
		if (fp == NULL) {
			snprintf(errbuf, PCAP_ERRBUF_SIZE, "%s: %s", fname,
			    pcap_strerror(errno));
			goto bad;
		}
	}
	if (fread((char *)&hdr, sizeof(hdr), 1, fp) != 1) {
		snprintf(errbuf, PCAP_ERRBUF_SIZE, "fread: %s",
		    pcap_strerror(errno));
		goto bad;
	}
	magic = hdr.magic;
	if (magic != TCPDUMP_MAGIC && magic != PATCHED_TCPDUMP_MAGIC) {
		magic = SWAPLONG(magic);
		if (magic != TCPDUMP_MAGIC && magic != PATCHED_TCPDUMP_MAGIC) {
			snprintf(errbuf, PCAP_ERRBUF_SIZE,
			    "bad dump file format");
			goto bad;
		}
		p->sf.swapped = 1;
		swap_hdr(&hdr);
	}
	if (magic == PATCHED_TCPDUMP_MAGIC) {
		/*
		 * XXX - the patch that's in some versions of libpcap
		 * changes the packet header but not the magic number;
		 * we'd have to use some hacks^H^H^H^H^Hheuristics to
		 * detect that.
		 */
		p->sf.hdrsize = sizeof(struct pcap_sf_patched_pkthdr);
	} else
		p->sf.hdrsize = sizeof(struct pcap_sf_pkthdr);
	if (hdr.version_major < PCAP_VERSION_MAJOR) {
		snprintf(errbuf, PCAP_ERRBUF_SIZE, "archaic file format");
		goto bad;
	}
	p->tzoff = hdr.thiszone;
	p->snapshot = hdr.snaplen;
	p->linktype = linktype_to_dlt(hdr.linktype);
	p->sf.rfile = fp;
#ifndef WIN32
	p->bufsize = hdr.snaplen;
#else
	/* Allocate the space for pcap_pkthdr as well. It will be used by pcap_read_ex */
	p->bufsize = hdr.snaplen+sizeof(struct pcap_pkthdr);
#endif

	/* Align link header as required for proper data alignment */
	/* XXX should handle all types */
	switch (p->linktype) {

	case DLT_EN10MB:
		linklen = 14;
		break;

	case DLT_FDDI:
		linklen = 13 + 8;	/* fddi_header + llc */
		break;

	case DLT_NULL:
	default:
		linklen = 0;
		break;
	}

	if (p->bufsize < 0)
		p->bufsize = BPF_MAXBUFSIZE;
	p->sf.base = (u_char *)malloc(p->bufsize + BPF_ALIGNMENT);
	if (p->sf.base == NULL) {
		strlcpy(errbuf, "out of swap", PCAP_ERRBUF_SIZE);
		goto bad;
	}
	p->buffer = p->sf.base + BPF_ALIGNMENT - (linklen % BPF_ALIGNMENT);
	p->sf.version_major = hdr.version_major;
	p->sf.version_minor = hdr.version_minor;
#ifdef PCAP_FDDIPAD
	/* XXX padding only needed for kernel fcode */
	pcap_fddipad = 0;
#endif

	/*
	 * We interchanged the caplen and len fields at version 2.3,
	 * in order to match the bpf header layout.  But unfortunately
	 * some files were written with version 2.3 in their headers
	 * but without the interchanged fields.
	 *
	 * In addition, DG/UX tcpdump writes out files with a version
	 * number of 543.0, and with the caplen and len fields in the
	 * pre-2.3 order.
	 */
	switch (hdr.version_major) {

	case 2:
		if (hdr.version_minor < 3)
			p->sf.lengths_swapped = SWAPPED;
		else if (hdr.version_minor == 3)
			p->sf.lengths_swapped = MAYBE_SWAPPED;
		else
			p->sf.lengths_swapped = NOT_SWAPPED;
		break;

	case 543:
		p->sf.lengths_swapped = SWAPPED;
		break;

	default:
		p->sf.lengths_swapped = NOT_SWAPPED;
		break;
	}

#ifndef WIN32
	/*
	 * You can do "select()" and "poll()" on plain files on most
	 * platforms, and should be able to do so on pipes.
	 *
	 * You can't do "select()" on anything other than sockets in
	 * Windows, so, on Win32 systems, we don't have "selectable_fd".
	 */
	p->selectable_fd = fileno(fp);
#endif

	p->read_op = pcap_offline_read;
	p->setfilter_op = install_bpf_program;
	p->set_datalink_op = NULL;	/* we don't support munging link-layer headers */
	p->getnonblock_op = sf_getnonblock;
	p->setnonblock_op = sf_setnonblock;
	p->stats_op = sf_stats;
	p->close_op = sf_close;

	return (p);
 bad:
	if(fp)
		fclose(fp);
	free(p);
	return (NULL);
}

/*
 * Read sf_readfile and return the next packet.  Return the header in hdr
 * and the contents in buf.  Return 0 on success, SFERR_EOF if there were
 * no more packets, and SFERR_TRUNC if a partial packet was encountered.
 */
static int
sf_next_packet(pcap_t *p, struct pcap_pkthdr *hdr, u_char *buf, u_int buflen)
{
	struct pcap_sf_patched_pkthdr sf_hdr;
	FILE *fp = p->sf.rfile;
	size_t amt_read;
	bpf_u_int32 t;

	/*
	 * Read the packet header; the structure we use as a buffer
	 * is the longer structure for files generated by the patched
	 * libpcap, but if the file has the magic number for an
	 * unpatched libpcap we only read as many bytes as the regular
	 * header has.
	 */
	amt_read = fread(&sf_hdr, 1, p->sf.hdrsize, fp);
	if (amt_read != p->sf.hdrsize) {
		if (ferror(fp)) {
			snprintf(p->errbuf, PCAP_ERRBUF_SIZE,
			    "error reading dump file: %s",
			    pcap_strerror(errno));
			return (-1);
		} else {
			if (amt_read != 0) {
				snprintf(p->errbuf, PCAP_ERRBUF_SIZE,
				    "truncated dump file; tried to read %d header bytes, only got %lu",
				    p->sf.hdrsize, (unsigned long)amt_read);
				return (-1);
			}
			/* EOF */
			return (1);
		}
	}

	if (p->sf.swapped) {
		/* these were written in opposite byte order */
		hdr->caplen = SWAPLONG(sf_hdr.caplen);
		hdr->len = SWAPLONG(sf_hdr.len);
		hdr->ts.tv_sec = SWAPLONG(sf_hdr.ts.tv_sec);
		hdr->ts.tv_usec = SWAPLONG(sf_hdr.ts.tv_usec);
	} else {
		hdr->caplen = sf_hdr.caplen;
		hdr->len = sf_hdr.len;
		hdr->ts.tv_sec = sf_hdr.ts.tv_sec;
		hdr->ts.tv_usec = sf_hdr.ts.tv_usec;
	}
	/* Swap the caplen and len fields, if necessary. */
	switch (p->sf.lengths_swapped) {

	case NOT_SWAPPED:
		break;

	case MAYBE_SWAPPED:
		if (hdr->caplen <= hdr->len) {
			/*
			 * The captured length is <= the actual length,
			 * so presumably they weren't swapped.
			 */
			break;
		}
		/* FALLTHROUGH */

	case SWAPPED:
		t = hdr->caplen;
		hdr->caplen = hdr->len;
		hdr->len = t;
		break;
	}

	if (hdr->caplen > buflen) {
		/*
		 * This can happen due to Solaris 2.3 systems tripping
		 * over the BUFMOD problem and not setting the snapshot
		 * correctly in the savefile header.  If the caplen isn't
		 * grossly wrong, try to salvage.
		 */
		static u_char *tp = NULL;
		static size_t tsize = 0;

		if (hdr->caplen > 65535) {
			snprintf(p->errbuf, PCAP_ERRBUF_SIZE,
			    "bogus savefile header");
			return (-1);
		}

		if (tsize < hdr->caplen) {
			tsize = ((hdr->caplen + 1023) / 1024) * 1024;
			if (tp != NULL)
				free((u_char *)tp);
			tp = (u_char *)malloc(tsize);
			if (tp == NULL) {
				tsize = 0;
				snprintf(p->errbuf, PCAP_ERRBUF_SIZE,
				    "BUFMOD hack malloc");
				return (-1);
			}
		}
		amt_read = fread((char *)tp, 1, hdr->caplen, fp);
		if (amt_read != hdr->caplen) {
			if (ferror(fp)) {
				snprintf(p->errbuf, PCAP_ERRBUF_SIZE,
				    "error reading dump file: %s",
				    pcap_strerror(errno));
			} else {
				snprintf(p->errbuf, PCAP_ERRBUF_SIZE,
				    "truncated dump file; tried to read %u captured bytes, only got %lu",
				    hdr->caplen, (unsigned long)amt_read);
			}
			return (-1);
		}
		/*
		 * We can only keep up to buflen bytes.  Since caplen > buflen
		 * is exactly how we got here, we know we can only keep the
		 * first buflen bytes and must drop the remainder.  Adjust
		 * caplen accordingly, so we don't get confused later as
		 * to how many bytes we have to play with.
		 */
		hdr->caplen = buflen;
		memcpy((char *)buf, (char *)tp, buflen);

	} else {
		/* read the packet itself */
		amt_read = fread((char *)buf, 1, hdr->caplen, fp);
		if (amt_read != hdr->caplen) {
			if (ferror(fp)) {
				snprintf(p->errbuf, PCAP_ERRBUF_SIZE,
				    "error reading dump file: %s",
				    pcap_strerror(errno));
			} else {
				snprintf(p->errbuf, PCAP_ERRBUF_SIZE,
				    "truncated dump file; tried to read %u captured bytes, only got %lu",
				    hdr->caplen, (unsigned long)amt_read);
			}
			return (-1);
		}
	}
	return (0);
}

/*
 * Print out packets stored in the file initialized by sf_read_init().
 * If cnt > 0, return after 'cnt' packets, otherwise continue until eof.
 */
int
pcap_offline_read(pcap_t *p, int cnt, pcap_handler callback, u_char *user)
{
	struct bpf_insn *fcode = p->fcode.bf_insns;
	int status = 0;
	int n = 0;

	while (status == 0) {
		struct pcap_pkthdr h;

		/*
		 * Has "pcap_breakloop()" been called?
		 * If so, return immediately - if we haven't read any
		 * packets, clear the flag and return -2 to indicate
		 * that we were told to break out of the loop, otherwise
		 * leave the flag set, so that the *next* call will break
		 * out of the loop without having read any packets, and
		 * return the number of packets we've processed so far.
		 */
		if (p->break_loop) {
			if (n == 0) {
				p->break_loop = 0;
				return (-2);
			} else
				return (n);
		}

		status = sf_next_packet(p, &h, p->buffer, p->bufsize);
		if (status) {
			if (status == 1)
				return (0);
			return (status);
		}

		if (fcode == NULL ||
		    bpf_filter(fcode, p->buffer, h.len, h.caplen)) {
			(*callback)(user, &h, p->buffer);
			if (++n >= cnt && cnt > 0)
				break;
		}
	}
	/*XXX this breaks semantics tcpslice expects */
	return (n);
}

/*
 * Output a packet to the initialized dump file.
 */
void
pcap_dump(u_char *user, const struct pcap_pkthdr *h, const u_char *sp)
{
	register FILE *f;
	struct pcap_sf_pkthdr sf_hdr;

	f = (FILE *)user;
	sf_hdr.ts.tv_sec  = h->ts.tv_sec;
	sf_hdr.ts.tv_usec = h->ts.tv_usec;
	sf_hdr.caplen     = h->caplen;
	sf_hdr.len        = h->len;
	/* XXX we should check the return status */
	(void)fwrite(&sf_hdr, sizeof(sf_hdr), 1, f);
	(void)fwrite((char *)sp, h->caplen, 1, f);
}

/*
 * Initialize so that sf_write() will output to the file named 'fname'.
 */
pcap_dumper_t *
pcap_dump_open(pcap_t *p, const char *fname)
{
	FILE *f;
	int linktype;

	linktype = dlt_to_linktype(p->linktype);
	if (linktype == -1) {
		snprintf(p->errbuf, PCAP_ERRBUF_SIZE,
		    "%s: link-layer type %d isn't supported in savefiles",
		    fname, linktype);
		return (NULL);
	}

	if (fname[0] == '-' && fname[1] == '\0') {
		f = stdout;
#ifdef WIN32
		_setmode(_fileno(f), _O_BINARY);
#endif
	} else {
#ifndef WIN32
		f = fopen(fname, "w");
#else
		f = fopen(fname, "wb");
		setbuf(f, NULL);	/* XXX - why? */
#endif
		if (f == NULL) {
			snprintf(p->errbuf, PCAP_ERRBUF_SIZE, "%s: %s",
			    fname, pcap_strerror(errno));
			return (NULL);
		}
	}
	(void)sf_write_header(f, linktype, p->tzoff, p->snapshot);
	return ((pcap_dumper_t *)f);
}

FILE *
pcap_dump_file(pcap_dumper_t *p)
{
	return ((FILE *)p);
}

int
pcap_dump_flush(pcap_dumper_t *p)
{

	if (fflush((FILE *)p) == EOF)
		return (-1);
	else
		return (0);
}

void
pcap_dump_close(pcap_dumper_t *p)
{

#ifdef notyet
	if (ferror((FILE *)p))
		return-an-error;
	/* XXX should check return from fclose() too */
#endif
	(void)fclose((FILE *)p);
}
