/*
 * POSIX Compatibility Library for AmigaOS
 *
 * Written by Frank Wille <frank@phoenix.owl.de> in 2005
 *
 * $Id: sendmsg.c,v 1.1 2005/04/10 19:56:39 phx Exp $
 */

#include "bsdsocket.h"


int __Psendmsg(int fd, const struct msghdr *msg, int flags)
{
  int s;

  if ((s = __init_bsdsocket(fd)) >= 0)
    return sendmsg(s,(struct msghdr *)msg,flags);

  return -1;
}
