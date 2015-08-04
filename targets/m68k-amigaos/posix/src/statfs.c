/*
 * POSIX Compatibility Library for AmigaOS
 *
 * Written by Frank Wille <frank@phoenix.owl.de> in 2003-2005
 *
 * $Id: statfs.c,v 1.3 2005/03/28 11:18:39 phx Exp $
 */

#pragma amiga-align
#include <dos/dos.h>
#include <proto/dos.h>
#pragma default-align
#include <errno.h>
#include "conv.h"
#include "info2statfs.h"


int statfs(const char *path,struct statfs *buf)
{
  struct InfoData info;  /* long-word aligned! */
  BPTR lock;

  if (lock = Lock((STRPTR)__convert_path(path),ACCESS_READ)) {
    if (Info(lock,&info)) {
      UnLock(lock);
      return __info2statfs(&info,buf);
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
