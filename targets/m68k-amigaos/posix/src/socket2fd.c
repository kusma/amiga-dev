/*
 * POSIX Compatibility Library for AmigaOS
 *
 * Written by Frank Wille <frank@phoenix.owl.de> in 2005
 *
 * $Id: socket2fd.c,v 1.1 2005/10/20 20:36:52 phx Exp $
 */

#include "fdesc.h"
#include "bsdsocket.h"


int __socket2fd(int s)
{
  struct __fd_s *fp;
  int fd;

  if (s >= 0) {
    for (fd=0; fd<MAX_FDESC; fd++) {
      if (fp = __fdesc[fd]) {
        if (fp->flags & FDFL_SOCKET) {
          if (s == (int)fp->file)
            return fd;
        }
      }
    }

    if ((fd = __next_free_fd(3)) < 0)
      return -1;
    if ((fp = __new_fdesc()) == NULL)
      return -1;
    fp->flags = FDFL_SOCKET | FDFL_INTERACTIVE;
    fp->file = s;
    __fdesc[fd] = fp;
    return fd;
  }

  return -1;
}
