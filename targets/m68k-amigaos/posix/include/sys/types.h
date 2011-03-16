/*
 * POSIX Compatibility Library for AmigaOS
 *
 * Written by Frank Wille <frank@phoenix.owl.de> in 2003-2005
 *
 * $Id: types.h,v 1.10 2007/01/14 16:00:23 phx Exp $
 */

#ifndef _SYS_TYPES_H_
#define _SYS_TYPES_H_

#include <machine/types.h>
#include <machine/ansi.h>
#include <sys/ansi.h>
#include <time.h> /* for time_t */


#ifndef int8_t
typedef __int8_t        int8_t;
#define int8_t          __int8_t
#endif

#ifndef uint8_t
typedef __uint8_t       uint8_t;
#define uint8_t         __uint8_t
#endif

#ifndef int16_t
typedef __int16_t       int16_t;
#define int16_t         __int16_t
#endif

#ifndef uint16_t
typedef __uint16_t      uint16_t;
#define uint16_t        __uint16_t
#endif

#ifndef int32_t
typedef __int32_t       int32_t;
#define int32_t         __int32_t
#endif

#ifndef uint32_t
typedef __uint32_t      uint32_t;
#define uint32_t        __uint32_t
#endif

#if (__STDC__ == 1L) && (__STDC_VERSION__ >= 199901L)
#ifndef int64_t
typedef __int64_t       int64_t;
#define int64_t         __int64_t
#endif

#ifndef uint64_t
typedef __uint64_t      uint64_t;
#define uint64_t        __uint64_t
#endif
#endif

typedef uint8_t         u_int8_t;
typedef uint16_t        u_int16_t;
typedef uint32_t        u_int32_t;
#if (__STDC__ == 1L) && (__STDC_VERSION__ >= 199901L)
typedef uint64_t        u_int64_t;
#endif

#if !defined(_POSIX_SOURCE)
typedef unsigned char   u_char;
typedef unsigned short  u_short;
typedef unsigned int    u_int;
typedef unsigned long   u_long;

typedef unsigned char   unchar;         /* Sys V compatibility */
typedef unsigned short  ushort;         /* Sys V compatibility */
typedef unsigned int    uint;           /* Sys V compatibility */
typedef unsigned long   ulong;          /* Sys V compatibility */

typedef u_long          cpuid_t;
#endif

#if (__STDC__ == 1L) && (__STDC_VERSION__ >= 199901L)
typedef	uint64_t        u_quad_t;       /* quads */
typedef	int64_t         quad_t;
typedef	quad_t          *qaddr_t;
#endif

#ifndef uid_t
typedef __uid_t         uid_t;          /* user id */
#define uid_t           __uid_t
#endif

#ifndef gid_t
typedef __gid_t         gid_t;          /* group id */
#define gid_t           __gid_t
#endif

#ifndef mode_t
typedef __mode_t        mode_t;         /* permissions */
#define mode_t          __mode_t
#endif

#ifndef off_t
typedef __off_t         off_t;          /* file offset */
#define off_t           __off_t
#endif

#ifndef pid_t
typedef __pid_t         pid_t;          /* process id */
#define pid_t           __pid_t
#endif

#ifndef caddr_t
typedef __caddr_t       caddr_t;        /* core address */
#define caddr_t         __caddr_t
#endif

typedef u_int32_t       dev_t;          /* device number */
typedef u_int32_t       id_t;           /* group id, process id or user id */
typedef u_int32_t       ino_t;          /* inode number */
typedef u_int32_t       nlink_t;        /* link count */
typedef int32_t         blkcnt_t;       /* fs block count */
typedef u_int32_t       blksize_t;      /* fs optimal block size */
typedef u_int32_t       rlim_t;         /* resource limit */
typedef int32_t         daddr_t;        /* disk address */
typedef u_int32_t       fixpt_t;        /* fixed point number */
typedef long            key_t;          /* IPC key (for Sys V IPC) */
typedef int32_t         segsz_t;        /* segment size */
typedef int32_t         swblk_t;        /* swap offset */
typedef int32_t         dtime_t;        /* on-disk time_t */

#ifndef __SSIZE_T
#define __SSIZE_T 1
typedef long            ssize_t;
#endif

#ifdef  _BSD_SUSECONDS_T_
typedef _BSD_SUSECONDS_T_       suseconds_t;
#undef  _BSD_SUSECONDS_T_
#endif

#ifdef  _BSD_USECONDS_T_
typedef _BSD_USECONDS_T_        useconds_t;
#undef  _BSD_USECONDS_T_
#endif

#ifndef FD_SET

#define NBBY  8                         /* number of bits in a byte */
typedef int32_t fd_mask;
#define NFDBITS (sizeof(fd_mask) * NBBY) /* bits per mask */

#ifndef howmany
#define howmany(x, y) (((x) + ((y) - 1)) / (y))
#endif

/*
 * Select uses bit masks of file descriptors in longs.  These macros
 * manipulate such bit fields (the filesystem macros use chars).
 * FD_SETSIZE may be defined by the user, but the default here should
 * be enough for most uses.
 */

#ifndef FD_SETSIZE
#define FD_SETSIZE      64
#endif

typedef struct fd_set {
        fd_mask       fds_bits[howmany(FD_SETSIZE, NFDBITS)];
} fd_set;

#ifndef __STRING_H
#include <string.h> /* memset() */
#endif

#define FD_SET(n, p)    \
    ((p)->fds_bits[(n)/NFDBITS] |= (1 << ((n) % NFDBITS)))
#define FD_CLR(n, p)    \
    ((p)->fds_bits[(n)/NFDBITS] &= ~(1 << ((n) % NFDBITS)))
#define FD_ISSET(n, p)  \
    ((p)->fds_bits[(n)/NFDBITS] & (1 << ((n) % NFDBITS)))
#define FD_ZERO(p)      (void)memset((p), 0, sizeof(*(p)))

#endif /* FD_SET */

#endif /* _SYS_TYPES_H_ */
