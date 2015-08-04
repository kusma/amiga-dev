/*
 * POSIX Compatibility Library for AmigaOS
 *
 * Written by Frank Wille <frank@phoenix.owl.de> in 2006
 *
 * $Id: strsep.c,v 1.1 2006/06/02 22:16:18 phx Exp $
 */

#include <string.h>


char *strsep(char **strptr,const char *delimstr)
{
  char *s;
  const char *spanptr;
  int c,spc;
  char *token;

  if ((s = *strptr) == NULL)
    return NULL;

  for (token=s; ;) {
    c = *s++;
    spanptr = delimstr;

    do {
      if ((spc = *spanptr++) == c) {
        if (c == 0)
          s = NULL;
        else
          s[-1] = '\0';
        *strptr = s;
        return token;
      }
    }
    while (spc != 0);
  }
}
