/*
 * POSIX Compatibility Library for AmigaOS
 *
 * Written by Frank Wille <frank@phoenix.owl.de> in 2005
 *
 * $Id: getpwent.c,v 1.1 2005/04/23 19:52:46 phx Exp $
 */

#include <pwd.h>

static int pwindex = 0;


void setpwent(void)
{
  pwindex = 0;
}


struct passwd *getpwent(void)
{
  if (pwindex == 0) {
    pwindex = 1;
    return getpwuid(0);
  }
  return 0;
}


void endpwent(void)
{
}
