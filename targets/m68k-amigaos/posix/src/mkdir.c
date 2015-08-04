/*
 * POSIX Compatibility Library for AmigaOS
 *
 * Written by Frank Wille <frank@phoenix.owl.de> in 2003
 *
 * $Id: mkdir.c,v 1.2 2005/02/27 13:45:25 phx Exp $
 */

#pragma amiga-align
#include <dos/dos.h>
#include <proto/dos.h>
#pragma default-align
#include <errno.h>
#include "conv.h"
#include "prot.h"


int mkdir(const char *path,mode_t mode)
{
  BPTR lock;
  STRPTR cnvpath = (STRPTR)__convert_path(path);

  if (lock = Lock(cnvpath,ACCESS_READ)) {
    UnLock(lock);
    errno = EEXIST;
    return -1;
  }

  if (lock = CreateDir(cnvpath)) {
    UnLock(lock);
    if (__set_masked_prot((char *)cnvpath,mode) < 0) {
      errno = EACCES; /* @@@ can't set protection? */
      return -1;
    }
    errno = 0;
    return 0;
  }

  switch (IoErr()) {  /* find reason for failure */
    case ERROR_OBJECT_NOT_FOUND:
      errno = ENOENT; break;
    case ERROR_DISK_WRITE_PROTECTED:
      errno = EROFS; break;
    case ERROR_DISK_FULL:
      errno = ENOSPC; break;
    default:
      errno = EIO; break;
  }
  return -1;
}
