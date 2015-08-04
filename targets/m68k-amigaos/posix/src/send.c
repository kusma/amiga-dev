/*
 * POSIX Compatibility Library for AmigaOS
 *
 * Written by Frank Wille <frank@phoenix.owl.de> in 2005
 *
 * $Id: send.c,v 1.1 2005/04/10 19:56:39 phx Exp $
 */

#include "bsdsocket.h"


int __Psend(int fd, const void *buf, size_t len, int flags)
{
  int s;

  if ((s = __init_bsdsocket(fd)) >= 0)
    return send(s,buf,len,flags);

  return -1;
}
