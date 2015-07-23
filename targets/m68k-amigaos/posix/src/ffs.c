/*
 * POSIX Compatibility Library for AmigaOS
 *
 * Written by Frank Wille <frank@phoenix.owl.de> in 2003
 *
 * $Id: ffs.c,v 1.2 2005/02/27 13:45:23 phx Exp $
 */

#include "compiler.h"


#if defined(__M68K__) && defined(__VBCC__)

static int ASM __m68k_ffs(REG(d0,int mask)) =
  "\tmove.l\td0,d1\n"
  "\tneg.l\td0\n"
  "\tand.l\td0,d1\n"
  "\tmoveq\t#32,d0\n"
  "\tbfffo\td1{0:32},d1\n"
  "\tsub.l\td1,d0";

int ffs(int mask)
{
  return __m68k_ffs(mask);
}


#else

int ffs(int mask)
{
  int bit;

  if (mask == 0)
    return 0;
  for (bit=1; !(mask&1); bit++)
    mask >>= 1;

  return bit;
}
#endif
