/*
 * POSIX Compatibility Library for AmigaOS
 *
 * Written by Frank Wille <frank@phoenix.owl.de> in 2003-2005
 *
 * $Id: chdir.c,v 1.4 2005/04/21 21:22:31 phx Exp $
 */

#pragma amiga-align
#include <dos/dos.h>
#include <proto/dos.h>
#pragma default-align
#include <errno.h>
#include "conv.h"
#include "currentdir.h"


int chdir(const char *path)
{
  struct FileInfoBlock fib;  /* long-word aligned! */
  BPTR fl;

  errno = 0;
  if (fl = Lock((STRPTR)__convert_path(path),SHARED_LOCK)) {
    if (Examine(fl,&fib)) {
      if (fib.fib_DirEntryType >= 0) {  /* must be a directory */
        fl = CurrentDir(fl);
        __freeoldcwd(fl);
        return 0;
      }
    }
    errno = ENOTDIR;
    UnLock(fl);
  }
  else {
    if (IoErr() == ERROR_OBJECT_IN_USE)
      errno = EACCES;    /* @@@ unlike Unix, AmigaOS doesn't allow that */
    else
      errno = ENOENT;
  }
  return -1;
}
