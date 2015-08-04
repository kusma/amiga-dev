/*
 * POSIX Compatibility Library for AmigaOS
 *
 * Written by Frank Wille <frank@phoenix.owl.de> in 2005
 *
 * $Id: recv.c,v 1.1 2005/04/10 19:56:38 phx Exp $
 */

#include "bsdsocket.h"


int __Precv(int fd, void *buf, size_t len, int flags)
{
  int s;

  if ((s = __init_bsdsocket(fd)) >= 0)
    return recv(s,buf,len,flags);

  return -1;
}
