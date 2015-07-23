/*
 * POSIX Compatibility Library for AmigaOS
 *
 * Written by Frank Wille <frank@phoenix.owl.de> in 2005
 *
 * $Id: getenv.c,v 1.1 2005/04/17 18:36:07 phx Exp $
 */

#pragma amiga-align
#include <dos/dos.h>
#include <dos/var.h>
#include <proto/dos.h>
#pragma default-align
#include <stdlib.h>


/* in contrast to the vclib-function get a new buffer every time! */

char *getenv(const char *name)
{
  char *varbuf = NULL;
  size_t size = 16;
  size_t len;

  do {
    if (varbuf)
      free(varbuf);
    size <<= 1;
    if (varbuf = malloc(size))
      len = GetVar((STRPTR)name,varbuf,size,GVF_BINARY_VAR) + 1;
    else
      return NULL;
  }
  while (len >= size);  /* make sure we got the whole variable */

  if (len == 0) {
    /* doesn't exist */
    free(varbuf);
    return NULL;
  }

  return varbuf;
}
