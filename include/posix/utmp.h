/*
 * AmiTCP/IP compatibility headers for PosixLib
 *
 * Written by Frank Wille <frank@phoenix.owl.de> in 2005
 *
 * $Id: utmp.h,v 1.1 2005/04/10 19:56:40 phx Exp $
 */

#ifndef _UTMP_H_
#define _UTMP_H_

#define _PATH_UTMP      "AmiTCP:log/utmp"
#define _PATH_WTMP      "AmiTCP:log/wtmp"
#define _PATH_LASTLOG   "AmiTCP:log/lastlog"

#define UT_NAMESIZE     32
#define UT_LINESIZE     32
#define UT_HOSTSIZE     64

struct lastlog {
  long  ll_time;                        /* the login time */
  uid_t ll_uid;                         /* user ID */
  char  ll_name[UT_NAMESIZE];           /* the login name */
  char  ll_host[UT_HOSTSIZE];           /* where the login originated */
};

#define ll_line ll_host

struct utmp {
  long    ut_time;                      /* the login time */
  long    ut_sid;                       /* session ID */
  char    ut_name[UT_NAMESIZE];         /* the login name */
  char    ut_host[UT_HOSTSIZE];         /* where the login originated */
};

#define ut_line ut_host

#endif /* _UTMP_H_ */
