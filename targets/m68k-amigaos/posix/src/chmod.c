/*
 * POSIX Compatibility Library for AmigaOS
 *
 * Written by Frank Wille <frank@phoenix.owl.de> in 2003
 *
 * $Id: chmod.c,v 1.3 2005/03/31 19:03:23 phx Exp $
 */

#include <errno.h>
#include "conv.h"
#include "prot.h"


int chmod(const char *path,mode_t mode)
{
  errno = 0;
  if (__set_prot(__convert_path(path),mode) < 0) {
    errno = EACCES;
    return -1;
  }
  return 0;
}
