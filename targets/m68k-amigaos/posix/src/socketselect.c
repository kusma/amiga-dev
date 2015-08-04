/*
 * POSIX Compatibility Library for AmigaOS
 *
 * Written by Frank Wille <frank@phoenix.owl.de> in 2005
 *
 * $Id: socketselect.c,v 1.1 2005/10/20 20:36:52 phx Exp $
 */

#include "bsdsocket.h"


int __socket_select(int nfds,fd_set *rd,fd_set *wr,fd_set *ex,
                    struct timeval *timeout,unsigned long *sigmask)
{
  if (__init_bsdsocket(-1) >= 0) {
    return WaitSelect(nfds,rd,wr,ex,timeout,sigmask);
  }
  return -1;
}
