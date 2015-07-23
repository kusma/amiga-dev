/*
 * POSIX Compatibility Library for AmigaOS
 *
 * Written by Frank Wille <frank@phoenix.owl.de> in 2005
 *
 * $Id: fchmod.c,v 1.2 2005/10/17 20:14:51 phx Exp $
 */

#include <errno.h>
#include "prot.h"
#include "fdesc.h"


int fchmod(int fd,mode_t mode)
{
  struct __fd_s *fp;

  errno = 0;
  if (!(fp = __chk_fd(fd)))
    return -1;

  if (fp->flags & FDFL_SOCKET) {
    errno = EINVAL;
    return -1;
  }

  /* AmigaOS filesystems don't allow to modify file attributes when the
     file is locked/opened. So remember new mode until file is closed. */
  fp->flags |= FDFL_MODESET;
  fp->amode = __mode2prot(mode);
  return 0;
}
