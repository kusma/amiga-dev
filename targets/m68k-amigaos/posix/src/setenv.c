/*
 * POSIX Compatibility Library for AmigaOS
 *
 * Written by Frank Wille <frank@phoenix.owl.de> in 2006
 *
 * $Id: setenv.c,v 1.1 2006/08/08 16:59:22 phx Exp $
 */

#pragma amiga-align
#include <dos/var.h>
#include <proto/dos.h>
#pragma default-align
#include <errno.h>


int setenv(const char *name,const char *value,int overwrite)
{
  if (!SetVar(name,value,-1,GVF_LOCAL_ONLY)) {
    errno = 0;
    return 0;
  }
  errno = ENOMEM;  /* @@@ */
  return -1;
}
