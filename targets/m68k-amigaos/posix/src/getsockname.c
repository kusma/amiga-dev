/*
 * POSIX Compatibility Library for AmigaOS
 *
 * Written by Frank Wille <frank@phoenix.owl.de> in 2005
 *
 * $Id: getsockname.c,v 1.1 2005/04/10 19:56:37 phx Exp $
 */

#include "bsdsocket.h"


int __Pgetsockname(int fd, struct sockaddr *a, socklen_t *l)
{
  int s;

  if ((s = __init_bsdsocket(fd)) >= 0)
    return getsockname(s,a,(LONG *)l);

  return -1;
}
