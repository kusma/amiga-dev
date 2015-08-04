/*
 * POSIX Compatibility Library for AmigaOS
 *
 * Written by Frank Wille <frank@phoenix.owl.de> in 2005
 *
 * $Id: inet_network.c,v 1.1 2005/04/10 19:56:37 phx Exp $
 */

#define AMITCP
#include "bsdsocket.h"


in_addr_t __Pinet_network(const char *n)
{
  if (__init_bsdsocket(-1) >= 0)
    return inet_network(n);

  return INADDR_NONE;
}
