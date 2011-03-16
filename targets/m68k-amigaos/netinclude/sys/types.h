#ifndef SYS_TYPES_H
#define	SYS_TYPES_H \
       "$Id: types.h,v 4.1 1994/10/05 23:16:17 ppessi Exp $"
/*
 *	Unix-compatible types for 32-bit compilers
 *
 *      Copyright © 1994 AmiTCP/IP Group,
 *                       Network Solutions Development, Inc.
 *                       All rights reserved.
 */

typedef	unsigned char	u_char;
typedef	unsigned short	u_short;
typedef	unsigned int	u_int;
typedef	unsigned long	u_long;
typedef	unsigned short	ushort;		/* Sys V compatibility */
typedef	char*           caddr_t;        /* core address */
typedef unsigned long   dev_t;
typedef unsigned long   ino_t;
typedef long            off_t;

#ifndef AMITCP_TYPES_H
#include <amitcp/types.h>
#endif

/*
 * Select uses bit masks of file descriptors in longs.  These macros
 * manipulate such bit fields (the filesystem macros use chars).
 * FD_SETSIZE may be defined by the user, but the default here should
 * be enough for most uses.
 */
#define	NBBY	8			/* number of bits in a byte */

#ifndef	FD_SETSIZE
#define FD_SETSIZE	64
#endif

typedef long	fd_mask;
#define NFDBITS	(sizeof(fd_mask) * NBBY) /* bits per mask */

#ifndef howmany
#define	howmany(x, y)	(((x)+((y)-1))/(y))
#endif

typedef	struct fd_set {
	fd_mask	fds_bits[howmany(FD_SETSIZE, NFDBITS)];
} fd_set;

#define	FD_SET(n, p)	((p)->fds_bits[(n)/NFDBITS] |= (1 << ((n) % NFDBITS)))
#define	FD_CLR(n, p)	((p)->fds_bits[(n)/NFDBITS] &= ~(1 << ((n) % NFDBITS)))
#define	FD_ISSET(n, p)	((p)->fds_bits[(n)/NFDBITS] & (1 << ((n) % NFDBITS)))
#define	FD_ZERO(p)	bzero((char *)(p), sizeof(*(p)))

#endif /* !SYS_TYPES_H */
