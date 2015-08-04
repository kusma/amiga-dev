/*
 * POSIX Compatibility Library for AmigaOS
 *
 * Written by Frank Wille <frank@phoenix.owl.de> in 2005
 *
 * $Id: getlogin.c,v 1.1 2005/04/23 19:52:46 phx Exp $
 */

#pragma amiga-align
#include <dos/dos.h>
#include <dos/var.h>
#include <proto/dos.h>
#pragma default-align
#include <string.h>
#include <errno.h>

static char username[256];


char *getlogin(void)
{
  int i;

  if (GetVar("USER",username,sizeof(username),0) <= 0) {
    if (GetVar("LOGUSER",username,sizeof(username),0) <= 0) {
      if (GetVar("USERNAME",username,sizeof(username),0) <= 0) {
        strcpy(username,"anonymous");
      }
    }
  }

  for (i=strlen(username)-1; i>=0; i--) {
    if (username[i]==' ' || username[i]=='\t' ||
        username[i]=='\r' || username[i]=='\n')
      username[i] = '\0';
    else
      break;
  }

  return username;
}


int setlogin(const char *name)
{
  /* should we allow to set username? */
  errno = EPERM;
  return -1;
}
