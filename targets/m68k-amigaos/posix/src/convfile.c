/*
 * POSIX Compatibility Library for AmigaOS
 *
 * Written by Frank Wille <frank@phoenix.owl.de> in 2003
 *
 * $Id: convfile.c,v 1.4 2005/11/01 12:09:51 phx Exp $
 */

#include <stdlib.h>
#include <sys/syslimits.h>


char *__convert_path(const char *path)
{
  static char buf[PATH_MAX];
  int i=0;
  char c;

  if (path == NULL)
    return NULL;

  if (*path == '.') {
    if (*(path+1) == '/')
      path += 2;
    else if (*(path+1) == '\0')
      path++;
    else if (*(path+1)=='.' && *(path+2)=='/')
      path += 2;
  }

  while (c = *path++) {
    if (i) {
      if (i >= PATH_MAX)
        break;
      if (c=='.' && *path=='.' && *(path+1)=='/' &&
          (*(path-2)=='/' || *(path-2)==':')) {
        path++;
        continue;
      }
    }
    buf[i++] = c;
  }
  buf[i] = '\0';

  return buf;
}
