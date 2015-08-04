/*
 * POSIX Compatibility Library for AmigaOS
 *
 * Written by Frank Wille <frank@phoenix.owl.de> in 2003-2004
 *
 * $Id: kill.c,v 1.2 2006/08/08 16:58:08 phx Exp $
 */

#include <sys/types.h>
#include <signal.h>
#include <stdlib.h>
#include <errno.h>
#pragma amiga-align
#include <dos/dosextens.h>
#include <proto/dos.h>
#include <proto/exec.h>
#pragma default-align

extern void __exit(int);


int kill(pid_t pid,int signo)
{
  struct Process *p;
  int rc = -1;

  errno = 0;
  Forbid();

  if (pid > 0) {
    if (p = FindCliProc((ULONG)pid)) {
      if (signo==SIGTERM || signo==SIGINT) {
        if (FindTask(NULL) == (struct Task *)p) {
          /* kill myself */
          Permit();
          __exit(EXIT_FAILURE);
        }
        Signal((struct Task *)p,SIGBREAKF_CTRL_C);
        rc = 0;
      }
      else {
        /* @@@ handle other signals? */
        errno = EINVAL;
      }
    }
    else {
      errno = ESRCH;
    }
  }
  else {
    /* @@@ handle pid==0, pid<0 */
    errno = ESRCH;  /* @@@ */
  }

  Permit();
  return rc;
}
