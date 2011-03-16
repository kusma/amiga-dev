#ifndef SYS_PARAM_H
#define SYS_PARAM_H \
       "$Id: param.h,v 4.1 1994/10/05 23:12:24 ppessi Exp $"
/*
 *	Definitions of some system parameters, FD_SET macros
 *
 *      Copyright © 1994 AmiTCP/IP Group,
 *                       Network Solutions Development, Inc.
 *                       All rights reserved.
 */

/*
 * AmiTCP/IP is not BSD4.3 compatible
 */
#define	BSD	199402	/* Feb, 1994 system version (year & month) */
#define BSD4_3	1
#define BSD4_4	1

#ifndef SYS_TYPES_H
#include <sys/types.h>
#endif

#define NOFILE		FD_SETSIZE /* perhaps we shouldn't use FD_SETSIZE */

#define	BIG_ENDIAN	4321	/* MSB first: 68000, ibm, net */
#define	BYTE_ORDER	BIG_ENDIAN

#define MAXHOSTNAMELEN  64	/* max length of hostname */
#define MAXLOGNAME      32	/* max length of login name */

/*
 * Macros for network/external number representation conversion.
 */
#ifndef ntohl
#define	ntohl(x)	(x)
#define	ntohs(x)	(x)
#define	htonl(x)	(x)
#define	htons(x)	(x)

#define	NTOHL(x)	(x)
#define	NTOHS(x)	(x)
#define	HTONL(x)	(x)
#define	HTONS(x)	(x)
#endif

/* Limits */
#if __SASC
#ifndef _LIMITS_H
#include <limits.h>
#endif
#elif __GNUC__
#ifndef _MACHINE_LIMITS_H
#include <machine/limits.h>
#endif
#undef LONGBITS			/* exec/types wants to use this symbol */
#undef BITSPERBYTE		/* dos/dos.h defines this */
#undef MAXINT			/* dos/dos.h defines this */
#undef MININT			/* dos/dos.h defines this */
#endif

/* Bit map related macros. */
#define	setbit(a,i)	((a)[(i)/NBBY] |= 1<<((i)%NBBY))
#define	clrbit(a,i)	((a)[(i)/NBBY] &= ~(1<<((i)%NBBY)))
#define	isset(a,i)	((a)[(i)/NBBY] & (1<<((i)%NBBY)))
#define	isclr(a,i)	(((a)[(i)/NBBY] & (1<<((i)%NBBY))) == 0)

/* Macros for counting and rounding. */
#ifndef howmany
#define	howmany(x, y)	(((x)+((y)-1))/(y))
#endif
#define	roundup(x, y)	((((x)+((y)-1))/(y))*(y))
#define powerof2(x)	((((x)-1)&(x))==0)

#ifndef KERNEL			/* Kernel has these in kern/amiga_subr.h */
#if __SASC			/* SASC has built-in versions */
#ifndef _STRING_H
#ifndef USE_BUILTIN_MATH
#define USE_BUILTIN_MATH
#endif
#include <string.h>
#endif

#else

#ifndef min
#define min(x,y) ((x) < (y) ? (x) : (y))
#endif
#ifndef max
#define max(x,y) ((x) > (y) ? (x) : (y))
#endif

#endif
#endif /* !KERNEL */

#endif /* !SYS_PARAM_H */
