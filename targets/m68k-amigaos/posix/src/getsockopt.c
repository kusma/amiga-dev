/*
 * POSIX Compatibility Library for AmigaOS
 *
 * Written by Frank Wille <frank@phoenix.owl.de> in 2005
 *
 * $Id: getsockopt.c,v 1.1 2005/04/10 19:56:37 phx Exp $
 */

#include "bsdsocket.h"


int __Pgetsockopt(int fd, int lev, int nam, void *val, socklen_t *len)
{
  int s;

  if ((s = __init_bsdsocket(fd)) >= 0)
    return getsockopt(s,lev,nam,val,(LONG *)len);

  return -1;
}
