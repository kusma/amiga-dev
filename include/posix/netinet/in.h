/*
 * AmiTCP/IP compatibility headers for PosixLib
 *
 * Written by Frank Wille <frank@phoenix.owl.de> in 2005
 *
 * $Id: in.h,v 1.1 2005/04/10 19:56:41 phx Exp $
 */

#ifndef _NETINET_IN_H_
#define _NETINET_IN_H_

#include <machine/int_types.h>

#ifndef uint8_t
typedef __uint8_t       uint8_t;
#define uint8_t         __uint8_t
#endif

#ifndef uint32_t
typedef __uint32_t      uint32_t;
#define uint32_t        __uint32_t
#endif

#include <sys/ansi.h>

#ifndef in_addr_t
typedef __in_addr_t     in_addr_t;
#define in_addr_t       __in_addr_t
#endif

#ifndef in_port_t
typedef __in_port_t     in_port_t;
#define in_port_t       __in_port_t
#endif

#ifndef sa_family_t
typedef __sa_family_t   sa_family_t;
#define sa_family_t     __sa_family_t
#endif


/*
 * Macros for network/external number representation conversion.
 */
#ifndef ntohl
#define ntohl(x)        (x)
#define ntohs(x)        (x)
#define htonl(x)        (x)
#define htons(x)        (x)

#define NTOHL(x)        (x)
#define NTOHS(x)        (x)
#define HTONL(x)        (x)
#define HTONS(x)        (x)
#endif

/*
 * Constants and structures defined by the internet system,
 * Per RFC 790, September 1981.
 */

/*
 * Protocols
 */
#define IPPROTO_IP              0               /* dummy for IP */
#define IPPROTO_ICMP            1               /* control message protocol */
#define IPPROTO_GGP             3               /* gateway^2 (deprecated) */
#define IPPROTO_TCP             6               /* tcp */
#define IPPROTO_EGP             8               /* exterior gateway protocol */
#define IPPROTO_PUP             12              /* pup */
#define IPPROTO_UDP             17              /* user datagram protocol */
#define IPPROTO_IDP             22              /* xns idp */
#define IPPROTO_TP              29              /* tp-4 w/ class negotiation */
#define IPPROTO_EON             80              /* ISO cnlp */

#define IPPROTO_RAW             255             /* raw IP packet */
#define IPPROTO_MAX             256


/*
 * Local port number conventions:
 * Ports < IPPORT_RESERVED are reserved for
 * privileged processes (e.g. root).
 * Ports > IPPORT_USERRESERVED are reserved
 * for servers, not necessarily privileged.
 */
#define IPPORT_RESERVED         1024
#define IPPORT_USERRESERVED     5000

/*
 * Internet address (a structure for historical reasons)
 */
struct in_addr {
        in_addr_t s_addr;
};

/*
 * Definitions of bits in internet address integers.
 * On subnets, the decomposition of addresses to host and net parts
 * is done according to subnet mask, not the masks here.
 */
#define IN_CLASSA(i)            (((long)(i) & 0x80000000) == 0)
#define IN_CLASSA_NET           0xff000000
#define IN_CLASSA_NSHIFT        24
#define IN_CLASSA_HOST          0x00ffffff
#define IN_CLASSA_MAX           128

#define IN_CLASSB(i)            (((long)(i) & 0xc0000000) == 0x80000000)
#define IN_CLASSB_NET           0xffff0000
#define IN_CLASSB_NSHIFT        16
#define IN_CLASSB_HOST          0x0000ffff
#define IN_CLASSB_MAX           65536

#define IN_CLASSC(i)            (((long)(i) & 0xe0000000) == 0xc0000000)
#define IN_CLASSC_NET           0xffffff00
#define IN_CLASSC_NSHIFT        8
#define IN_CLASSC_HOST          0x000000ff

#define IN_CLASSD(i)            (((long)(i) & 0xf0000000) == 0xe0000000)
#define IN_MULTICAST(i)         IN_CLASSD(i)

#define IN_EXPERIMENTAL(i)      (((long)(i) & 0xe0000000) == 0xe0000000)
#define IN_BADCLASS(i)          (((long)(i) & 0xf0000000) == 0xf0000000)

#define INADDR_ANY              (uint32_t)0x00000000
#define INADDR_BROADCAST        (uint32_t)0xffffffff    /* must be masked */
#if !defined(KERNEL) || defined(AMITCP)
#define INADDR_NONE             0xffffffff              /* -1 return */
#endif

#define IN_LOOPBACKNET          127                     /* official! */

/*
 * Socket address, internet style.
 */
struct sockaddr_in {
        uint8_t        sin_len;
        sa_family_t    sin_family;
        in_port_t      sin_port;
        struct in_addr sin_addr;
        __int8_t       sin_zero[8];
};

/*
 * Structure used to describe IP options.
 * Used to store options internally, to pass them to a process,
 * or to restore options retrieved earlier.
 * The ip_dst is used for the first-hop gateway when using a source route
 * (this gets put into the header proper).
 */
struct ip_opts {
        struct  in_addr ip_dst;         /* first hop, 0 w/o src rt */
#if defined(__cplusplus)
        __int8_t        Ip_opts[40];    /* actually variable in size */
#else
        __int8_t        ip_opts[40];    /* actually variable in size */
#endif
};

/*
 * Options for use with [gs]etsockopt at the IP level.
 * First word of comment is data type; bool is stored in int.
 */
#define IP_OPTIONS      1       /* buf/ip_opts; set/get IP per-packet options */
#define IP_HDRINCL      2       /* int; header is included with data (raw) */
#define IP_TOS          3       /* int; IP type of service and precedence */
#define IP_TTL          4       /* int; IP time to live */
#define IP_RECVOPTS     5       /* bool; receive all IP options w/datagram */
#define IP_RECVRETOPTS  6       /* bool; receive IP options for response */
#define IP_RECVDSTADDR  7       /* bool; receive IP dst addr w/datagram */
#define IP_RETOPTS      8       /* ip_opts; set/get IP per-packet options */

#endif /* _NETINET_IN_H_ */
