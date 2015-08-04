/*
 * POSIX Compatibility Library for AmigaOS
 *
 * Written by Frank Wille <frank@phoenix.owl.de> in 2010
 *
 * $Id: sockstat.c,v 1.1 2010/01/10 17:24:19 phx Exp $
 */

#include <string.h>
#include <errno.h>
#include <sys/stat.h>


int __sockstat(int bsdsock,struct stat *sb)
{
  /* for a socket all fields are zero, except the mode is _S_IFSOCK */
  memset(sb,0,sizeof(struct stat));
  sb->st_mode = _S_IFSOCK;
  errno = 0;
  return 0;
}
