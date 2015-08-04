/*
 * POSIX Compatibility Library for AmigaOS
 *
 * Written by Frank Wille <frank@phoenix.owl.de> in 2003
 *
 * $Id: getdtablesize.c,v 1.2 2005/02/27 13:45:24 phx Exp $
 */

#include "fdesc.h"


int getdtablesize(void)
{
  return MAX_FDESC;
}
