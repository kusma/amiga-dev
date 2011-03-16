#ifndef	UTMP_H
#define	UTMP_H \
       "$Id: utmp.h,v 4.1 1994/10/03 20:52:20 ppessi Exp $"
/*
 *	Definitions of utmp and lastlog structures for 32 bit C compilers
 *
 *      Copyright © 1994 AmiTCP/IP Group,
 *                       Network Solutions Development, Inc.
 *                       All rights reserved.
 */

#define	_PATH_UTMP	"AmiTCP:log/utmp"
#define	_PATH_WTMP	"AmiTCP:log/wtmp"
#define	_PATH_LASTLOG	"AmiTCP:log/lastlog"

#define	UT_NAMESIZE	32
#define	UT_LINESIZE	32
#define	UT_HOSTSIZE	64

struct lastlog {
  long	ll_time;		        /* the login time */
  uid_t ll_uid;		                /* user ID */
  char  ll_name[UT_NAMESIZE];		/* the login name */
  char	ll_host[UT_HOSTSIZE];		/* where the login originated */
};

#define ll_line ll_host

struct utmp {
  long	  ut_time;		        /* the login time */
  long    ut_sid;			/* session ID */
  char    ut_name[UT_NAMESIZE];		/* the login name */
  char    ut_host[UT_HOSTSIZE];		/* where the login originated */
};

#define ut_line ut_host

#endif /* !UTMP_H */
