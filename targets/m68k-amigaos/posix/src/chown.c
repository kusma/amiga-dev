/*
 * POSIX Compatibility Library for AmigaOS
 *
 * Written by Frank Wille <frank@phoenix.owl.de> in 2003-2005
 *
 * $Id: chown.c,v 1.3 2005/03/28 11:18:38 phx Exp $
 */

#pragma amiga-align
#include <dos/dos.h>
#include <proto/dos.h>
#pragma default-align
#include <errno.h>
#include <sys/types.h>
#include "conv.h"


int chown(const char *path,uid_t owner,gid_t group)
{
  struct FileInfoBlock fib;  /* long-word aligned! */
  BPTR lock;
  STRPTR cpath = (STRPTR)__convert_path(path);

  if (lock = Lock(cpath,ACCESS_READ)) {
    if (Examine(lock,&fib)) {
      if (owner == (uid_t)-1)
        owner = (uid_t)fib.fib_OwnerUID;
      if (group == (gid_t)-1)
        group = (gid_t)fib.fib_OwnerGID;
      if (SetOwner(cpath,(((LONG)owner&0xffff)<<16)|((LONG)group&0xffff))) {
        UnLock(lock);
        errno = 0;
        return 0;
      }
      else {
        switch (IoErr()) {
          case ERROR_OBJECT_NOT_FOUND:
            errno = ENOENT; break;
          case ERROR_DISK_WRITE_PROTECTED:
            errno = EROFS; break;
          default:
            errno = EIO; break;
        }
      }
    }
    else
      errno = EIO;
    UnLock(lock);
  }
  else {
    if (IoErr() == ERROR_OBJECT_IN_USE)
      errno = EACCES;    /* @@@ unlike Unix, AmigaOS doesn't allow that */
    else
      errno = ENOENT;
  }
  return -1;
}
