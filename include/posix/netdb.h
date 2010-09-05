/*
 * AmiTCP/IP compatibility headers for PosixLib
 *
 * Written by Frank Wille <frank@phoenix.owl.de> in 2005
 *
 * $Id: netdb.h,v 1.3 2005/10/17 21:55:04 phx Exp $
 */

#ifndef _NETDB_H_
#define _NETDB_H_

#include <sys/ansi.h>

#ifndef socklen_t
typedef __socklen_t     socklen_t;
#define socklen_t       __socklen_t
#endif

#define _PATH_DB            "AmiTCP:db"
#define _PATH_AMITCP_CONFIG "AmiTCP:db/AmiTCP.config"
#define _PATH_HEQUIV        "AmiTCP:db/hosts.equiv"
#define _PATH_INETDCONF     "AmiTCP:db/inetd.conf"

/*
 * Structures returned by network data base library.  All addresses are
 * supplied in host order, and returned in network order (suitable for
 * use in system calls).
 */
struct  hostent {
        char    *h_name;        /* official name of host */
        char    **h_aliases;    /* alias list */
        int     h_addrtype;     /* host address type */
        int     h_length;       /* length of address */
        char    **h_addr_list;  /* list of addresses from name server */
#define h_addr  h_addr_list[0]  /* address, for backward compatiblity */
};

/*
 * Assumption here is that a network number
 * fits in 32 bits -- probably a poor one.
 */
struct  netent {
        char            *n_name;        /* official name of net */
        char            **n_aliases;    /* alias list */
        int             n_addrtype;     /* net address type */
        unsigned long   n_net;          /* network # */
};

struct  servent {
        char    *s_name;        /* official service name */
        char    **s_aliases;    /* alias list */
        int     s_port;         /* port # */
        char    *s_proto;       /* protocol to use */
};

struct  protoent {
        char    *p_name;        /* official protocol name */
        char    **p_aliases;    /* alias list */
        int     p_proto;        /* protocol # */
};

#ifndef KERNEL

struct hostent  *__Pgethostbyaddr(const char *, socklen_t, int);
struct hostent  *__Pgethostbyname(const char *);
struct netent   *__Pgetnetbyaddr(unsigned long, int);
struct netent   *__Pgetnetbyname(const char *);
struct protoent *__Pgetprotobyname(const char *);
struct protoent *__Pgetprotobynumber(int);
struct servent  *__Pgetservbyname(const char *, const char *);
struct servent  *__Pgetservbyport(int, const char *);

#ifndef gethostbyaddr
#define gethostbyaddr(a,b,c) __Pgethostbyaddr(a,b,c)
#endif
#ifndef gethostbyname
#define gethostbyname(a) __Pgethostbyname(a)
#endif
#ifndef getnetbyaddr
#define getnetbyaddr(a,b) __Pgetnetbyaddr(a,b)
#endif
#ifndef getnetbyname
#define getnetbyname(a) __Pgetnetbyname(a)
#endif
#ifndef getprotobyname
#define getprotobyname(a) __Pgetprotobyname(a)
#endif
#ifndef getprotobynumber
#define getprotobynumber(a) __Pgetprotobynumber(a)
#endif
#ifndef getservbyname
#define getservbyname(a,b) __Pgetservbyname(a,b)
#endif
#ifndef getservbyport
#define getservbyport(a,b) __Pgetservbyport(a,b)
#endif

#endif /* !KERNEL */

/*
 * Error return codes from gethostbyname() and gethostbyaddr()
 * (left in extern int h_errno).
 */
#ifndef KERNEL
extern int h_errno;
#endif

#define HOST_NOT_FOUND  1 /* Authoritative Answer Host not found */
#define TRY_AGAIN       2 /* Non-Authoritive Host not found, or SERVERFAIL */
#define NO_RECOVERY     3 /* Non recoverable errors, FORMERR, REFUSED, NOTIMP */
#define NO_DATA         4 /* Valid name, no data record of requested type */
#define NO_ADDRESS      NO_DATA         /* no address, look for MX record */

#endif /* _NETDB_H_ */
