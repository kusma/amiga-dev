/*
 * POSIX Compatibility Library for AmigaOS
 *
 * Written by Frank Wille <frank@phoenix.owl.de> in 2005
 *
 * $Id: currentdir.c,v 1.2 2005/04/23 21:31:57 phx Exp $
 */

#pragma amiga-align
#include <dos/dos.h>
#include <proto/dos.h>
#pragma default-align


static BPTR orig_dirlock = 0;


void __freeoldcwd(long fl)
{
  if (orig_dirlock == 0) {
    orig_dirlock = (BPTR)fl;
  }
  else {
    if ((BPTR)fl != orig_dirlock)
      UnLock((BPTR)fl);
  }
}


void _EXIT_8_currentdir(void)
{
  BPTR fl;

  if (orig_dirlock != 0) {
    /* reset to original work directory */
    fl = CurrentDir(orig_dirlock);
    if (fl != orig_dirlock)
      UnLock(fl);
  }
}
