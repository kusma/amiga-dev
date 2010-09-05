/*
 * AmiTCP/IP compatibility headers for PosixLib
 *
 * Written by Frank Wille <frank@phoenix.owl.de> in 2005
 *
 * $Id: inet.h,v 1.2 2005/04/22 19:51:35 phx Exp $
 */

#ifndef _ARPA_INET_H_
#define _ARPA_INET_H_

#ifndef KERNEL
#ifndef _NETINET_IN_H_
#include <netinet/in.h>
#endif


/* Prototypes */

in_addr_t        __Pinet_addr(const char *);
in_addr_t        __Pinet_network(const char *);

#ifndef inet_addr
#define inet_addr(a) __Pinet_addr(a)
#endif
#ifndef inet_network
#define inet_network(a) __Pinet_network(a)
#endif

in_addr_t        inet_lnaof(struct in_addr);
struct in_addr   inet_makeaddr(in_addr_t, in_addr_t);
in_addr_t        inet_netof(struct in_addr);
char            *inet_ntoa(struct in_addr);

#endif /* !KERNEL */

#endif /* _ARPA_INET_H_ */
