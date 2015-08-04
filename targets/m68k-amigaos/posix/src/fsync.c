/*
 * POSIX Compatibility Library for AmigaOS
 *
 * Written by Frank Wille <frank@phoenix.owl.de> in 2005
 *
 * $Id: fsync.c,v 1.2 2005/10/17 20:14:52 phx Exp $
 */

#pragma amiga-align
#include <dos/dosextens.h>
#include <proto/dos.h>
#pragma default-align
#include <errno.h>
#include "fdesc.h"


int fsync(int fd)
{
  struct __fd_s *fp;
  struct FileHandle *fh;

  if (!(fp = __chk_fd(fd)))
    return -1;

  if (fp->flags & FDFL_SOCKET) {
    errno = EINVAL;
    return -1;
  }

  if (fh = BADDR(fp->file))
    DoPkt(fh->fh_Type,ACTION_FLUSH,0,0,0,0,0);

  return 0;
}
