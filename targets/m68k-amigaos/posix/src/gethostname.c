/*
 * POSIX Compatibility Library for AmigaOS
 *
 * Written by Frank Wille <frank@phoenix.owl.de> in 2005
 *
 * $Id: gethostname.c,v 1.1 2005/04/10 19:56:36 phx Exp $
 */

#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "bsdsocket.h"


int __Pgethostname(char *name, size_t len)
{
  if (__init_bsdsocket(-1) >= 0) {
    return gethostname(name,len);
  }
  else {
    char *p;

    if (p = getenv("HOSTNAME")) {
      if (strlen(p)+1 <= len) {
        strcpy(name,p);
        return 0;
      }
    }
    errno = EFAULT;
  }

  return -1;
}
