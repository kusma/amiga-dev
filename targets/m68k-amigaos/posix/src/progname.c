/*
 * POSIX Compatibility Library for AmigaOS
 *
 * Written by Frank Wille <frank@phoenix.owl.de> in 2005
 *
 * $Id: progname.c,v 1.2 2005/04/25 21:00:08 phx Exp $
 */

#pragma amiga-align
#include <workbench/startup.h>
#include <proto/dos.h>
#pragma default-align
#include <string.h>

char __progname[256];
extern struct WBStartup *WBenchMsg;  /* from startup-code */


void _INIT_3_progname(void)
{
  if (!GetProgramName(__progname,256)) {
  	if(WBenchMsg) {
      /* @@@ make sure that wa_Name doesn't exceed buffer size!!! */
      strcpy(__progname,(char *)WBenchMsg->sm_ArgList[0].wa_Name);
    }
  }
}


char *getprogname(void)
{
  return __progname;
}


void setprogname(const char *name)
{
  if (strlen(name) < 256)
    strcpy(__progname,name);
}
