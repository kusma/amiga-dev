/*
 * POSIX Compatibility Library for AmigaOS
 *
 * Written by Frank Wille <frank@phoenix.owl.de> in 2005
 *
 * $Id: getnetbyaddr.c,v 1.1 2005/04/10 19:56:36 phx Exp $
 */

#include "bsdsocket.h"


struct netent *__Pgetnetbyaddr(unsigned long n, int t)
{
  if (__init_bsdsocket(-1) >= 0)
    return getnetbyaddr(n,t);

  return 0;
}
