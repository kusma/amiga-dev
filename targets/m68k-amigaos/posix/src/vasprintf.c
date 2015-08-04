/*
 * POSIX Compatibility Library for AmigaOS
 *
 * Written by Frank Wille <frank@phoenix.owl.de> in 2006
 *
 * $Id: vasprintf.c,v 1.1 2006/08/09 11:05:34 phx Exp $
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#define BUFINC 256


int vasprintf(char **ret,const char *format,va_list args)
{
  size_t size = 0;
  char *buf = 0;
  int n;

  do {
    size += BUFINC;
    buf = realloc(buf,size);
    if (!buf) {
      *ret = 0;
      return -1;
    }
    n = vsnprintf(buf,size,format,args);
  }
  while (n >= size);

  *ret = realloc(buf,n);
  return *ret ? n : -1;
}
