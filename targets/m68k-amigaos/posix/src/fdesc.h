/*
 * POSIX Compatibility Library for AmigaOS
 *
 * Written by Frank Wille <frank@phoenix.owl.de> in 2003-2006
 *
 * $Id: fdesc.h,v 1.8 2010/01/10 16:05:00 phx Exp $
 */

#include <stdio.h>
#include <sys/types.h>
#pragma amiga-align
#include <dos/dos.h>
#pragma default-align

/* maximum number of file descriptors */
#define MAX_FDESC 64

/* start- and end-offset for LockRecord() when emulating flock() */
#define FLOCK_START 0
#define FLOCK_END (-1)


/* internal file descriptor structure */
struct __fd_s {
  long file;
  int nestcnt;
  int open_flags;
  unsigned long flags;
  unsigned long amode;
};
#define FDFL_STDIO            (1<<0)  /* never close */
#define FDFL_INTERACTIVE      (1<<1)  /* tty */
#define FDFL_DIRLOCK          (1<<2)  /* file is a lock, not a handle! */
#define FDFL_SOCKET           (1<<3)  /* file is a socket fd (AmiTCP) */
#define FDFL_MODESET          (1<<4)  /* try to set mode on close() */
#define FDFL_FLOCKED          (1<<5)  /* locked by flock() */

/* extended vbcc FILE structure */
struct extFILE {
  FILE file;
  int fd;               /* file descriptor for this stream */
  char *lbuf_base;      /* line buffer for fgetln() */
  size_t lbuf_size;
};


/* global variables */

extern struct __fd_s *__fdesc[MAX_FDESC];


/* Prototypes */

struct __fd_s *__new_fdesc(void);
struct __fd_s *__chk_fd(int);
int __next_free_fd(int);
void __close_fdesc(struct __fd_s *);
