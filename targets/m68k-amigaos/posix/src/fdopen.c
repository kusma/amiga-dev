/*
 * POSIX Compatibility Library for AmigaOS
 *
 * Written by Frank Wille <frank@phoenix.owl.de> in 2005-2006
 *
 * $Id: fdopen.c,v 1.4 2006/08/09 11:05:33 phx Exp $
 */

#include <stddef.h>
#include <stdio.h>
#include <limits.h>
#include <stdarg.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include "fdesc.h"

extern FILE *__firstfile,*__lastfile;


FILE *fdopen(int fd,const char *mode)
{
  struct extFILE *f;
  struct __fd_s *fdptr;
  int om,append;

  if (!(fdptr = __chk_fd(fd)))
    return 0;

  if (fdptr->flags & FDFL_SOCKET) {
    errno = EFTYPE;
    return 0;
  }

  if (*mode == 'w')
    om = O_CREAT|O_TRUNC|O_WRONLY;
  else
    om = O_RDONLY;
  if (*mode == 'a')
    om = O_CREAT|O_APPEND|O_WRONLY;

  if (!(f = malloc(sizeof(struct extFILE))))
    return(0);
  f->file.filehandle = (char *)fdptr->file;
  f->file.count = 0;
  f->file.base = 0;
  f->file.bufsize = 0;
  f->file.next = 0;
  f->fd = fd;
  f->lbuf_base = 0;
  f->lbuf_size = 0;
  if (*mode == 'r')
    f->file.flags = _READABLE;
  else
    f->file.flags = _WRITEABLE;
  if (*++mode == 'b')
    mode++;
  if (*mode == '+') {
    f->file.flags |= _READABLE|_WRITEABLE;
    om &= ~O_ACCMODE;
    om |= O_RDWR;
  }

  if ((fdptr->open_flags & (O_RDONLY|O_WRONLY|O_RDWR|O_NONBLOCK))
      != (om & (O_RDONLY|O_WRONLY|O_RDWR|O_NONBLOCK))) {
    free(f);
    errno = EINVAL;
    return 0;
  }
  if (fdptr->flags & FDFL_INTERACTIVE)
    f->file.flags |= _LINEBUF;

  if(__lastfile) {
    __lastfile->next = (FILE *)f;
    f->file.prev = __lastfile;
    __lastfile = (FILE *)f;
  }
  else
    __firstfile = __lastfile = (FILE *)f;

  return (FILE *)f;
}
