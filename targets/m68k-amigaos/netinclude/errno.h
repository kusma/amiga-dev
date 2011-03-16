#ifndef ERRNO_H
#define ERRNO_H \
       "$Id: errno.h,v 4.1 1994/10/03 20:52:20 ppessi Exp $"
/*
 *      errno for Network Programs
 *
 *      Copyright © 1994 AmiTCP/IP Group, Network Solutions Development, Inc.
 *                       All rights reserved.
 */

#include <sys/errno.h>

extern int errno;
extern int _OSERR;

#define __sys_nerr 80

#ifndef STRICT_ANSI
#define sys_nerr __sys_nerr
#endif

#endif /* ERRNO_H */
