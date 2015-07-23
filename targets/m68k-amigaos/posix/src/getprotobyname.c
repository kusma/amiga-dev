/*
 * POSIX Compatibility Library for AmigaOS
 *
 * Written by Frank Wille <frank@phoenix.owl.de> in 2005
 *
 * $Id: getprotobyname.c,v 1.1 2005/04/10 19:56:36 phx Exp $
 */

#include "bsdsocket.h"


struct protoent *__Pgetprotobyname(const char *n)
{
  if (__init_bsdsocket(-1) >= 0)
    return getprotobyname(n);

  return 0;
}
