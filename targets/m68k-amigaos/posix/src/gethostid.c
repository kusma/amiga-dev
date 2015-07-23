/*
 * POSIX Compatibility Library for AmigaOS
 *
 * Written by Frank Wille <frank@phoenix.owl.de> in 2005
 *
 * $Id: gethostid.c,v 1.1 2005/04/10 19:56:35 phx Exp $
 */

#include "bsdsocket.h"


long __Pgethostid(void)
{
  if (__init_bsdsocket(-1) >= 0)
    return (long)gethostid();

  return 0;  /* @@@ shouldn't fail? */
}
