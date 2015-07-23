/*
 * POSIX Compatibility Library for AmigaOS
 *
 * Written by Frank Wille <frank@phoenix.owl.de> in 2006
 *
 * $Id: _stdio.c,v 1.3 2006/11/20 20:13:00 phx Exp $
 */

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include "fdesc.h"

FILE *stdin,*stdout,*stderr;

static struct extFILE **__stdio[3] = {
  (struct extFILE **)&stdin,
  (struct extFILE **)&stdout,
  (struct extFILE **)&stderr
};

extern FILE *__firstfile,*__lastfile; /* from vclib */


static void init_stdio(int fd)
{
  struct extFILE *sio;
  struct __fd_s *fp;

  if (!(fp = __chk_fd(fd)))
    exit(EXIT_FAILURE);

  sio = *__stdio[fd] = malloc(sizeof(struct extFILE));
  sio->fd = fd;
  sio->lbuf_base = 0;
  sio->lbuf_size = 0;
  sio->file.filehandle = (char *)fp->file;
  sio->file.pointer = 0;
  sio->file.base = 0;
  sio->file.count = 0;
  sio->file.bufsize = 0;

  switch (fp->open_flags & O_ACCMODE) {
    case O_RDONLY: sio->file.flags = _READABLE; break;
    case O_WRONLY: sio->file.flags = _WRITEABLE; break;
    case O_RDWR: sio->file.flags = _READABLE|_WRITEABLE; break;
  }

  if (fp->flags & FDFL_INTERACTIVE) {
    sio->file.flags |= _ISTTY;
    if (sio->file.flags & _WRITEABLE)
      sio->file.flags |= _LINEBUF;
    else
      sio->file.flags |= _UNBUF;
  }
}


void _INIT_6_stdio(void)
{
  init_stdio(0);
  init_stdio(1);
  init_stdio(2);

  stdin->prev = 0;
  stdin->next = stdout;
  stdout->prev = stdin;
  stdout->next = stderr;
  stderr->prev = stdout;
  stderr->next = 0;
  __firstfile = stdin;
  __lastfile = stderr;
}


void _EXIT_7_stdio(void)
{
  fflush(0);
}
