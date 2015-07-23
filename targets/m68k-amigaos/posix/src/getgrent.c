/*
 * POSIX Compatibility Library for AmigaOS
 *
 * Written by Frank Wille <frank@phoenix.owl.de> in 2005
 *
 * $Id: getgrent.c,v 1.1 2005/04/23 19:52:46 phx Exp $
 */

#include <grp.h>

static int grindex = 0;


void setgrent(void)
{
  grindex = 0;
}


struct group *getgrent(void)
{
  return 0;
}


void endgrent(void)
{
}
