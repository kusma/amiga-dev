/*
 * NETBIOS protocol formats
 *
 * @(#) $Header: /Users/olsen/Code/cvs2svn-2010/os4-cvs-repository/contrib/roadshow/client-software/tcpdump-3.8.1/netbios.h,v 1.2 2004-09-22 15:40:55 olsen Exp $
 */

struct p8022Hdr {
    u_char	dsap;
    u_char	ssap;
    u_char	flags;
};

#define	p8022Size	3		/* min 802.2 header size */

#define UI		0x03		/* 802.2 flags */

