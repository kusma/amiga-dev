/*
 * POSIX Compatibility Library for AmigaOS
 *
 * Written by Frank Wille <frank@phoenix.owl.de> in 2005
 *
 * $Id: inet_netof.c,v 1.1 2005/04/10 19:56:37 phx Exp $
 */

#include "bsdsocket.h"


in_addr_t inet_netof(struct in_addr in)
{
  if (__init_bsdsocket(-1) >= 0)
    return (in_addr_t)Inet_NetOf((LONG)(in).s_addr);

  return 0; /* @@@ */
}
