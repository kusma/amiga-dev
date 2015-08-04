/*
 * POSIX Compatibility Library for AmigaOS
 *
 * Written by Frank Wille <frank@phoenix.owl.de> in 2003-2005
 *
 * $Id: isatty.c,v 1.4 2005/10/17 20:14:52 phx Exp $
 */

#include <errno.h>
#include "fdesc.h"


int isatty(int fd)
{
  struct __fd_s *fp;

  if (!(fp = __chk_fd(fd)))
    return 0;

  if ((fp->flags & (FDFL_DIRLOCK|FDFL_SOCKET)) == 0) {
    if (fp->flags & FDFL_INTERACTIVE) {
      return 1;
    }
  }
  errno = ENOTTY;
  return 0;
}
