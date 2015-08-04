/*
 * POSIX Compatibility Library for AmigaOS
 *
 * Written by Frank Wille <frank@phoenix.owl.de> in 2006
 *
 * $Id: fgetln.c,v 1.1 2006/08/09 11:05:33 phx Exp $
 */

#include <stdio.h>
#include <stdlib.h>
#include "fdesc.h"

#define BUFINC 32


static char *getbuf(struct extFILE *f,size_t len)
{
  if (len >= f->lbuf_size) {
    f->lbuf_size += BUFINC;
    f->lbuf_base = realloc(f->lbuf_base,f->lbuf_size);
  }
  return f->lbuf_base;
}


char *fgetln(FILE *fp,size_t *lenp)
{
  size_t len = 0;
  int c,oerr;
  char *line;

  oerr = fp->flags & _ERR;
  do {
    line = getbuf((struct extFILE *)fp,len);
    c = getc(fp);
    if (c == EOF)
      break;
    line[len++] = c;
  }
  while (c != '\n');
  if (fp->flags & _ERR)
    return 0;
  fp->flags |= oerr;
  *lenp = len;
  return line;
}
