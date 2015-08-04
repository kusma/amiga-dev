/*
 * POSIX Compatibility Library for AmigaOS
 *
 * Written by Frank Wille <frank@phoenix.owl.de> in 2003
 *
 * $Id: creat.c,v 1.2 2005/02/27 13:45:22 phx Exp $
 */

#include <fcntl.h>


int creat(const char *path,mode_t mode)
{
  return open(path,O_CREAT|O_TRUNC|O_WRONLY,mode);
}
