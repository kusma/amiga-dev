/*
 * POSIX Compatibility Library for AmigaOS
 *
 * Written by Frank Wille <frank@phoenix.owl.de> in 2005
 *
 * $Id: accept.c,v 1.2 2005/10/19 21:41:59 phx Exp $
 */

#include "bsdsocket.h"


int __Paccept(int fd, struct sockaddr *a, socklen_t *l)
{
  int s;

  if ((s = __init_bsdsocket(fd)) >= 0)
    return __socket2fd(accept(s,a,(LONG *)l));

  return -1;
}
