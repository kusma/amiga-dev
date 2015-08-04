/*
 * POSIX Compatibility Library for AmigaOS
 *
 * Written by Frank Wille <frank@phoenix.owl.de> in 2005
 *
 * $Id: getgrnam.c,v 1.1 2005/04/23 19:52:46 phx Exp $
 */

#include <string.h>
#include <stdlib.h>
#include <grp.h>
#include <unistd.h>


struct group *getgrgid(gid_t gid)
{
  static struct group gr;

  /* @@@ */

  return NULL;
}


struct group *getgrnam(const char *name)
{
  return NULL;
}
