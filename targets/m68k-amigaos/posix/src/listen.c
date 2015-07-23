/*
 * POSIX Compatibility Library for AmigaOS
 *
 * Written by Frank Wille <frank@phoenix.owl.de> in 2005
 *
 * $Id: listen.c,v 1.1 2005/04/10 19:56:38 phx Exp $
 */

#include "bsdsocket.h"


int __Plisten(int fd, int bl)
{
  int s;

  if ((s = __init_bsdsocket(fd)) >= 0)
    return listen(s,bl);

  return -1;
}
