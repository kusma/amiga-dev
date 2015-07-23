/*
 * POSIX Compatibility Library for AmigaOS
 *
 * Written by Frank Wille <frank@phoenix.owl.de> in 2005
 *
 * $Id: popen.c,v 1.2 2005/07/21 20:01:41 phx Exp $
 */

#pragma amiga-align
#include <dos/dos.h>
#include <dos/dostags.h>
#include <proto/dos.h>
#include <proto/exec.h>
#pragma default-align
#include <stdio.h>
#include <errno.h>
#include "conv.h"


FILE *popen(const char *command,const char *mode)
{
  char pipename[32];
  static ULONG id = 0;
  FILE *fp = 0;
  BPTR pipe=0, null=0;

  if (mode[1]!='\0' || (*mode!='r' && *mode!='w')) {
    errno = EINVAL;
    return NULL;
  }

  if (!id)
    id = (ULONG)FindTask(NULL);
  else
    id++;

  /* this requires a working queue-handler! */
  sprintf(pipename,"PIPE:PosixLib_popen_%08lx",id);
  errno = 0;
  if ((pipe = Open(pipename,MODE_NEWFILE))!=0 &&
      (null = Open("NIL:",MODE_NEWFILE))!=0) {

    switch (SystemTags(__convert_path(command),
                       SYS_Input,*mode=='r'?null:pipe,
                       SYS_Output,*mode=='r'?pipe:null,
                       SYS_Asynch,TRUE,
                       SYS_UserShell,TRUE,
                       TAG_END)) {
      case 0:
        fp = fopen(pipename,mode);
        pipe = null = 0;
        break;
      case -1:
        errno = ENOMEM;
        break;
      default:
        errno = EIO;
        break;
    }
  }
  else
    errno = EIO;

  if (null)
    Close(null);
  if (pipe)
    Close(pipe);

  return fp;
}
