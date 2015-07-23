/*
 * POSIX Compatibility Library for AmigaOS
 *
 * Written by Frank Wille <frank@phoenix.owl.de> in 2003-2005
 *
 * $Id: fstatfs.c,v 1.3 2005/10/17 20:14:52 phx Exp $
 */

#pragma amiga-align
#include <dos/dos.h>
#include <proto/dos.h>
#pragma default-align
#include <errno.h>
#include "fdesc.h"
#include "info2statfs.h"


int fstatfs(int fd,struct statfs *buf)
{
  struct InfoData info;  /* long-word aligned! */
  struct __fd_s *fp;
  int rc = 0;

  if (!(fp = __chk_fd(fd)))
    return -1;

  if (fp->flags & FDFL_SOCKET) {
    errno = EFTYPE;
    return -1;
  }

  if (fp->flags & FDFL_DIRLOCK) {
    rc = Info(fp->file,&info);  /* file is a directory-lock */
  }
  else {
    BPTR lock;

    if (lock = DupLockFromFH(fp->file)) {
      rc = Info(lock,&info);
      UnLock(lock);
    }
    else {
      errno = EACCES;  /* @@@ exclusive lock on file, can't stat! */
      return -1;
    }
  }
  if (!rc) {
    errno = EIO;
    return -1;
  }

  return __info2statfs(&info,buf);
}
