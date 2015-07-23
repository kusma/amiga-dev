/*
 * POSIX Compatibility Library for AmigaOS
 *
 * Written by Frank Wille <frank@phoenix.owl.de> in 2005
 *
 * $Id: recvfrom.c,v 1.1 2005/04/10 19:56:38 phx Exp $
 */

#include "bsdsocket.h"


int __Precvfrom(int fd, void *buf, size_t len, int flags,
                struct sockaddr *from, socklen_t *fromlen)
{
  int s;

  if ((s = __init_bsdsocket(fd)) >= 0)
    return recvfrom(s,buf,len,flags,from,(LONG *)fromlen);

  return -1;
}
