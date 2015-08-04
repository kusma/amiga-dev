/*
 * POSIX Compatibility Library for AmigaOS
 *
 * Written by Frank Wille <frank@phoenix.owl.de> in 2006
 *
 * $Id: strlcpy.c,v 1.1 2006/06/02 22:16:18 phx Exp $
 */

#include <string.h>


size_t strlcpy(char *dptr,const char *sptr,size_t siz)
{
  char *d = dptr;
  const char *s = sptr;
  size_t n = siz;

  if (n!=0 && --n!=0) {
    do {
      if ((*d++ = *s++) == 0)
        break;
    }
    while (--n != 0);
  }
  if (n == 0) {
    if (siz != 0)
      *d = '\0';
    while (*s++);
  }
  return s - sptr - 1;
}
