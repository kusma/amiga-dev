#ifndef RPC_TYPES_H
#define RPC_TYPES_H
/*
 * $Id: types.h,v 4.1 1994/09/26 08:09:11 jraja Exp jraja $
 *
 * RPC additions to <sys/types.h>
 *
 * Copyright © 1994 AmiTCP/IP Group,
 *                  Network Solutions Development Inc.
 *                  All rights reserved.
 *
 */
/* @(#)types.h	2.3 88/08/15 4.0 RPCSRC */
/*      @(#)types.h 1.18 87/07/24 SMI      */

#ifndef SYS_TYPES_H
#include <sys/types.h>
#endif
#ifndef SYS_TIME_H
#include <sys/time.h>
#endif

#include <stddef.h>

#ifndef UNISTD_H
#include <unistd.h>
#endif

/*
 * Check against short integers
 */
#if __SASC && defined(_SHORTINT)
#error RPC library works with 32-bit integers only.
#endif

/*
 * Support for Amiga SAS/C 6.x (add others)
 */
#if __SASC
#define XDRFUN __regargs
#else
#define XDRFUN
#endif /* __SASC */

#define	bool_t  int
#define	enum_t	int
#ifndef FALSE
#	define	FALSE	(0)
#endif
#ifndef TRUE
#	define	TRUE	(1)
#endif
#define __dontcare__	-1
#ifndef NULL
#	define NULL 0L
#endif

extern void exit(int code);
#ifndef USE_MEM_STUBS
extern void *malloc(size_t n);
extern void *calloc(size_t c, size_t n);
extern void free(void *p);
#define mem_alloc malloc
#define mem_calloc calloc
#define mem_free(ptr, bsize)	free(ptr)
#else
extern void *mem_alloc(size_t bsize);
extern void *mem_calloc(size_t nelt, size_t esize);
extern void mem_free(void *ptr, size_t bsize);
#endif

#ifndef INADDR_LOOPBACK
#define       INADDR_LOOPBACK         (u_long)0x7F000001
#endif
#ifndef MAXHOSTNAMELEN
#define        MAXHOSTNAMELEN  64
#endif

extern int _rpc_dtablesize(void);
extern int bindresvport(int sd, struct sockaddr_in * sin);
extern void get_myaddress(struct sockaddr_in * addr);

#endif /* !RPC_TYPES_H */
