/*
 * POSIX Compatibility Library for AmigaOS
 *
 * Written by Frank Wille <frank@phoenix.owl.de> in 2003
 *
 * $Id: setid.c,v 1.2 2005/02/27 13:45:26 phx Exp $
 */

#include <sys/types.h>


int setuid(uid_t uid)
{
  return -1;
}


int seteuid(uid_t euid)
{
  return -1;
}


int setgid(gid_t gid)
{
  return -1;
}


int setegid(gid_t egid)
{
  return -1;
}
