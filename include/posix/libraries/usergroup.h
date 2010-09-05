/*
 * AmiTCP/IP compatibility headers for PosixLib
 *
 * Written by Frank Wille <frank@phoenix.owl.de> in 2005
 *
 * $Id: usergroup.h,v 1.1 2005/04/10 19:56:41 phx Exp $
 */

#ifndef LIBRARIES_USERGROUP_H
#define LIBRARIES_USERGROUP_H

#ifndef EXEC_TYPES_H
#include <exec/types.h>
#endif

#ifndef _PWD_H_
#include <pwd.h>
#endif 
#ifndef _GRP_H_
#include <grp.h>
#endif
#ifndef _UTMP_H_
#include <utmp.h>
#endif

#ifndef pid_t
/* @@@ was "struct Task *" in AmiTCP headers !!! */
typedef __pid_t         pid_t;          /* process id */
#define pid_t           __pid_t
#endif

#ifndef mode_t
typedef __mode_t        mode_t;         /* permissions */
#define mode_t          __mode_t
#endif

#define USERGROUPNAME "AmiTCP:libs/usergroup.library"

/* Extended password encryption begins with underscore */
#define _PASSWORD_EFMT1 '_'

/* Maximum length for password */
#define _PASSWORD_LEN   128

/* A user can belong to NGROUPS different groups */
#define NGROUPS 32

/* Max length of a login name */
#define MAXLOGNAME      32

/* Credentials of a process */
#pragma amiga-align
struct UserGroupCredentials {
  uid_t   cr_ruid;
  gid_t   cr_rgid;
#if 0 /* @@@ mode_t is 16 bits in AmiTCP! */
  mode_t  cr_umask;             /* umask */
#else
  unsigned short cr_umask;
#endif
  uid_t   cr_euid;
  short   cr_ngroups;           /* number of groups */
  gid_t   cr_groups[NGROUPS];
  pid_t   cr_session;
  char    cr_login[MAXLOGNAME]; /* setlogin() name */
};
#pragma default-align

/*
 * ID conversion macros
 */
#define UG2MU(id) ((id) == 0 ? 65535 : (id) == -2 ? 0 : (id))
#define MU2UG(id) ((id) == 65535 ? 0L : (id) == 0L ? -2L : (id))

/*
 * Context tags
 */
#define UGT_ERRNOBPTR 0x80000001
#define UGT_ERRNOWPTR 0x80000002
#define UGT_ERRNOLPTR 0x80000004
#define UGT_ERRNOPTR(size)\
  ((size == 4) ? UGT_ERRNOLPTR :\
   (size == 2) ? UGT_ERRNOWPTR :\
   (size == 1) ? UGT_ERRNOBPTR : 1L)
#define UGT_OWNER     0x80000011
#define UGT_INTRMASK  0x80000010

#endif /* LIBRARIES_USERGROUP_H */
