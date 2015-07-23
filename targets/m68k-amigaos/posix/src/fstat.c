/*
 * POSIX Compatibility Library for AmigaOS
 *
 * Written by Frank Wille <frank@phoenix.owl.de> in 2003,2010
 *
 * $Id: fstat.c,v 1.4 2010/01/10 17:24:19 phx Exp $
 */

#pragma amiga-align
#include <dos/dos.h>
#include <proto/dos.h>
#pragma default-align
#include <errno.h>
#include "fdesc.h"
#include "fib2stat.h"
#include "sockstat.h"


int fstat(int fd,struct stat *sb)
{
  struct FileInfoBlock fib;  /* long-word aligned! */
  struct __fd_s *fp;
  int rc;

  if (!(fp = __chk_fd(fd)))
    return -1;

  if (fp->flags & FDFL_SOCKET)
    return __sockstat((int)fp->file,sb);

  if (fp->flags & FDFL_DIRLOCK)
    rc = Examine(fp->file,&fib);  /* file is a directory-lock */
  else
    rc = ExamineFH(fp->file,&fib);
  if (!rc) {
    errno = EIO;
    return -1;
  }

  return __fib2stat(&fib,sb);
}
