/*
 * POSIX Compatibility Library for AmigaOS
 *
 * Written by Frank Wille <frank@phoenix.owl.de> in 2006
 *
 * $Id: strlcat.c,v 1.1 2006/06/02 22:16:18 phx Exp $
 */

#include <string.h>


size_t strlcat(char *dptr,const char *sptr,size_t siz)
{
  char *d = dptr;
  const char *s = sptr;
  size_t n = siz;
  size_t dlen;

  while (n--!=0 && *d!='\0')
    d++;
  dlen = d - dptr;
  if ((n = siz - dlen) == 0)
    return dlen + strlen(s);

  while (*s != '\0') {
    if (n != 1) {
      *d++ = *s;
      n--;
    }
    s++;
  }
  *d = '\0';

  return dlen + (s - sptr);
}
