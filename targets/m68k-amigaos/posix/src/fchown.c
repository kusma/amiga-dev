/*
 * POSIX Compatibility Library for AmigaOS
 *
 * Written by Frank Wille <frank@phoenix.owl.de> in 2003-2005
 *
 * $Id: fchown.c,v 1.3 2005/10/17 20:14:52 phx Exp $
 */

#pragma amiga-align
#include <dos/dos.h>
#include <proto/dos.h>
#pragma default-align
#include <errno.h>
#include <sys/syslimits.h>
#include "fdesc.h"


int fchown(int fd,uid_t owner,gid_t group)
{
  struct FileInfoBlock fib;  /* long-word aligned! */
  char name[PATH_MAX];
  struct __fd_s *fp;

  if (!(fp = __chk_fd(fd)))
    return -1;

  if (fp->flags & FDFL_SOCKET) {
    errno = EINVAL;
    return -1;
  }

  if (fp->flags & FDFL_DIRLOCK) {
    if (NameFromLock(fp->file,name,PATH_MAX)) {
      if (!Examine(fp->file,&fib))
        errno = EIO;
    }
    else
      errno = ENAMETOOLONG;    
  }
  else {
    if (NameFromFH(fp->file,name,PATH_MAX)) {
      if (!ExamineFH(fp->file,&fib))
        errno = EIO;
    }
    else
      errno = ENAMETOOLONG;    
  }

  if (!errno) {
    if (owner == (uid_t)-1)
      owner = (uid_t)fib.fib_OwnerUID;
    if (group == (gid_t)-1)
      group = (gid_t)fib.fib_OwnerGID;
    if (SetOwner(name,(((LONG)owner&0xffff)<<16)|((LONG)group&0xffff))) {
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

  return -1;
}
