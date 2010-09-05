/*
 * POSIX Compatibility Library for AmigaOS
 *
 * Written by Frank Wille <frank@phoenix.owl.de> in 2003
 *
 * $Id: filio.h,v 1.1 2005/04/11 21:46:13 phx Exp $
 */

#ifndef _SYS_FILIO_H_
#define _SYS_FILIO_H_

#include <sys/ioccom.h>

/* Generic file-descriptor ioctl's. */
#define FIOCLEX          _IO('f', 1)            /* set close on exec on fd */
#define FIONCLEX         _IO('f', 2)            /* remove close on exec */
#define FIONREAD        _IOR('f', 127, int)     /* get # bytes to read */
#define FIONBIO         _IOW('f', 126, int)     /* set/clear non-blocking i/o *
#define FIOASYNC        _IOW('f', 125, int)     /* set/clear async i/o */
#define FIOSETOWN       _IOW('f', 124, int)     /* set owner */
#define FIOGETOWN       _IOR('f', 123, int)     /* get owner */

#endif  /* _SYS_FILIO_H_ */
