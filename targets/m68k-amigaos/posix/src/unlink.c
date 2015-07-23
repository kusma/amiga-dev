/*
 * POSIX Compatibility Library for AmigaOS
 *
 * Written by Frank Wille <frank@phoenix.owl.de> in 2003-2005
 *
 * $Id: unlink.c,v 1.3 2005/03/28 11:18:39 phx Exp $
 */

#pragma amiga-align
#include <dos/dos.h>
#include <proto/dos.h>
#pragma default-align
#include <errno.h>
#include "conv.h"

#ifndef EPERM
#define EPERM 1 /* vbcc's errno.h doesn't define EPERM */
#endif


int unlink(const char *path)
{
  struct FileInfoBlock fib;  /* long-word aligned! */
  BPTR lock;

  errno = 0;
  if (lock = Lock((STRPTR)__convert_path(path),ACCESS_READ)) {
    if (Examine(lock,&fib)) {
      UnLock(lock);
      if (fib.fib_DirEntryType < 0) {
        if (!DeleteFile((STRPTR)__convert_path(path))) {
          switch (IoErr()) {
            case ERROR_DISK_WRITE_PROTECTED:
              errno = EROFS; break;
            case ERROR_OBJECT_IN_USE:
              errno = EBUSY; break;
            case ERROR_DELETE_PROTECTED:
              errno = EPERM; break;  /* @@@ Unix doesn't care! */
            default:
              errno = EIO; break;
          }
        }
      }
      else
        errno = EPERM;  /* can't remove directory */
    }
    else {
      UnLock(lock);
      errno = EIO; /* @@@ */
    }
  }
  else {
    if (IoErr() == ERROR_OBJECT_IN_USE)
      errno = EBUSY;
    else
      errno = ENOENT;
  }

  return errno ? -1 : 0;
}
