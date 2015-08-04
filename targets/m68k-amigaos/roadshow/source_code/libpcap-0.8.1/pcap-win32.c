/*
 * Copyright (c) 1999 - 2003
 * NetGroup, Politecnico di Torino (Italy)
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Politecnico di Torino nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#ifndef lint
static const char rcsid[] _U_ =
    "@(#) $Header: /Users/olsen/Code/cvs2svn-2010/os4-cvs-repository/contrib/roadshow/client-software/libpcap-0.8.1/pcap-win32.c,v 1.2 2004-09-22 15:28:25 olsen Exp $ (LBL)";
#endif

#include <pcap-int.h>
#include <packet32.h>
#include <Ntddndis.h>
#ifdef __MINGW32__
int* _errno();
#define errno (*_errno())
#endif /* __MINGW32__ */

static int pcap_setfilter_win32(pcap_t *, struct bpf_program *);
static int pcap_getnonblock_win32(pcap_t *, char *);
static int pcap_setnonblock_win32(pcap_t *, int, char *);

#define	PcapBufSize 256000	/*dimension of the buffer in the pcap_t structure*/
#define	SIZE_BUF 1000000

/*
 * Header that the WinPcap driver associates to the packets.
 * Once was in bpf.h
 */
struct bpf_hdr {
	struct timeval	bh_tstamp;	/* time stamp */
	bpf_u_int32	bh_caplen;	/* length of captured portion */
	bpf_u_int32	bh_datalen;	/* original length of packet */
	u_short		bh_hdrlen;	/* length of bpf header (this struct
					   plus alignment padding) */
};

/* Start winsock */
int 
wsockinit()
{
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;
	wVersionRequested = MAKEWORD( 1, 1); 
	err = WSAStartup( wVersionRequested, &wsaData );
	if ( err != 0 )
	{
		return -1;
	}
	return 0;
}


static int
pcap_stats_win32(pcap_t *p, struct pcap_stat *ps)
{

	if(PacketGetStats(p->adapter, (struct bpf_stat*)ps) != TRUE){
		snprintf(p->errbuf, PCAP_ERRBUF_SIZE, "PacketGetStats error: %s", pcap_win32strerror());
		return -1;
	}

	return 0;
}

static int
pcap_read_win32(pcap_t *p, int cnt, pcap_handler callback, u_char *user)
{
	int cc;
	int n = 0;
	register u_char *bp, *ep;

	cc = p->cc;
	if (p->cc == 0) {
		/*
		 * Has "pcap_breakloop()" been called?
		 */
		if (p->break_loop) {
			/*
			 * Yes - clear the flag that indicates that it
			 * has, and return -2 to indicate that we were
			 * told to break out of the loop.
			 */
			p->break_loop = 0;
			return (-2);
		}

	    /* capture the packets */
		if(PacketReceivePacket(p->adapter,p->Packet,TRUE)==FALSE){
			snprintf(p->errbuf, PCAP_ERRBUF_SIZE, "read error: PacketReceivePacket failed");
			return (-1);
		}
			
		cc = p->Packet->ulBytesReceived;

		bp = p->Packet->Buffer;
	} 
	else
		bp = p->bp;

	/*
	 * Loop through each packet.
	 */
#define bhp ((struct bpf_hdr *)bp)
	ep = bp + cc;
	while (1) {
		register int caplen, hdrlen;

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
			} else {
				p->bp = bp;
				p->cc = ep - bp;
				return (n);
			}
		}
		if (bp >= ep)
			break;

		caplen = bhp->bh_caplen;
		hdrlen = bhp->bh_hdrlen;

		/*
		 * XXX A bpf_hdr matches a pcap_pkthdr.
		 */
		(*callback)(user, (struct pcap_pkthdr*)bp, bp + hdrlen);
		bp += BPF_WORDALIGN(caplen + hdrlen);
		if (++n >= cnt && cnt > 0) {
			p->bp = bp;
			p->cc = ep - bp;
			return (n);
		}
	}
#undef bhp
	p->cc = 0;
	return (n);
}


static void
pcap_close_win32(pcap_t *p)
{
	if (p->buffer != NULL)
		free(p->buffer);
	if (p->adapter != NULL) {
		PacketCloseAdapter(p->adapter);
		p->adapter = NULL;
	}
}

pcap_t *
pcap_open_live(const char *device, int snaplen, int promisc, int to_ms,
    char *ebuf)
{
	register pcap_t *p;
	NetType type;

	/* Init WinSock */
	wsockinit();

	p = (pcap_t *)malloc(sizeof(*p));
	if (p == NULL) 
	{
		snprintf(ebuf, PCAP_ERRBUF_SIZE, "malloc: %s", pcap_strerror(errno));
		return (NULL);
	}
	memset(p, 0, sizeof(*p));
	p->adapter=NULL;

	p->adapter = PacketOpenAdapter((char*)device);
	
	if (p->adapter == NULL)
	{
		/* Adapter detected but we are not able to open it. Return failure. */
		snprintf(ebuf, PCAP_ERRBUF_SIZE, "Error opening adapter: %s", pcap_win32strerror());
		return NULL;
	}
	
	/*get network type*/
	if(PacketGetNetType (p->adapter,&type) == FALSE)
	{
		snprintf(ebuf, PCAP_ERRBUF_SIZE, "Cannot determine the network type: %s", pcap_win32strerror());
		goto bad;
	}
	
	/*Set the linktype*/
	switch (type.LinkType) 
	{
	case NdisMediumWan:
		p->linktype = DLT_EN10MB;
		break;
		
	case NdisMedium802_3:
		p->linktype = DLT_EN10MB;
		break;
		
	case NdisMediumFddi:
		p->linktype = DLT_FDDI;
		break;
		
	case NdisMedium802_5:			
		p->linktype = DLT_IEEE802;	
		break;
		
	case NdisMediumArcnetRaw:
		p->linktype = DLT_ARCNET;
		break;
		
	case NdisMediumArcnet878_2:
		p->linktype = DLT_ARCNET;
		break;
		
	case NdisMediumAtm:
		p->linktype = DLT_ATM_RFC1483;
		break;
		
	default:
		p->linktype = DLT_EN10MB;			/*an unknown adapter is assumed to be ethernet*/
		break;
	}

	/* Set promisquous mode */
	if (promisc) PacketSetHwFilter(p->adapter,NDIS_PACKET_TYPE_PROMISCUOUS);
	 else PacketSetHwFilter(p->adapter,NDIS_PACKET_TYPE_ALL_LOCAL);

	/* Set the buffer size */
	p->bufsize = PcapBufSize;

	p->buffer = (u_char *)malloc(PcapBufSize);
	if (p->buffer == NULL) 
	{
		snprintf(ebuf, PCAP_ERRBUF_SIZE, "malloc: %s", pcap_strerror(errno));
		goto bad;
	}

	p->snapshot = snaplen;

	/* allocate Packet structure used during the capture */
	if((p->Packet = PacketAllocatePacket())==NULL)
	{
		snprintf(ebuf, PCAP_ERRBUF_SIZE, "failed to allocate the PACKET structure");
		goto bad;
	}

	PacketInitPacket(p->Packet,(BYTE*)p->buffer,p->bufsize);

	/* allocate the standard buffer in the driver */
	if(PacketSetBuff( p->adapter, SIZE_BUF)==FALSE)
	{
		snprintf(ebuf, PCAP_ERRBUF_SIZE,"driver error: not enough memory to allocate the kernel buffer\n");
		goto bad;
	}

	/* tell the driver to copy the buffer only if it contains at least 16K */
	if(PacketSetMinToCopy(p->adapter,16000)==FALSE)
	{
		snprintf(ebuf, PCAP_ERRBUF_SIZE,"Error calling PacketSetMinToCopy: %s\n", pcap_win32strerror());
		goto bad;
	}

	PacketSetReadTimeout(p->adapter, to_ms);

	p->read_op = pcap_read_win32;
	p->setfilter_op = pcap_setfilter_win32;
	p->set_datalink_op = NULL;	/* can't change data link type */
	p->getnonblock_op = pcap_getnonblock_win32;
	p->setnonblock_op = pcap_setnonblock_win32;
	p->stats_op = pcap_stats_win32;
	p->close_op = pcap_close_win32;

	return (p);
bad:
	if (p->adapter)
	    PacketCloseAdapter(p->adapter);
	if (p->buffer != NULL)
		free(p->buffer);
	free(p);
	return (NULL);
}


static int
pcap_setfilter_win32(pcap_t *p, struct bpf_program *fp)
{
	if(PacketSetBpf(p->adapter,fp)==FALSE){
		/* kernel filter not installed. */
		snprintf(p->errbuf, PCAP_ERRBUF_SIZE, "Driver error: cannot set bpf filter: %s", pcap_win32strerror());
		return (-1);
	}
	return (0);
}


static int
pcap_getnonblock_win32(pcap_t *p, char *errbuf)
{
	/*
	 * XXX - if there were a PacketGetReadTimeout() call, we
	 * would use it, and return 1 if the timeout is -1
	 * and 0 otherwise.
	 */
	return (p->nonblock);
}

static int
pcap_setnonblock_win32(pcap_t *p, int nonblock, char *errbuf)
{
	int newtimeout;

	if (nonblock) {
		/*
		 * Set the read timeout to -1 for non-blocking mode.
		 */
		newtimeout = -1;
	} else {
		/*
		 * Restore the timeout set when the device was opened.
		 * (Note that this may be -1, in which case we're not
		 * really leaving non-blocking mode.)
		 */
		newtimeout = p->timeout;
	}
	if (!PacketSetReadTimeout(p->adapter, newtimeout)) {
		snprintf(p->errbuf, PCAP_ERRBUF_SIZE,
		    "PacketSetReadTimeout: %s", pcap_win32strerror());
		return (-1);
	}
	p->nonblock = (newtimeout == -1);
	return (0);
}

/* Set the driver working mode */
int 
pcap_setmode(pcap_t *p, int mode){
	
	if (p->adapter==NULL)
	{
		snprintf(p->errbuf, PCAP_ERRBUF_SIZE, "impossible to set mode while reading from a file");
		return -1;
	}

	if(PacketSetMode(p->adapter,mode)==FALSE)
	{
		snprintf(p->errbuf, PCAP_ERRBUF_SIZE, "driver error: working mode not recognized");
		return -1;
	}

	return 0;
}

/* Send a packet to the network */
int 
pcap_sendpacket(pcap_t *p, u_char *buf, int size){
	LPPACKET PacketToSend;

	if (p->adapter==NULL)
	{
		snprintf(p->errbuf, PCAP_ERRBUF_SIZE, "Writing a packet is allowed only on a physical adapter");
		return -1;
	}

	PacketToSend=PacketAllocatePacket();
	PacketInitPacket(PacketToSend,buf,size);
	if(PacketSendPacket(p->adapter,PacketToSend,TRUE) == FALSE){
		PacketFreePacket(PacketToSend);
		return -1;
	}

	PacketFreePacket(PacketToSend);
	return 0;
}

/* Set the dimension of the kernel-level capture buffer */
int 
pcap_setbuff(pcap_t *p, int dim)
{
	if (p->adapter==NULL)
	{
		snprintf(p->errbuf, PCAP_ERRBUF_SIZE, "The kernel buffer size cannot be set while reading from a file");
		return -1;
	}
	
	if(PacketSetBuff(p->adapter,dim)==FALSE)
	{
		snprintf(p->errbuf, PCAP_ERRBUF_SIZE, "driver error: not enough memory to allocate the kernel buffer");
		return -1;
	}
	return 0;
}

/*set the minimum amount of data that will release a read call*/
int 
pcap_setmintocopy(pcap_t *p, int size)
{
	if (p->adapter==NULL)
	{
		snprintf(p->errbuf, PCAP_ERRBUF_SIZE, "Impossible to set the mintocopy parameter on an offline capture");
		return -1;
	}	

	if(PacketSetMinToCopy(p->adapter, size)==FALSE)
	{
		snprintf(p->errbuf, PCAP_ERRBUF_SIZE, "driver error: unable to set the requested mintocopy size");
		return -1;
	}
	return 0;
}
