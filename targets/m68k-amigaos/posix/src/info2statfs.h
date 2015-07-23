/*
 * POSIX Compatibility Library for AmigaOS
 *
 * Written by Frank Wille <frank@phoenix.owl.de> in 2003
 *
 * $Id: info2statfs.h,v 1.2 2005/02/27 13:45:25 phx Exp $
 */

#include <sys/mount.h>

int __info2statfs(struct InfoData *,struct statfs *);
