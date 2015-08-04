/*
 * POSIX Compatibility Library for AmigaOS
 *
 * Written by Frank Wille <frank@phoenix.owl.de> in 2010
 *
 * $Id: sockstat.h,v 1.1 2010/01/10 17:24:19 phx Exp $
 */

#include <sys/stat.h>

int __sockstat(int,struct stat *);
