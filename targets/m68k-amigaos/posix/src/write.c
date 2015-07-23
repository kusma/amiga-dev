/*
 * POSIX Compatibility Library for AmigaOS
 *
 * Written by Frank Wille <frank@phoenix.owl.de> in 2003
 *
 * $Id: write.c,v 1.3 2005/10/17 20:14:53 phx Exp $
 */

#pragma amiga-align
#include <dos/dos.h>
#include <proto/dos.h>
#pragma default-align
#include <stddef.h>
#include <errno.h>
#include "fdesc.h"
#include "bsdsocket.h"


int write(int fd,const void *buf,size_t nbytes)
{
  struct __fd_s *fp;
  int n;

  if (!(fp = __chk_fd(fd)))
    return -1;

  if (nbytes == 0)
    return 0;

  if (fp->flags & FDFL_DIRLOCK) {
    errno = EBADF;  /* we don't read directory objects */
    return -1;
  }

  if (fp->flags & FDFL_SOCKET) {
    if ((n = __init_bsdsocket(fd)) >= 0)
      return send(n,buf,nbytes,0);
    return -1;
  }

  n = Write(fp->file,buf,nbytes);
  if ((size_t)n != nbytes) {
    n = -1;
    if (IoErr() == ERROR_DISK_FULL)
      errno = ENOSPC;
    else
      errno = EIO;
  }

  return n;
}
