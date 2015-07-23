/*
 * POSIX Compatibility Library for AmigaOS
 *
 * Written by Frank Wille <frank@phoenix.owl.de> in 2005
 *
 * $Id: inet_makeaddr.c,v 1.1 2005/04/10 19:56:37 phx Exp $
 */

#define AMITCP
#include "bsdsocket.h"


struct in_addr inet_makeaddr(in_addr_t net, in_addr_t host)
{
  struct in_addr addr;

  if (__init_bsdsocket(-1) >= 0) {
    addr.s_addr = Inet_MakeAddr(net,host);
  }
  else
    addr.s_addr = INADDR_NONE;  /* @@@ */

  return addr;
}
