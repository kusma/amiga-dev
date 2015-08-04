/*
 * POSIX Compatibility Library for AmigaOS
 *
 * Written by Frank Wille <frank@phoenix.owl.de> in 2003
 *
 * $Id: bzero.c,v 1.2 2005/02/27 13:45:21 phx Exp $
 */

#include <string.h>


void bzero(void *b,size_t len)
{
  memset(b,0,len);
}
