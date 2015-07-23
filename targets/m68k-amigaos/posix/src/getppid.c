/*
 * POSIX Compatibility Library for AmigaOS
 *
 * Written by Frank Wille <frank@phoenix.owl.de> in 2003-2004
 *
 * $Id: getppid.c,v 1.1 2004/11/28 15:07:28 phx Exp $
 */

#include <sys/types.h>
#pragma amiga-align
#include <dos/dosextens.h>
#include <proto/exec.h>
#pragma default-align


pid_t getppid(void)
{
  return 0;  /*@@@ how to determine the parent process? */
}
