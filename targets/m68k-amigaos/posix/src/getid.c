/*
 * POSIX Compatibility Library for AmigaOS
 *
 * Written by Frank Wille <frank@phoenix.owl.de> in 2003
 *
 * $Id: getid.c,v 1.2 2005/02/27 13:45:24 phx Exp $
 */

#include <sys/types.h>


uid_t getuid(void)
{
  return 0;
}


uid_t geteuid(void)
{
  return 0;
}


gid_t getgid(void)
{
  return 0;
}


gid_t getegid(void)
{
  return 0;
}
