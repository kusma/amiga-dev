/*
 * POSIX Compatibility Library for AmigaOS
 *
 * Written by Frank Wille <frank@phoenix.owl.de> in 2003
 *
 * $Id: ioctl.h,v 1.1 2005/04/11 21:46:13 phx Exp $
 */

#ifndef _SYS_IOCTL_H_
#define _SYS_IOCTL_H_

/* @@@ tty stuff */

#include <sys/filio.h>
#include <sys/sockio.h>
/* @@@ other io includes are missing */

#ifndef _KERNEL
int ioctl(int, unsigned long, ...);
#endif

#endif  /* _SYS_IOCTL_H_ */
