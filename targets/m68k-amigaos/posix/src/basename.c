/*
 * POSIX Compatibility Library for AmigaOS
 *
 * Written by Frank Wille <frank@phoenix.owl.de> in 2005
 *
 * $Id: basename.c,v 1.1 2005/10/29 17:10:34 phx Exp $
 */

#include <limits.h>
#include <string.h>
#include <sys/syslimits.h>


char *basename(char *path)
{
  static char singledot[] = ".";
  static char result[PATH_MAX];
  char *p,*lastp;
  size_t len;

  if (!path || *path=='\0')
    return singledot;

  lastp = path + strlen(path) - 1;
  while (lastp!=path && *lastp=='/')
    lastp--;

  p = lastp;
  while (p!=path && *(p-1)!='/' && *(p-1)!=':')
    p--;

  len = (lastp - p) + 1;
  if (len > (PATH_MAX - 1))
    len = PATH_MAX - 1;

  memcpy(result,p,len);
  result[len] = '\0';

  return result;
}
