/*
 * POSIX Compatibility Library for AmigaOS
 *
 * Written by Frank Wille <frank@phoenix.owl.de> in 2003
 *
 * $Id: bcopy.c,v 1.2 2005/02/27 13:45:20 phx Exp $
 */

#include <string.h>


void bcopy(const void *src, void *dst, size_t len)
{
  memcpy(src,dst,len);
}
