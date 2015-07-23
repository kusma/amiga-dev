/*
 * POSIX Compatibility Library for AmigaOS
 *
 * Written by Frank Wille <frank@phoenix.owl.de> in 2005
 *
 * $Id: bsdsocket.c,v 1.8 2005/10/20 20:36:51 phx Exp $
 */

#pragma amiga-align
#include <exec/libraries.h>
#include <libraries/bsdsocket.h>
#include <workbench/startup.h>
#include <proto/exec.h>
#include <proto/dos.h>
#pragma default-align
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include "fdesc.h"
#include "bsdsocket.h"

struct Library *SocketBase = NULL;
#ifdef __amigaos4__
struct SocketIFace *ISocket;
#endif


void _EXIT_4_bsdsocket(void)
{
  #ifdef __amigaos4__
  if (ISocket)
    DropInterface((struct Interface *)ISocket);
  #endif
  if (SocketBase)
    CloseLibrary(SocketBase);
}


int __init_bsdsocket(int s)
{
  static char prgname[256];
  struct __fd_s *fp;

  errno = EACCES; /* @@@ */
  if (!SocketBase) {
    struct TagItem tags[3];

    if (SocketBase = OpenLibrary("bsdsocket.library",4)) {
      #ifdef __amigaos4__
      if (!(ISocket = (struct SocketIFace *)
                       GetInterface(SocketBase,"main",1,0))) {
        SocketBase = NULL;
        return -1;
      }
      #endif
      tags[0].ti_Tag = SBTM_SETVAL(SBTC_ERRNOPTR(sizeof(errno)));
      tags[0].ti_Data = (ULONG)&errno;
      tags[1].ti_Tag = SBTM_SETVAL(SBTC_LOGTAGPTR);
      tags[1].ti_Data = (ULONG)getprogname();
      tags[2].ti_Tag = TAG_DONE;
      if (SocketBaseTagList(tags) != 0) {
        #ifdef __amigaos4__
        DropInterface((struct Interface *)ISocket);
        #endif
        CloseLibrary(SocketBase);
        SocketBase = NULL;
        return -1;
      }
    }
    else
      return -1;
  }

  errno = 0;

  if (s != -1) {
    /* check if 's' is a socket and return AmiTCP socket-fd */
    if (fp = __chk_fd(s)) {
      if (fp->flags & FDFL_SOCKET) {
        return fp->file;
      }
      else
        errno = ENOTSOCK;
    }
    return -1;
  }

  return 0;
}
