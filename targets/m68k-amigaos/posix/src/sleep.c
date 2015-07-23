/*
 * POSIX Compatibility Library for AmigaOS
 *
 * Written by Frank Wille <frank@phoenix.owl.de> in 2003
 *
 * $Id: sleep.c,v 1.2 2005/02/27 13:45:26 phx Exp $
 */

#pragma amiga-align
#include <dos/dos.h>
#include <proto/dos.h>
#pragma default-align


unsigned int sleep(unsigned int seconds)
{
  Delay(seconds * TICKS_PER_SECOND);
  return 0;
}
