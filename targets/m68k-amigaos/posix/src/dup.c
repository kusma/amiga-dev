/*
 * POSIX Compatibility Library for AmigaOS
 *
 * Written by Frank Wille <frank@phoenix.owl.de> in 2003-2005
 *
 * $Id: dup.c,v 1.3 2005/04/23 11:02:02 phx Exp $
 */

#include "fdesc.h"
#include "bsdsocket.h"


int dup(int oldd)
{
  struct __fd_s *fp;
  int newd;

  if (!(fp = __chk_fd(oldd)))
    return -1;
  if ((newd = __next_free_fd(0)) < 0)
    return -1;

  if (fp->flags & FDFL_SOCKET) {
    if (__init_bsdsocket(-1) >= 0) {
      struct __fd_s *newfp;
      int dupsock;

      dupsock = Dup2Socket(fp->file,-1);
      if (dupsock >= 0) {
        if (newfp = __new_fdesc()) {
          newfp->flags = FDFL_SOCKET | FDFL_INTERACTIVE;
          newfp->file = dupsock;
          __fdesc[newd] = newfp;
          return newd;
        }
        else
          CloseSocket(dupsock);
      }
    }
    return -1;
  }

  else {
    __fdesc[newd] = fp;
    fp->nestcnt++;
  }
  return newd;
}
