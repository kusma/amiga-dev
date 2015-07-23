/*
 * POSIX Compatibility Library for AmigaOS
 *
 * Written by Frank Wille <frank@phoenix.owl.de> in 2003
 *
 * $Id: close.c,v 1.2 2005/02/27 13:45:21 phx Exp $
 */

#include "fdesc.h"


int close(int fd)
{
  struct __fd_s *fp;

  if (!(fp = __chk_fd(fd)))
    return -1;
  __close_fdesc(fp);
  __fdesc[fd] = NULL;
  return 0;
}
