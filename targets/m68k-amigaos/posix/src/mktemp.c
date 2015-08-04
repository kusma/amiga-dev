/*
 * POSIX Compatibility Library for AmigaOS
 *
 * Written by Frank Wille <frank@phoenix.owl.de> in 2005
 *
 * $Id: mktemp.c,v 1.1 2005/03/28 19:24:27 phx Exp $
 */

#pragma amiga-align
#include <dos/dos.h>
#include <proto/dos.h>
#pragma default-align
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include "conv.h"


static int get_temp(char *path,int doopen)
{
  pid_t pid;
  char *start,*end,*p;
  unsigned long long trys;

  errno = 0;
  /* locate XXX pattern to replace */
  for (p=path; *p!='\0' && *p!='X'; p++);
  start = p;
  trys = 1;
  while (*p=='X') {
    *p++ = 'a';
    trys *= 26;
  }
  end = p;

  if (trys > 1) {
    /* encode pid into pattern */
    for (p=start,pid=getpid(); pid>0 && p<end; p++) {
      *p += (char)(pid % 26);
      pid /= 26;
    }
  }

  /* try to open a file with temporary names until successful */
  while (trys--) {
    if (doopen) {
      int fd;

      if ((fd = open(path,O_CREAT|O_EXCL|O_RDWR,0600)) >= 0)
        return fd;
    }
    else {
      BPTR fl;

      if (!(fl = Lock((STRPTR)__convert_path(path),SHARED_LOCK))) {
        if (IoErr() == ERROR_OBJECT_NOT_FOUND)
          return 0;  /* ok, use this path */
      }
      else
        UnLock(fl);
    }

    /* modify path until no file with this name found */
    for (p=end-1; p>=start; p--) {
      if (*p < 'z') {
        *p += 1;
        break;
      }
      else
        *p = 'a';
    }
  }

  /* errno is set by open() and stays 0 otherwise - BSD does the same */

  return -1;  /* failed to make a unique file name with this template */
}


char *mktemp(char *template)
{
  if (get_temp(template,0) != -1)
    return template;
  return 0;
}


int mkstemp(char *template)
{
  return get_temp(template,1);
}
