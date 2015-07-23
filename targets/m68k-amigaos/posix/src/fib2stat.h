/*
 * POSIX Compatibility Library for AmigaOS
 *
 * Written by Frank Wille <frank@phoenix.owl.de> in 2003
 *
 * $Id: fib2stat.h,v 1.2 2005/02/27 13:45:23 phx Exp $
 */

#include <sys/stat.h>

int __fib2stat(struct FileInfoBlock *,struct stat *);
