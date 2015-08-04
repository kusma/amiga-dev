/*
 * POSIX Compatibility Library for AmigaOS
 *
 * Written by Frank Wille <frank@phoenix.owl.de> in 2003
 *
 * $Id: getcwd.c,v 1.3 2006/06/02 11:56:10 phx Exp $
 */

#pragma amiga-align
#include <dos/dos.h>
#include <proto/dos.h>
#pragma default-align
#include <stdlib.h>
#include <errno.h>
#include <sys/param.h>


char *getcwd(char *dest,size_t size)
{
  char *buf = dest;
  BPTR cwd;

  errno = 0;
  if (buf == NULL) {
    /* @@@ we would need to determine the real path-length here,
       instead just allocating a buffer for MAXPATHLEN */
    if ((buf = malloc(MAXPATHLEN)) == NULL) {
      errno = ENOMEM;
      return NULL;
    }
    size = MAXPATHLEN;
  }
  if (size == 0) {
    errno = EINVAL;
    return NULL;
  }

  if (cwd = Lock("",SHARED_LOCK)) {
    if (!NameFromLock(cwd,buf,size)) {
      if (IoErr() == ERROR_LINE_TOO_LONG)
        errno = ERANGE; /* path doesn't fit into buffer */
      else
        errno = EACCES; /* @@@ */
    }
    UnLock(cwd);
  }
  else
    errno = EACCES; /* @@@ */

  if (errno) {
    if (dest == NULL)
      free(buf);
    buf = NULL;
  }
  return buf;
}
