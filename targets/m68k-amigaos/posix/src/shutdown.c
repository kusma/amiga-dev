/*
 * POSIX Compatibility Library for AmigaOS
 *
 * Written by Frank Wille <frank@phoenix.owl.de> in 2005
 *
 * $Id: shutdown.c,v 1.1 2005/04/10 19:56:39 phx Exp $
 */

#include "bsdsocket.h"


int __Pshutdown(int fd, int how)
{
  int s;

  if ((s = __init_bsdsocket(fd)) >= 0)
    return shutdown(s,how);

  return -1;
}
