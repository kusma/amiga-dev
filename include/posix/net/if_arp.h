/*
 * AmiTCP/IP compatibility headers for PosixLib
 *
 * Written by Frank Wille <frank@phoenix.owl.de> in 2005
 *
 * $Id: if_arp.h,v 1.1 2005/04/11 21:25:56 phx Exp $
 */

#ifndef _NET_IF_ARP_H_
#define _NET_IF_ARP_H_

#ifndef _SYS_SOCKET_H_
#include <sys/socket.h>
#endif

/*
 * Address Resolution Protocol.
 *
 * See RFC 826 for protocol description.  ARP packets are variable
 * in size; the arphdr structure defines the fixed-length portion.
 * Protocol type values are the same as those for 10 Mb/s Ethernet.
 * It is followed by the variable-sized fields ar_sha, arp_spa,
 * arp_tha and arp_tpa in that order, according to the lengths
 * specified.  Field names used correspond to RFC 826.
 */
struct  arphdr {
        u_int16_t ar_hrd;         /* format of hardware address */
#define ARPHRD_ETHER    1       /* ethernet hardware address */
#define ARPHRD_ARCNET   7       /* ARCNET hardware address */
        u_int16_t ar_pro;         /* format of protocol address */
        u_int8_t  ar_hln;         /* length of hardware address */
        u_int8_t  ar_pln;         /* length of protocol address */
        u_int16_t ar_op;          /* one of: */
#define ARPOP_REQUEST   1       /* request to resolve address */
#define ARPOP_REPLY     2       /* response to previous request */
/*
 * The remaining fields are variable in size,
 * according to the sizes above.
 */
/*      u_int8_t  ar_sha[];       \* sender hardware address */
/*      u_int8_t  ar_spa[];       \* sender protocol address */
/*      u_int8_t  ar_tha[];       \* target hardware address */
/*      u_int8_t  ar_tpa[];       \* target protocol address */
};

#define MAXADDRARP  16          /* Maximum number of octets in hw address */

/*
 * ARP ioctl request. 
 */
struct arpreq {
        struct  sockaddr arp_pa;                /* protocol address */
        struct  {                               /* hardware address */
          u_int8_t sa_len;
          u_int8_t sa_family;
          char   sa_data[MAXADDRARP];
        }  arp_ha;
        int     arp_flags;                      /* flags */
};

/*  arp_flags and at_flags field values */
#define ATF_INUSE       0x01    /* entry in use */
#define ATF_COM         0x02    /* completed entry (enaddr valid) */
#define ATF_PERM        0x04    /* permanent entry */
#define ATF_PUBL        0x08    /* publish entry (respond for other host) */
#define ATF_USETRAILERS 0x10    /* has requested trailers */

/* 
 * An AmiTCP/IP specific ARP table ioctl request
 */
struct arptabreq {
        struct arpreq  atr_arpreq; /* We want to identify the interface */
        long           atr_size;          /* # of elements in art_table */
        long           atr_inuse;               /* # of elements in use */
        struct arpreq *atr_table;
};

#endif /* _NET_IF_ARP_H_ */
