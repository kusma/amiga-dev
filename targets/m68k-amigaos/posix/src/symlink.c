/*
 * POSIX Compatibility Library for AmigaOS
 *
 * Written by Frank Wille <frank@phoenix.owl.de> in 2005
 *
 * $Id: symlink.c,v 1.1 2005/03/28 17:36:10 phx Exp $
 */

#pragma amiga-align
#include <dos/dos.h>
#include <proto/dos.h>
#pragma default-align
#include <errno.h>
#include "conv.h"


int symlink(const char *oname,const char *lname)
{
  if (MakeLink(__convert_path(lname),
               (LONG)__convert_path(oname),LINK_SOFT)) {
    errno = 0;
    return 0;
  }

  switch (IoErr()) {  /* find reason for failure */
    case ERROR_DISK_WRITE_PROTECTED:
      errno = EROFS; break;
    case ERROR_DISK_FULL:
      errno = ENOSPC; break;
    case ERROR_OBJECT_EXISTS:
      errno = EEXIST; break;
    case ERROR_ACTION_NOT_KNOWN:
      errno = EOPNOTSUPP; break;
    default:
      errno = EIO; break;
  }

  return -1;
}
