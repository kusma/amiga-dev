/*
 * POSIX Compatibility Library for AmigaOS
 *
 * Written by Frank Wille <frank@phoenix.owl.de> in 2003
 *
 * $Id: dup2.c,v 1.3 2005/04/23 11:21:56 phx Exp $
 */

#include "fdesc.h"
#include "bsdsocket.h"


int dup2(int oldd,int newd)
{
  struct __fd_s *oldfp,*newfp;

  if (!(oldfp = __chk_fd(oldd)))
    return -1;

  if (newfp = __fdesc[newd]) 
    __close_fdesc(newfp);

  if (oldfp->flags & FDFL_SOCKET) {
    if (__init_bsdsocket(-1) >= 0) {
      struct __fd_s *newfp;
      int dupsock;

      dupsock = Dup2Socket(oldfp->file,-1);
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
    __fdesc[newd] = oldfp;
    oldfp->nestcnt++;
  }
  return newd;
}
