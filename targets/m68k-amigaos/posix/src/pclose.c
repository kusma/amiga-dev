/*
 * POSIX Compatibility Library for AmigaOS
 *
 * Written by Frank Wille <frank@phoenix.owl.de> in 2005
 *
 * $Id: pclose.c,v 1.2 2005/07/21 20:01:40 phx Exp $
 */

#include <stdio.h>


int pclose(FILE *fp)
{
  if (fp) {
    /* @@@ we need to wait until the process terminates, and return
       it's exit code @@@ */
    fclose(fp);
    return 0;
  }
  return -1;
}
