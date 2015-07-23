/*
 * POSIX Compatibility Library for AmigaOS
 *
 * Written by Frank Wille <frank@phoenix.owl.de> in 2005
 *
 * $Id: socket.c,v 1.2 2005/10/19 21:42:00 phx Exp $
 */

#include <errno.h>
#include "fdesc.h"
#include "bsdsocket.h"


int __Psocket(int domain,int type,int proto)
{
  if (__init_bsdsocket(-1) >= 0)
    return __socket2fd(socket(domain,type,proto));

  return -1;
}
