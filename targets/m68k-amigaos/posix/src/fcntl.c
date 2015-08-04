/*
 * POSIX Compatibility Library for AmigaOS
 *
 * Written by Frank Wille <frank@phoenix.owl.de> in 2003-2005
 *
 * $Id: fcntl.c,v 1.3 2005/10/17 20:14:52 phx Exp $
 */

#pragma amiga-align
#include <dos/dos.h>
#include <proto/dos.h>
#pragma default-align
#include <stdarg.h>
#include <errno.h>
#include <fcntl.h>
#include "fdesc.h"


int fcntl(int fd, int cmd, ...)
{
  va_list vl;
  int newfd,rc=0;
  struct __fd_s *fp;

  if (!(fp = __chk_fd(fd)))
    return -1;

  va_start(vl,cmd);

  switch (cmd) {
    case F_DUPFD:
      if ((newfd = __next_free_fd(va_arg(vl,int))) < 0) {
        rc = -1;
        break;
      }
      __fdesc[newfd] = fp;
      fp->nestcnt++;
      break;

    case F_GETFD:
    case F_SETFD:
      break;            /* close-on-exec() not implemented */

    case F_GETFL:
      rc = fp->open_flags & (O_NONBLOCK | O_APPEND);
      break;

    case F_SETFL:
      fp->open_flags |= va_arg(vl,int) & (O_NONBLOCK | O_APPEND);
      if (fp->open_flags & O_APPEND) {
        if (fp->flags & FDFL_SOCKET) {
          fp->flags &= ~O_APPEND;
          errno = EINVAL;
          return -1;
        }
        Seek(fp->file,0,OFFSET_END);
      }
      break;

    default:
      errno = EINVAL;
      rc = -1;
      break;
  }

  va_end(vl);
  return rc;
}
