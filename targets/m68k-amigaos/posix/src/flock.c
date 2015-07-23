/*
 * POSIX Compatibility Library for AmigaOS
 *
 * Written by Frank Wille <frank@phoenix.owl.de> in 2006
 *
 * $Id: flock.c,v 1.1 2006/01/20 23:06:40 phx Exp $
 */

#pragma amiga-align
#include <dos/record.h>
#include <proto/dos.h>
#pragma default-align
#include <fcntl.h>
#include <errno.h>
#include "fdesc.h"


int flock(int fd, int op)
{
  struct __fd_s *fp;
  int nb;
  BOOL success;

  if (!(fp = __chk_fd(fd)))
    return -1;

  nb = (op & LOCK_NB) != 0;
  op &= ~LOCK_NB;

  if ((fp->flags & (FDFL_DIRLOCK|FDFL_SOCKET)) != 0) {
    errno = EOPNOTSUPP;
    return -1;
  }
  if (fp->flags & FDFL_FLOCKED) {
    /* remove old flock() first */
    UnLockRecord(fp->file,FLOCK_START,FLOCK_END);
    fp->flags &= ~FDFL_FLOCKED;
  }

  /* FIXME @@@
     ACTION_LOCK_RECORD is not supported by all filesystems and does
     not work in the same way on those who support it.
     For example SFS doesn't support it. The RAM-handler supports
     a locking length of 0 while FFS doesn't.
  */
  switch (op) {
    case LOCK_SH:
      success = LockRecord(fp->file,FLOCK_START,FLOCK_END,
                           REC_SHARED+nb,nb?0:-1);
      break;
    case LOCK_EX:
      success = LockRecord(fp->file,FLOCK_START,FLOCK_END,
                           REC_EXCLUSIVE+nb,nb?0:-1);
      break;
    case LOCK_UN:
      success = UnLockRecord(fp->file,FLOCK_START,FLOCK_END);
      fp->flags &= ~FDFL_FLOCKED;
      break;
    default:
      errno = EINVAL;
      return -1;
  }

  if (!success) {
    switch (IoErr()) {
      case ERROR_ACTION_NOT_KNOWN:
        /* @@@ simulate a successful operation for filesystems
           which don't support record locking - DANGEROUS */
        break;
      case ERROR_RECORD_NOT_LOCKED:
        return 0;  /* it is allowed to unlock a file which was never locked */
      case ERROR_LOCK_COLLISION:
      case ERROR_LOCK_TIMEOUT:
        errno = EAGAIN;
        return -1;
      default:
        errno = EIO;  /* unknown error */
        return -1;
    }
  }

  fp->flags |= FDFL_FLOCKED;
  return 0;
}
