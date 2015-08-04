/*
 * POSIX Compatibility Library for AmigaOS
 *
 * Written by Frank Wille <frank@phoenix.owl.de> in 2005
 *
 * $Id: sendto.c,v 1.1 2005/04/10 19:56:39 phx Exp $
 */

#include "bsdsocket.h"


int __Psendto(int fd, const void *buf, size_t len, int flags,
              const struct sockaddr *to, socklen_t tolen)
{
  int s;

  if ((s = __init_bsdsocket(fd)) >= 0)
    return sendto(s,buf,len,flags,to,tolen);

  return -1;
}
