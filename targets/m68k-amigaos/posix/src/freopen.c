/*
 * POSIX Compatibility Library for AmigaOS
 *
 * Written by Frank Wille <frank@phoenix.owl.de> in 2005-2006
 *
 * $Id: freopen.c,v 1.4 2006/06/03 10:25:13 phx Exp $
 */

#include <stddef.h>
#include <stdio.h>
#include <limits.h>
#include <stdarg.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include "fdesc.h"

extern FILE *__firstfile,*__lastfile;


FILE *freopen(const char *name,const char *mode,FILE *f)
{
  struct __fd_s *fdptr;
  int om,append;

  if (f->filehandle) {
    fflush(f);
    if (close(((struct extFILE *)f)->fd) < 0)
      return 0;
    if (f->prev) f->prev->next = f->next; else __firstfile = f->next;
    if (f->next) f->next->prev = f->prev; else __lastfile = f->prev;
    if (f->base && !(f->flags&_NOTMYBUF)) free(f->base-1);
  }

  if (*mode == 'w')
    om = O_CREAT|O_TRUNC|O_WRONLY;
  else
    om = O_RDONLY;
  if (*mode == 'a')
    om = O_CREAT|O_APPEND|O_WRONLY;
  f->count = 0;
  f->base = 0;
  f->bufsize = 0;
  f->next = 0;
  if (*mode == 'r')
    f->flags = _READABLE;
  else
    f->flags = _WRITEABLE;
  if (*++mode == 'b')
    mode++;
  if (*mode == '+') {
    f->flags |= _READABLE|_WRITEABLE;
    om &= ~O_ACCMODE;
    om |= O_RDWR;
  }

  ((struct extFILE *)f)->fd = open(name,om,0666);
  if (((struct extFILE *)f)->fd < 0) {
    free(f);
    return 0;
  }

  if (fdptr = __chk_fd(((struct extFILE *)f)->fd)) {
    if (fdptr->flags & FDFL_INTERACTIVE)
      f->flags |= _LINEBUF|_ISTTY;
    f->filehandle = (char *)fdptr->file;
  }
  else {
    free(f);
    return 0;
  }

  if(__lastfile) {
    __lastfile->next = f;
    f->prev = __lastfile;
    __lastfile = f;
  }
  else
    __firstfile = __lastfile = f;

  return f;
}
