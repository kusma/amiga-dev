/*
 * POSIX Compatibility Library for AmigaOS
 *
 * Written by Frank Wille <frank@phoenix.owl.de> in 2005
 *
 * $Id: link.c,v 1.1 2005/04/10 19:56:38 phx Exp $
 */

#pragma amiga-align
#include <dos/dos.h>
#include <proto/dos.h>
#pragma default-align
#include <errno.h>
#include "conv.h"


int link(const char *oname,const char *lname)
{
  BPTR fl;

  if (fl = Lock(__convert_path(oname),SHARED_LOCK)) {
    if (MakeLink(__convert_path(lname),(LONG)fl,LINK_HARD)) {
      UnLock(fl);
      errno = 0;
      return 0;
    }
  }

  switch (IoErr()) {  /* find reason for failure */
    case ERROR_OBJECT_NOT_FOUND:
      errno = ENOENT; break;
    case ERROR_DISK_WRITE_PROTECTED:
      errno = EROFS; break;
    case ERROR_DISK_FULL:
      errno = ENOSPC; break;
    case ERROR_OBJECT_EXISTS:
      errno = EEXIST; break;
    case ERROR_OBJECT_WRONG_TYPE:
      errno = EXDEV; break;
    case ERROR_ACTION_NOT_KNOWN:
      errno = EOPNOTSUPP; break;
    default:
      errno = EIO; break;
  }

  return -1;
}
