/*
 * POSIX Compatibility Library for AmigaOS
 *
 * Written by Frank Wille <frank@phoenix.owl.de> in 2003-2004
 *
 * $Id: killpg.c,v 1.1 2004/11/28 21:29:08 phx Exp $
 */

#include <sys/types.h>
#include <stdlib.h>
#include <errno.h>
#pragma amiga-align
#include <dos/dosextens.h>
#include <proto/dos.h>
#include <proto/exec.h>
#pragma default-align


int killpg(pid_t pid,int signo)
{
  /* @@@ not supported? */
  errno = ESRCH;
  return -1;
}
