/*
 * POSIX Compatibility Library for AmigaOS
 *
 * Written by Frank Wille <frank@phoenix.owl.de> in 2003
 *
 * $Id: fchdir.c,v 1.3 2005/04/21 21:22:32 phx Exp $
 */

#pragma amiga-align
#include <dos/dos.h>
#include <proto/dos.h>
#pragma default-align
#include <errno.h>
#include "fdesc.h"
#include "currentdir.h"


int fchdir(int fd)
{
  struct __fd_s *fp;

  if (!(fp = __chk_fd(fd)))
    return -1;

  if (fp->flags & FDFL_DIRLOCK) {
    BPTR oldlock;

    oldlock = CurrentDir(DupLock(fp->file));
    __freeoldcwd(oldlock);
    return 0;
  }
  else
    errno = ENOTDIR;

  return -1;
}
