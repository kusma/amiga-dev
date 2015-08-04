/*
 * POSIX Compatibility Library for AmigaOS
 *
 * Written by Frank Wille <frank@phoenix.owl.de> in 2006
 *
 * $Id: unsetenv.c,v 1.1 2006/08/08 16:59:22 phx Exp $
 */

#pragma amiga-align
#include <dos/var.h>
#include <proto/dos.h>
#pragma default-align


void unsetenv(const char *name)
{
  DeleteVar(name,GVF_LOCAL_ONLY);
}
