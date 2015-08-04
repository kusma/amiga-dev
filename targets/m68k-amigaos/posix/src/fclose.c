/*
 * POSIX Compatibility Library for AmigaOS
 *
 * Written by Frank Wille <frank@phoenix.owl.de> in 2005-2006
 *
 * $Id: fclose.c,v 1.4 2006/08/09 11:05:32 phx Exp $
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "fdesc.h"

extern FILE *__firstfile,*__lastfile;


int fclose(FILE *f)
{
  if(!f)
    return EOF;
  if (!f->filehandle)
    return EOF;

  fflush(f);
  if (close(((struct extFILE *)f)->fd) < 0)
    return EOF;

  if (f->prev) f->prev->next = f->next; else __firstfile = f->next;
  if (f->next) f->next->prev = f->prev; else __lastfile = f->prev;
  if (f->base && !(f->flags&_NOTMYBUF)) free(f->base-1);
  if (((struct extFILE *)f)->lbuf_base)
    free(((struct extFILE *)f)->lbuf_base);
  free(f);
  return 0;
}
