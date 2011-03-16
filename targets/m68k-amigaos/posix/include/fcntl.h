/*
 * POSIX Compatibility Library for AmigaOS
 *
 * Written by Frank Wille <frank@phoenix.owl.de> in 2003,2006
 *
 * $Id: fcntl.h,v 1.3 2006/01/20 23:06:41 phx Exp $
 */

#ifndef _FCNTL_H_
#define _FCNTL_H_

#include <sys/types.h>


/* open() flags */
#define O_RDONLY        0x00000000      /* open for reading only */
#define O_WRONLY        0x00000001      /* open for writing only */
#define O_RDWR          0x00000002      /* open for reading and writing */
#define O_ACCMODE       0x00000003      /* mask for above modes */
#define O_NONBLOCK      0x00000004      /* no delay */
#define O_APPEND        0x00000008      /* set append mode */
#define O_CREAT         0x00000200      /* create if nonexistent */
#define O_TRUNC         0x00000400      /* truncate to zero length */
#define O_EXCL          0x00000800      /* error if already exists */

/* fcntl() commands */
#define F_DUPFD         0               /* duplicate file descriptor */
#define F_GETFD         1               /* get file descriptor flags */
#define F_SETFD         2               /* set file descriptor flags */
#define F_GETFL         3               /* get file status flags */
#define F_SETFL         4               /* set file status flags */
#define F_GETLK         7               /* get record locking information */
#define F_SETLK         8               /* set record locking information */
#define F_SETLKW        9               /* F_SETLK; wait if blocked */

#if !defined(_POSIX_C_SOURCE)
#define LOCK_SH         0x01            /* shared file lock */  
#define LOCK_EX         0x02            /* exclusive file lock */
#define LOCK_NB         0x04            /* don't block when locking */
#define LOCK_UN         0x08            /* unlock file */
#endif


/* Prototypes */

int open(const char *, int, ...);
int creat(const char *, mode_t);
int fcntl(int, int, ...);

#if !defined(_POSIX_C_SOURCE)
int flock(int,int);
#endif

#endif /* _FCNTL_H_ */
