/*
 * POSIX Compatibility Library for AmigaOS
 *
 * Written by Frank Wille <frank@phoenix.owl.de> in 2005-2006
 *
 * $Id: fopen.c,v 1.4 2006/08/09 11:05:33 phx Exp $
 */

#include <stddef.h>
#include <stdio.h>
#include <limits.h>
#include <stdarg.h>
#include <stdlib.h>
#include <fcntl.h>
#include "fdesc.h"

extern FILE *__firstfile,*__lastfile;


FILE *fopen(const char *name,const char *mode)
{
  struct extFILE *f;
  struct __fd_s *fdptr;
  int om,append;

  if (*mode == 'w')
    om = O_CREAT|O_TRUNC|O_WRONLY;
  else
    om = O_RDONLY;
  if (*mode == 'a')
    om = O_CREAT|O_APPEND|O_WRONLY;

  if (!(f = malloc(sizeof(struct extFILE))))
    return(0);
  f->file.count = 0;
  f->file.base = 0;
  f->file.bufsize = 0;
  f->file.next = 0;
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

  f->lbuf_base = 0;
  f->lbuf_size = 0;
  f->fd = open(name,om,0666);
  if (f->fd < 0) {
    free(f);
    return 0;
  }

  if (fdptr = __chk_fd(f->fd)) {
    if (fdptr->flags & FDFL_INTERACTIVE)
      f->file.flags |= _LINEBUF|_ISTTY;
    f->file.filehandle = (char *)fdptr->file;
  }
  else {
    free(f);
    return 0;
  }

  if(__lastfile) {
    __lastfile->next = (FILE *)f;
    f->file.prev = __lastfile;
    __lastfile = (FILE *)f;
  }
  else
    __firstfile = __lastfile = (FILE *)f;

  return (FILE *)f;
}


void _EXIT_6_fopen(void)
{
  while(__firstfile && !fclose(__firstfile));  /* close all open files */
}
