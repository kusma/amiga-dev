/*
 * POSIX Compatibility Library for AmigaOS
 *
 * Written by Frank Wille <frank@phoenix.owl.de> in 2003-2005
 *
 * $Id: lseek.c,v 1.3 2005/10/17 20:14:52 phx Exp $
 */

#pragma amiga-align
#include <dos/dos.h>
#include <proto/dos.h>
#pragma default-align
#include <errno.h>
#include <unistd.h>
#include "fdesc.h"


off_t lseek(int fd,off_t offset,int whence)
{
  struct FileInfoBlock fib; /* longword-aligned */
  struct __fd_s *fp;
  off_t currentpos,newpos;
  LONG mode;

  if (!(fp = __chk_fd(fd)))
    return -1;

  if (fp->flags & FDFL_DIRLOCK) {
    errno = EBADF;  /* we don't read directory objects */
    return -1;
  }

  if (fp->flags & FDFL_SOCKET) {
    errno = ESPIPE;
    return -1;
  }

  currentpos = (off_t)Seek(fp->file,0,OFFSET_CURRENT);
  if (currentpos<=0 && IoErr()==ERROR_ACTION_NOT_KNOWN)
    return 0;  /* Unix gives no error code for incompatible devices */

  /* determine AmigaOS seek-mode and new file position */
  if (!ExamineFH(fp->file,&fib)) {
    errno = EBADF;  /* @@@ better EIO, but Unix doesn't use it for lseek */
    return -1;
  }

  switch (whence) {
    case SEEK_SET:
      mode = OFFSET_BEGINNING;
      newpos = offset;
      break;
    case SEEK_CUR:
      mode = OFFSET_CURRENT;
      newpos = currentpos + offset;
      break;
    case SEEK_END:
      mode = OFFSET_END;
      newpos = (off_t)fib.fib_Size + offset;
      break;
    default:
      errno = EINVAL;
      return -1;
  }

  if (newpos < 0) {
    errno = EINVAL;
    return -1;
  }

  if (newpos > (off_t)fib.fib_Size) {
    /* we need to extend the file */
    if (SetFileSize(fp->file,(LONG)offset,mode) <= 0) {
      if (IoErr()) {
        errno = EINVAL; /* @@@ device doesn't support this action */
        return -1;
      }
    }
    Seek(fp->file,0,OFFSET_END);
  }
  else {
    /* normal seek, within file limits */
    Seek(fp->file,(LONG)offset,mode);
  }

  /* return new position */
  return (off_t)Seek(fp->file,0,OFFSET_CURRENT);
}
