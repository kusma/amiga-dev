/*
 * POSIX Compatibility Library for AmigaOS
 *
 * Written by Frank Wille <frank@phoenix.owl.de> in 2003-2005
 *
 * $Id: access.c,v 1.3 2005/03/28 11:18:38 phx Exp $
 * NOTE: Unlike Unix a file is no longer accessible when an
 *       exclusive lock exists on it. No way around it?
 */

#pragma amiga-align
#include <dos/dos.h>
#include <proto/dos.h>
#pragma default-align
#include <errno.h>
#include <sys/unistd.h>
#include "conv.h"


int access(const char *path,int mode)
{
  struct FileInfoBlock fib; /* long-word aligned! */
  struct InfoData info;     /* same here */
  BPTR lock;

  errno = 0;
  if (lock = Lock((STRPTR)__convert_path(path),ACCESS_READ)) {
    if (Examine(lock,&fib)) {
      if (((mode & R_OK) && (fib.fib_Protection & FIBF_READ)) ||
          ((mode & X_OK) && (fib.fib_Protection & FIBF_EXECUTE)))
        errno = EACCES;

      else if (mode & W_OK) {
        if (Info(lock,&info)) {
          if (info.id_DiskState == ID_VALIDATED) {
            if (fib.fib_Protection & FIBF_WRITE)
              errno = EACCES;
          }
          else
            errno = EROFS;
        }
        else
          errno = EIO;  /* @@@ Info failed */
      }
    }
    else
      errno = EIO; /* @@@ Exmaine failed */

    UnLock(lock);
  }
  else {
    if (IoErr() == ERROR_OBJECT_IN_USE)
      errno = EACCES;    /* @@@ unlike Unix, AmigaOS doesn't allow that */
    else
      errno = ENOENT;
  }

  return errno ? -1 : 0;
}
