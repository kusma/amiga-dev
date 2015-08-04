/*
 * POSIX Compatibility Library for AmigaOS
 *
 * Written by Frank Wille <frank@phoenix.owl.de> in 2006
 *
 * $Id: asprintf.c,v 1.1 2006/08/09 11:05:32 phx Exp $
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#define BUFINC 256


int asprintf(char **ret,const char *format,...)
{
  size_t size = 0;
  char *buf = 0;
  int n;
  va_list args;

  va_start(args,format);
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

  va_end(args);
  *ret = realloc(buf,n);
  return *ret ? n : -1;
}
