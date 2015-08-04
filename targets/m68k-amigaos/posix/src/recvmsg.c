/*
 * POSIX Compatibility Library for AmigaOS
 *
 * Written by Frank Wille <frank@phoenix.owl.de> in 2005
 *
 * $Id: recvmsg.c,v 1.1 2005/04/10 19:56:38 phx Exp $
 */

#include "bsdsocket.h"


int __Precvmsg(int fd, struct msghdr *msg, int flags)
{
  int s;

  if ((s = __init_bsdsocket(fd)) >= 0)
    return recvmsg(s,msg,flags);

  return -1;
}
