/*
 * POSIX Compatibility Library for AmigaOS
 *
 * Written by Frank Wille <frank@phoenix.owl.de> in 2005
 *
 * $Id: getpeername.c,v 1.1 2005/04/10 19:56:36 phx Exp $
 */

#include "bsdsocket.h"


int __Pgetpeername(int fd, struct sockaddr *a, socklen_t *l)
{
  int s;

  if ((s = __init_bsdsocket(fd)) >= 0)
    return getpeername(s,a,(LONG *)l);

  return -1;
}
