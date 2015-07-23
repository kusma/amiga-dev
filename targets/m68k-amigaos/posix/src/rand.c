/*
 * POSIX Compatibility Library for AmigaOS
 *
 * Written by Frank Wille <frank@phoenix.owl.de> in 2006
 *
 * $Id: rand.c,v 1.1 2006/03/12 21:30:17 phx Exp $
 */

#include <stdlib.h>

static unsigned long next = 1;


int rand(void)
{
  return (int)((next = next * 1103515245 + 12345) %
               ((unsigned long)RAND_MAX + 1));
}


void srand(unsigned int seed)
{
  next = seed;
}
