/*
 * POSIX Compatibility Library for AmigaOS
 *
 * Written by Frank Wille <frank@phoenix.owl.de> in 2005
 *
 * $Id: getpass.c,v 1.2 2005/04/23 11:30:48 phx Exp $
 */

#pragma amiga-align
#include <exec/types.h>
#include <dos/dos.h>
#include <proto/dos.h>
#pragma default-align
#include <string.h>
#include <errno.h>
#include <pwd.h>


char *getpass(const char *prompt)
{
  static char pwdbuf[_PASSWORD_LEN];
  BPTR in,out;
  int cons=0,len=0;

  if (in = Open("CONSOLE:",MODE_OLDFILE)) {
    out = in;
    cons = 1;
  }
  else {
    in = Input();
    out = Output();
  }
  pwdbuf[0] = '\0';

  if (SetMode(in,DOSTRUE)) {
    Write(out,prompt,strlen(prompt));

    for (;;) {
      if (WaitForChar(in,TICKS_PER_SECOND/2)) {
        char c;

        Read(in,&c,1);
        if (c == '\003') {
          Write(out,"\r\n",2);
          Write(out,prompt,strlen(prompt));
          len = 0;
        }
        else if (c=='\b' || c==127) {
          if (len > 0)
            len--;
        }
        else if (c=='\r' || c=='\n') {
          Write(out,"\r\n",2);
          break;
        }
        else if (len < _PASSWORD_LEN) {
          pwdbuf[len++] = c;
        }
        pwdbuf[len] = '\0';
      }
    }

    CheckSignal(SIGBREAKF_CTRL_C);
    SetMode(in,DOSFALSE);
    if (cons)
      Close(in);
    return pwdbuf;
  }

  return NULL;
}
