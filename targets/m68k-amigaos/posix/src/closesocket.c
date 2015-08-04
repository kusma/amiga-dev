/*
 * POSIX Compatibility Library for AmigaOS
 *
 * Written by Frank Wille <frank@phoenix.owl.de> in 2005
 *
 * $Id: closesocket.c,v 1.1 2005/10/20 20:36:52 phx Exp $
 */

#include "bsdsocket.h"


void __close_socket(int s)
{
  if (__init_bsdsocket(-1) >= 0) {
    CloseSocket(s);
  }
}
