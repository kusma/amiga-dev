#ifndef NETINET_IN_SYSTM_H
#define NETINET_IN_SYSTM_H \
       "$Id: in_systm.h,v 4.1 1994/10/05 23:12:24 ppessi Exp $"
/*
 *      Miscellaneous internetwork definitions for kernel.
 *
 *      Copyright © 1994 AmiTCP/IP Group,
 *                       Network Solutions Development, Inc.
 *                       All rights reserved.
 */

/*
 * Network types.
 *
 * Internally the system keeps counters in the headers with the bytes
 * swapped so that VAX instructions will work on them.  It reverses
 * the bytes before transmission at each protocol level.  The n_ types
 * represent the types with the bytes in ``high-ender'' order.
 */
typedef u_short n_short;		/* short as received from the net */
typedef u_long	n_long;			/* long as received from the net */

typedef	u_long	n_time;			/* ms since 00:00 GMT, byte rev */

#endif /* !NETINET_IN_SYSTM_H */
