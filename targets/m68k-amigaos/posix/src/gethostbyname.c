/*
 * POSIX Compatibility Library for AmigaOS
 *
 * Written by Frank Wille <frank@phoenix.owl.de> in 2005
 *
 * $Id: gethostbyname.c,v 1.1 2005/04/10 19:56:35 phx Exp $
 */

#include "bsdsocket.h"


struct hostent *__Pgethostbyname(const char *n)
{
  if (__init_bsdsocket(-1) >= 0)
    return gethostbyname(n);

  return 0;
}
