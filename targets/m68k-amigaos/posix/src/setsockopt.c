/*
 * POSIX Compatibility Library for AmigaOS
 *
 * Written by Frank Wille <frank@phoenix.owl.de> in 2005
 *
 * $Id: setsockopt.c,v 1.1 2005/04/10 19:56:39 phx Exp $
 */

#include "bsdsocket.h"


int __Psetsockopt(int fd, int lev, int nam, const void *val, socklen_t len)
{
  int s;

  if ((s = __init_bsdsocket(fd)) >= 0)
    return setsockopt(s,lev,nam,val,len);

  return -1;
}
