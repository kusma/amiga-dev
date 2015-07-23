/*
 * POSIX Compatibility Library for AmigaOS
 *
 * Written by Frank Wille <frank@phoenix.owl.de> in 2005
 *
 * $Id: getnetbyname.c,v 1.1 2005/04/10 19:56:36 phx Exp $
 */

#include "bsdsocket.h"


struct netent *__Pgetnetbyname(const char *n)
{
  if (__init_bsdsocket(-1) >= 0)
    return getnetbyname(n);

  return 0;
}