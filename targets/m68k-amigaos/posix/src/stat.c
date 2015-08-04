/*
 * POSIX Compatibility Library for AmigaOS
 *
 * Written by Frank Wille <frank@phoenix.owl.de> in 2003-2005
 *
 * $Id: stat.c,v 1.3 2005/03/28 11:18:39 phx Exp $
 */

#pragma amiga-align
#include <dos/dos.h>
#include <proto/dos.h>
#pragma default-align
#include <errno.h>
#include "conv.h"
#include "fib2stat.h"


int stat(const char *path,struct stat *sb)
{
  struct FileInfoBlock fib;  /* long-word aligned! */
  BPTR lock;

  if (lock = Lock((STRPTR)__convert_path(path),ACCESS_READ)) {
    if (Examine(lock,&fib)) {
      UnLock(lock);
      return __fib2stat(&fib,sb);
    }
    else
      errno = EIO;
  }
  else {
    if (IoErr() == ERROR_OBJECT_IN_USE)
      errno = EACCES;    /* @@@ unlike Unix, AmigaOS doesn't allow that */
    else
      errno = ENOENT;
  }
  return -1;
}
