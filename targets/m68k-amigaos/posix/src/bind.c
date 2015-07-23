/*
 * POSIX Compatibility Library for AmigaOS
 *
 * Written by Frank Wille <frank@phoenix.owl.de> in 2005
 *
 * $Id: bind.c,v 1.1 2005/04/10 19:56:35 phx Exp $
 */

#include "bsdsocket.h"


int __Pbind(int fd, const struct sockaddr *a, socklen_t l)
{
  int s;

  if ((s = __init_bsdsocket(fd)) >= 0)
    return bind(s,a,l);

  return -1;
}
