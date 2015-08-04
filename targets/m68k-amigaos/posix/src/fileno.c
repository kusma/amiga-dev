/*
 * POSIX Compatibility Library for AmigaOS
 *
 * Written by Frank Wille <frank@phoenix.owl.de> in 2005-2006
 *
 * $Id: fileno.c,v 1.2 2006/06/03 10:25:13 phx Exp $
 */

#include <stdio.h>
#include "fdesc.h"


int fileno(FILE *f)
{
  return f ? ((struct extFILE *)f)->fd : -1;
}
