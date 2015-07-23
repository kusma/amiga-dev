/*
 * POSIX Compatibility Library for AmigaOS
 *
 * Written by Frank Wille <frank@phoenix.owl.de> in 2003-2004
 *
 * $Id: getpid.c,v 1.2 2004/11/28 21:29:07 phx Exp $
 */

#include <sys/types.h>
#pragma amiga-align
#include <dos/dosextens.h>
#include <proto/exec.h>
#pragma default-align


pid_t getpid(void)
{
#ifdef __amigaos4__NOT_YET  /* @@@ how to find such an ID? */
  return ((struct Process *)FindTask(NULL))->pr_ProcessID;
#else
  return ((struct Process *)FindTask(NULL))->pr_TaskNum;
#endif
}
