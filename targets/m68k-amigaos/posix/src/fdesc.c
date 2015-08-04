/*
 * POSIX Compatibility Library for AmigaOS
 *
 * Written by Frank Wille <frank@phoenix.owl.de> in 2003-2006
 *
 * $Id: fdesc.c,v 1.6 2006/01/20 23:06:40 phx Exp $
 */

#pragma amiga-align
#include <dos/dos.h>
#include <dos/record.h>
#include <proto/dos.h>
#pragma default-align
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/syslimits.h>
#include "fdesc.h"
#include "bsdsocket.h"

struct __fd_s *__fdesc[MAX_FDESC];  /* all the process' file descriptors */



struct __fd_s *__new_fdesc(void)
{
  struct __fd_s *fdptr;

  if (fdptr = malloc(sizeof(struct __fd_s))) {
    memset(fdptr,0,sizeof(struct __fd_s));
  }
  else
    errno = ENFILE;
  
  return fdptr;
}


struct __fd_s *__chk_fd(int fd)
{
  struct __fd_s *fdptr;

  errno = 0;
  if (fd<0 || fd>=MAX_FDESC) {
    errno = EBADF;
    return NULL;
  }
  if ((fdptr = __fdesc[fd]) == NULL) {
    errno = EBADF;
    return NULL;
  }
  return fdptr;
}


int __next_free_fd(int start)
{
  int fd;

  for (fd=start; fd<MAX_FDESC; fd++) {
    if (__fdesc[fd] == NULL)
      break;
  }
  if (fd >= MAX_FDESC) {
    errno = EMFILE;
    return -1;
  }
  return fd;
}


void __close_fdesc(struct __fd_s *fdptr)
{
  if (fdptr->nestcnt == 0) {  /* last file descriptor closed? */

    if (fdptr->flags & FDFL_FLOCKED)
      UnLockRecord(fdptr->file,FLOCK_START,FLOCK_END);

    if (!(fdptr->flags & FDFL_STDIO)) {
      char fname[PATH_MAX];

      if (fdptr->flags & FDFL_DIRLOCK) {
        if (fdptr->flags & FDFL_MODESET) {
          if (!NameFromLock(fdptr->file,fname,PATH_MAX))
            fname[0] = '\0';
        }
        UnLock(fdptr->file);
      }
      else if (fdptr->flags & FDFL_SOCKET) {
        fdptr->flags &= ~FDFL_MODESET;
        __close_socket(fdptr->file);
      }
      else {
        if (fdptr->flags & FDFL_MODESET) {
          if (!NameFromFH(fdptr->file,fname,PATH_MAX))
            fname[0] = '\0';
        }
        Close(fdptr->file);
      }

      if ((fdptr->flags & FDFL_MODESET) && fname[0])
        SetProtection(fname,fdptr->amode);
    }
    free(fdptr);
  }

  else {
    fdptr->nestcnt--;
  }
}


void _INIT_5_fdesc(void)
{
  struct __fd_s *fp;

  __fdesc[0] = fp = __new_fdesc();
  if (fp) {
    fp->file = Input();
    if (IsInteractive(fp->file))
      fp->flags = FDFL_STDIO | FDFL_INTERACTIVE;
    else
      fp->flags = FDFL_STDIO;
    fp->open_flags = O_RDONLY;
  }

  __fdesc[1] = fp = __new_fdesc();
  if (fp) {
    fp->file = Output();
    if (IsInteractive(fp->file))
      fp->flags = FDFL_STDIO | FDFL_INTERACTIVE;
    else
      fp->flags = FDFL_STDIO;
    fp->open_flags = O_WRONLY;

    __fdesc[2] = __fdesc[1];
    fp->nestcnt++;
  }
}


void _EXIT_5_fdesc(void)
{
  struct __fd_s *fp;
  int fd;

  for (fd=0; fd<MAX_FDESC; fd++) {
    if (fp = __fdesc[fd]) {
      __close_fdesc(fp);
      __fdesc[fd] = NULL;
    }
  }
}
