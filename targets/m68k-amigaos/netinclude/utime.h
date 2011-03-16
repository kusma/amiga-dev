#ifndef	UTIME_H
#define	UTIME_H \
       "$Id: utime.h,v 4.1 1994/10/03 20:52:20 ppessi Exp $"
/*
 *      Definitions and prototype for the utime() (in the net.lib)
 *
 *      Copyright © 1994 AmiTCP/IP Group,
 *                       Network Solutions Development, Inc.
 *                       All rights reserved.
 */

struct utimbuf {
	time_t actime;		/* Access time */
	time_t modtime;		/* Modification time */
};

int utime(const char *, const struct utimbuf *);

#endif /* !UTIME_H */
