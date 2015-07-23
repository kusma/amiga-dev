/*
 * POSIX Compatibility Library for AmigaOS
 *
 * Written by Frank Wille <frank@phoenix.owl.de> in 2005
 *
 * $Id: ioctl.c,v 1.1 2005/10/17 20:44:30 phx Exp $
 */

#include <stdarg.h>
#include <errno.h>
#include "fdesc.h"
#include "bsdsocket.h"


int ioctl(int fd, unsigned long request, ...)
{
  struct __fd_s *fp;
  va_list vl;
  char *arg;

  if (!(fp = __chk_fd(fd)))
    return -1;

  va_start(vl,request);
  arg = va_arg(vl,char *);
  va_end(vl);

  if (fp->flags & FDFL_SOCKET) {
    int s;

    if ((s = __init_bsdsocket(fd)) >= 0)
      return IoctlSocket(s,request,arg);
  }
  else {
    /* @@@ ioctl is currently only supported for sockets */
    errno = EINVAL;
  }

  return -1;
}
