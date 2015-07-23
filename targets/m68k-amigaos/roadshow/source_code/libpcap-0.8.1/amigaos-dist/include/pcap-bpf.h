/*-
 * Copyright (c) 1990, 1991, 1992, 1993, 1994, 1995, 1996, 1997
 *	The Regents of the University of California.  All rights reserved.
 *
 * This code is derived from the Stanford/CMU enet packet filter,
 * (net/enet.c) distributed as part of 4.3BSD, and code contributed
 * to Berkeley by Steven McCanne and Van Jacobson both of Lawrence 
 * Berkeley Laboratory.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *      This product includes software developed by the University of
 *      California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *      @(#)bpf.h       7.1 (Berkeley) 5/7/91
 *
 * @(#) $Header: /Users/olsen/Code/cvs2svn-2010/os4-cvs-repository/contrib/roadshow/client-software/libpcap-0.8.1/amigaos-dist/include/pcap-bpf.h,v 1.1 2004-09-22 15:28:28 olsen Exp $ (LBL)
 */

/*
 * This is libpcap's cut-down version of bpf.h; it includes only
 * the stuff needed for the code generator and the userland BPF
 * interpreter, and the libpcap APIs for setting filters, etc..
 *
 * "pcap-bpf.c" will include the native OS version, as it deals with
 * the OS's BPF implementation.
 *
 * XXX - should this all just be moved to "pcap.h"?
 */

#ifndef BPF_MAJOR_VERSION

#ifdef __cplusplus
extern "C" {
#endif

#if defined(AMIGA)
#include "amiga_types.h"
#endif /* AMIGA */

/* BSD style release date */
#define BPF_RELEASE 199606

typedef	int bpf_int32;
typedef	u_int bpf_u_int32;

/*
 * Alignment macros.  BPF_WORDALIGN rounds up to the next 
 * even multiple of BPF_ALIGNMENT. 
 */
#ifndef __NetBSD__
#define BPF_ALIGNMENT sizeof(bpf_int32)
#else
#define BPF_ALIGNMENT sizeof(long)
#endif
#define BPF_WORDALIGN(x) (((x)+(BPF_ALIGNMENT-1))&~(BPF_ALIGNMENT-1))

#define BPF_MAXINSNS 512
#define BPF_MAXBUFSIZE 0x8000
#define BPF_MINBUFSIZE 32

/*
 * Structure for "pcap_compile()", "pcap_setfilter()", etc..
 */
struct bpf_program {
	u_int bf_len;
	struct bpf_insn *bf_insns;
};
 
/*
 * Struct return by BIOCVERSION.  This represents the version number of 
 * the filter language described by the instruction encodings below.
 * bpf understands a program iff kernel_major == filter_major &&
 * kernel_minor >= filter_minor, that is, if the value returned by the
 * running kernel has the same major number and a minor number equal
 * equal to or less than the filter being downloaded.  Otherwise, the
 * results are undefined, meaning an error may be returned or packets
 * may be accepted haphazardly.
 * It has nothing to do with the source code version.
 */
struct bpf_version {
	u_short bv_major;
	u_short bv_minor;
};
/* Current version number of filter architecture. */
#define BPF_MAJOR_VERSION 1
#define BPF_MINOR_VERSION 1

/*
 * Data-link level type codes.
 *
 * Do *NOT* add new values to this list without asking
 * "tcpdump-workers@tcpdump.org" for a value.  Otherwise, you run the
 * risk of using a value that's already being used for some other purpose,
 * and of having tools that read libpcap-format captures not being able
 * to handle captures with your new DLT_ value, with no hope that they
 * will ever be changed to do so (as that would destroy their ability
 * to read captures using that value for that other purpose).
 */

/*
 * These are the types that are the same on all platforms, and that
 * have been defined by <net/bpf.h> for ages.
 */
#define DLT_NULL	0	/* no link-layer encapsulation */
#define DLT_EN10MB	1	/* Ethernet (10Mb) */
#define DLT_EN3MB	2	/* Experimental Ethernet (3Mb) */
#define DLT_AX25	3	/* Amateur Radio AX.25 */
#define DLT_PRONET	4	/* Proteon ProNET Token Ring */
#define DLT_CHAOS	5	/* Chaos */
#define DLT_IEEE802	6	/* IEEE 802 Networks */
#define DLT_ARCNET	7	/* ARCNET, with BSD-style header */
#define DLT_SLIP	8	/* Serial Line IP */
#define DLT_PPP		9	/* Point-to-point Protocol */
#define DLT_FDDI	10	/* FDDI */

/*
 * These are types that are different on some platforms, and that
 * have been defined by <net/bpf.h> for ages.  We use #ifdefs to
 * detect the BSDs that define them differently from the traditional
 * libpcap <net/bpf.h>
 *
 * XXX - DLT_ATM_RFC1483 is 13 in BSD/OS, and DLT_RAW is 14 in BSD/OS,
 * but I don't know what the right #define is for BSD/OS.
 */
#define DLT_ATM_RFC1483	11	/* LLC/SNAP encapsulated atm */

#ifdef __OpenBSD__
#define DLT_RAW		14	/* raw IP */
#else
#define DLT_RAW		12	/* raw IP */
#endif

/*
 * Given that the only OS that currently generates BSD/OS SLIP or PPP
 * is, well, BSD/OS, arguably everybody should have chosen its values
 * for DLT_SLIP_BSDOS and DLT_PPP_BSDOS, which are 15 and 16, but they
 * didn't.  So it goes.
 */
#if defined(__NetBSD__) || defined(__FreeBSD__)
#ifndef DLT_SLIP_BSDOS
#define DLT_SLIP_BSDOS	13	/* BSD/OS Serial Line IP */
#define DLT_PPP_BSDOS	14	/* BSD/OS Point-to-point Protocol */
#endif
#else
#define DLT_SLIP_BSDOS	15	/* BSD/OS Serial Line IP */
#define DLT_PPP_BSDOS	16	/* BSD/OS Point-to-point Protocol */
#endif

/*
 * 17 is used for DLT_PFLOG in OpenBSD; don't use it for anything else.
 */

#define DLT_ATM_CLIP	19	/* Linux Classical-IP over ATM */

/*
 * These values are defined by NetBSD; other platforms should refrain from
 * using them for other purposes, so that NetBSD savefiles with link
 * types of 50 or 51 can be read as this type on all platforms.
 */
#define DLT_PPP_SERIAL	50	/* PPP over serial with HDLC encapsulation */
#define DLT_PPP_ETHER	51	/* PPP over Ethernet */

/*
 * Values between 100 and 103 are used in capture file headers as
 * link-layer types corresponding to DLT_ types that differ
 * between platforms; don't use those values for new DLT_ new types.
 */

/*
 * This value was defined by libpcap 0.5; platforms that have defined
 * it with a different value should define it here with that value -
 * a link type of 104 in a save file will be mapped to DLT_C_HDLC,
 * whatever value that happens to be, so programs will correctly
 * handle files with that link type regardless of the value of
 * DLT_C_HDLC.
 *
 * The name DLT_C_HDLC was used by BSD/OS; we use that name for source
 * compatibility with programs written for BSD/OS.
 *
 * libpcap 0.5 defined it as DLT_CHDLC; we define DLT_CHDLC as well,
 * for source compatibility with programs written for libpcap 0.5.
 */
#define DLT_C_HDLC	104	/* Cisco HDLC */
#define DLT_CHDLC	DLT_C_HDLC

#define DLT_IEEE802_11	105	/* IEEE 802.11 wireless */

/*
 * 106 is reserved for Linux Classical IP over ATM; it's like DLT_RAW,
 * except when it isn't.  (I.e., sometimes it's just raw IP, and
 * sometimes it isn't.)  We currently handle it as DLT_LINUX_SLL,
 * so that we don't have to worry about the link-layer header.)
 */

/*
 * Frame Relay; BSD/OS has a DLT_FR with a value of 11, but that collides
 * with other values.
 * DLT_FR and DLT_FRELAY packets start with the Q.922 Frame Relay header
 * (DLCI, etc.).
 */
#define DLT_FRELAY	107

/*
 * OpenBSD DLT_LOOP, for loopback devices; it's like DLT_NULL, except
 * that the AF_ type in the link-layer header is in network byte order.
 *
 * OpenBSD defines it as 12, but that collides with DLT_RAW, so we
 * define it as 108 here.  If OpenBSD picks up this file, it should
 * define DLT_LOOP as 12 in its version, as per the comment above -
 * and should not use 108 as a DLT_ value.
 */
#define DLT_LOOP	108

/*
 * Encapsulated packets for IPsec; DLT_ENC is 13 in OpenBSD, but that's
 * DLT_SLIP_BSDOS in NetBSD, so we don't use 13 for it in OSes other
 * than OpenBSD.
 */
#ifdef __OpenBSD__
#define DLT_ENC		13
#else
#define DLT_ENC		109
#endif

/*
 * Values between 110 and 112 are reserved for use in capture file headers
 * as link-layer types corresponding to DLT_ types that might differ
 * between platforms; don't use those values for new DLT_ types
 * other than the corresponding DLT_ types.
 */

/*
 * This is for Linux cooked sockets.
 */
#define DLT_LINUX_SLL	113

/*
 * Apple LocalTalk hardware.
 */
#define DLT_LTALK	114

/*
 * Acorn Econet.
 */
#define DLT_ECONET	115

/*
 * Reserved for use with OpenBSD ipfilter.
 */
#define DLT_IPFILTER	116

/*
 * OpenBSD DLT_PFLOG; DLT_PFLOG is 17 in OpenBSD, but that's DLT_LANE8023
 * in SuSE 6.3, so we can't use 17 for it in capture-file headers.
 */
#ifdef __OpenBSD__
#define DLT_PFLOG	17
#else
#define DLT_PFLOG	117
#endif

/*
 * Registered for Cisco-internal use.
 */
#define DLT_CISCO_IOS	118

/*
 * For 802.11 cards using the Prism II chips, with a link-layer
 * header including Prism monitor mode information plus an 802.11
 * header.
 */
#define DLT_PRISM_HEADER	119

/*
 * Reserved for Aironet 802.11 cards, with an Aironet link-layer header
 * (see Doug Ambrisko's FreeBSD patches).
 */
#define DLT_AIRONET_HEADER	120

/*
 * Reserved for Siemens HiPath HDLC.
 */
#define DLT_HHDLC		121

/*
 * This is for RFC 2625 IP-over-Fibre Channel.
 *
 * This is not for use with raw Fibre Channel, where the link-layer
 * header starts with a Fibre Channel frame header; it's for IP-over-FC,
 * where the link-layer header starts with an RFC 2625 Network_Header
 * field.
 */
#define DLT_IP_OVER_FC		122

/*
 * This is for Full Frontal ATM on Solaris with SunATM, with a
 * pseudo-header followed by an AALn PDU.
 *
 * There may be other forms of Full Frontal ATM on other OSes,
 * with different pseudo-headers.
 *
 * If ATM software returns a pseudo-header with VPI/VCI information
 * (and, ideally, packet type information, e.g. signalling, ILMI,
 * LANE, LLC-multiplexed traffic, etc.), it should not use
 * DLT_ATM_RFC1483, but should get a new DLT_ value, so tcpdump
 * and the like don't have to infer the presence or absence of a
 * pseudo-header and the form of the pseudo-header.
 */
#define DLT_SUNATM		123	/* Solaris+SunATM */

/* 
 * Reserved as per request from Kent Dahlgren <kent@praesum.com>
 * for private use.
 */
#define DLT_RIO                 124     /* RapidIO */
#define DLT_PCI_EXP             125     /* PCI Express */
#define DLT_AURORA              126     /* Xilinx Aurora link layer */

/*
 * For future use with 802.11 captures - defined by AbsoluteValue
 * Systems to store a number of bits of link-layer information:
 *
 *	http://www.shaftnet.org/~pizza/software/capturefrm.txt
 *
 * but could and arguably should also be used by non-AVS Linux
 * 802.11 drivers and BSD drivers; that may happen in the future.
 */
#define DLT_IEEE802_11_RADIO	127	/* 802.11 plus WLAN header */

/*
 * Reserved for the TZSP encapsulation, as per request from
 * Chris Waters <chris.waters@networkchemistry.com>
 * TZSP is a generic encapsulation for any other link type,
 * which includes a means to include meta-information
 * with the packet, e.g. signal strength and channel
 * for 802.11 packets.
 */
#define DLT_TZSP                128     /* Tazmen Sniffer Protocol */

/*
 * BSD's ARCNET headers have the source host, destination host,
 * and type at the beginning of the packet; that's what's handed
 * up to userland via BPF.
 *
 * Linux's ARCNET headers, however, have a 2-byte offset field
 * between the host IDs and the type; that's what's handed up
 * to userland via PF_PACKET sockets.
 *
 * We therefore have to have separate DLT_ values for them.
 */
#define DLT_ARCNET_LINUX	129	/* ARCNET */

/*
 * juniper-private data link types, as per request from
 * Hannes Gredler <hannes@juniper.net> the DLT_s are used
 * for passing on chassis-internal metainformation like
 * QOS profiles etc.
 */
#define DLT_JUNIPER_MLPPP       130
#define DLT_JUNIPER_MLFR        131
#define DLT_JUNIPER_ES          132
#define DLT_JUNIPER_GGSN        133
#define DLT_JUNIPER_MFR         134
#define DLT_JUNIPER_ATM2        135
#define DLT_JUNIPER_SERVICES    136
#define DLT_JUNIPER_ATM1        137

/*
 * Reserved for Apple IP-over-IEEE 1394, as per a request from Dieter
 * Siegmund <dieter@apple.com>.  The header that would be presented
 * would be an Ethernet-like header:
 *
 *	#define FIREWIRE_EUI64_LEN	8
 *	struct firewire_header {
 *		u_char  firewire_dhost[FIREWIRE_EUI64_LEN];
 *		u_char  firewire_dhost[FIREWIRE_EUI64_LEN];
 *		u_short firewire_type;
 *	};
 *
 * with "firewire_type" being an Ethernet type value, rather than,
 * for example, raw GASP frames being handed up.
 */
#define DLT_APPLE_IP_OVER_IEEE1394	138

/*
 * 139 through 142 are reserved for SS7.
 */

/*
 * Reserved for DOCSIS MAC frames.
 */
#define DLT_DOCSIS		143

/*
 * Linux-IrDA packets. Protocol defined at http://www.irda.org.
 * Those packets include IrLAP headers and above (IrLMP...), but
 * don't include Phy framing (SOF/EOF/CRC & byte stuffing), because Phy
 * framing can be handled by the hardware and depend on the bitrate.
 * This is exactly the format you would get capturing on a Linux-IrDA
 * interface (irdaX), but not on a raw serial port.
 * Note the capture is done in "Linux-cooked" mode, so each packet include
 * a fake packet header (struct sll_header). This is because IrDA packet
 * decoding is dependant on the direction of the packet (incomming or
 * outgoing).
 * When/if other platform implement IrDA capture, we may revisit the
 * issue and define a real DLT_IRDA...
 * Jean II
 */
#define DLT_LINUX_IRDA		144

/*
 * Reserved for IBM SP switch and IBM Next Federation switch.
 */
#define DLT_IBM_SP		145
#define DLT_IBM_SN		146

/*
 * Reserved for private use.  If you have some link-layer header type
 * that you want to use within your organization, with the capture files
 * using that link-layer header type not ever be sent outside your
 * organization, you can use these values.
 *
 * No libpcap release will use these for any purpose, nor will any
 * tcpdump release use them, either.
 *
 * Do *NOT* use these in capture files that you expect anybody not using
 * your private versions of capture-file-reading tools to read; in
 * particular, do *NOT* use them in products, otherwise you may find that
 * people won't be able to use tcpdump, or snort, or Ethereal, or... to
 * read capture files from your firewall/intrusion detection/traffic
 * monitoring/etc. appliance, or whatever product uses that DLT_ value,
 * and you may also find that the developers of those applications will
 * not accept patches to let them read those files.
 *
 * Instead, ask "tcpdump-workers@tcpdump.org" for a new DLT_ value,
 * as per the comment above.
 */
#define DLT_USER0		147
#define DLT_USER1		148
#define DLT_USER2		149
#define DLT_USER3		150
#define DLT_USER4		151
#define DLT_USER5		152
#define DLT_USER6		153
#define DLT_USER7		154
#define DLT_USER8		155
#define DLT_USER9		156
#define DLT_USER10		157
#define DLT_USER11		158
#define DLT_USER12		159
#define DLT_USER13		160
#define DLT_USER14		161
#define DLT_USER15		162

/*
 * The instruction encodings.
 */
/* instruction classes */
#define BPF_CLASS(code) ((code) & 0x07)
#define		BPF_LD		0x00
#define		BPF_LDX		0x01
#define		BPF_ST		0x02
#define		BPF_STX		0x03
#define		BPF_ALU		0x04
#define		BPF_JMP		0x05
#define		BPF_RET		0x06
#define		BPF_MISC	0x07

/* ld/ldx fields */
#define BPF_SIZE(code)	((code) & 0x18)
#define		BPF_W		0x00
#define		BPF_H		0x08
#define		BPF_B		0x10
#define BPF_MODE(code)	((code) & 0xe0)
#define		BPF_IMM 	0x00
#define		BPF_ABS		0x20
#define		BPF_IND		0x40
#define		BPF_MEM		0x60
#define		BPF_LEN		0x80
#define		BPF_MSH		0xa0

/* alu/jmp fields */
#define BPF_OP(code)	((code) & 0xf0)
#define		BPF_ADD		0x00
#define		BPF_SUB		0x10
#define		BPF_MUL		0x20
#define		BPF_DIV		0x30
#define		BPF_OR		0x40
#define		BPF_AND		0x50
#define		BPF_LSH		0x60
#define		BPF_RSH		0x70
#define		BPF_NEG		0x80
#define		BPF_JA		0x00
#define		BPF_JEQ		0x10
#define		BPF_JGT		0x20
#define		BPF_JGE		0x30
#define		BPF_JSET	0x40
#define BPF_SRC(code)	((code) & 0x08)
#define		BPF_K		0x00
#define		BPF_X		0x08

/* ret - BPF_K and BPF_X also apply */
#define BPF_RVAL(code)	((code) & 0x18)
#define		BPF_A		0x10

/* misc */
#define BPF_MISCOP(code) ((code) & 0xf8)
#define		BPF_TAX		0x00
#define		BPF_TXA		0x80

/*
 * The instruction data structure.
 */
struct bpf_insn {
	u_short	code;
	u_char 	jt;
	u_char 	jf;
	bpf_int32 k;
};

/*
 * Macros for insn array initializers.
 */
#define BPF_STMT(code, k) { (u_short)(code), 0, 0, k }
#define BPF_JUMP(code, k, jt, jf) { (u_short)(code), jt, jf, k }

#if __STDC__ || defined(__cplusplus)
extern int bpf_validate(struct bpf_insn *, int);
extern u_int bpf_filter(struct bpf_insn *, u_char *, u_int, u_int);
#else
extern int bpf_validate();
extern u_int bpf_filter();
#endif

/*
 * Number of scratch memory words (for BPF_LD|BPF_MEM and BPF_ST).
 */
#define BPF_MEMWORDS 16

#ifdef __cplusplus
}
#endif

#endif
