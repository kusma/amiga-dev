#ifndef _INLINE_USERGROUP_CONTEXT_H
#define _INLINE_USERGROUP_CONTEXT_H \
       "$Id: usergroup_context.h,v 1.1 1996/02/16 15:41:39 too Exp $"
/*
 *      Gcc base context defines for usergroup.library. 
 *
 *      Copyright © 1996 AmiTCP/IP Group,
 *                       Network Solutions Development, Inc.
 *                       All rights reserved.
 */

#ifdef BASE_EXT_DECL
#error BASE_EXT_DECL etc declarations not allowed
#endif

#define BASE_EXT_DECL
#define BASE_EXT_DECL0
#define BASE_PAR_DECL struct Library * UserGroupBase,
#define BASE_PAR_DECL0 struct Library * UserGroupBase
#define BASE_NAME UserGroupBase

#include <inline/usergroup.h>

/* BASE_* macros are undefined at the end of <inline/usergroup.h> */

/* Setup functions */
#define ug_SetupContextTagList(r...) ug_SetupContextTagList(UserGroupBase, r)
#define		     ug_GetErr(r)    ug_GetErr(UserGroupBase)
#define		   ug_StrError(r...) ug_StrError(UserGroupBase, r)
/* User identification */
#define			getuid(r)    getuid(UserGroupBase)
#define		       geteuid(r)    geteuid(UserGroupBase)
#define		      setreuid(r...) setreuid(UserGroupBase, r)
#define			setuid(r...) setuid(UserGroupBase, r)
/* Group membership */
#define			getgid(r)    getgid(UserGroupBase)
#define		       getegid(r)    getegid(UserGroupBase)
#define		      setregid(r...) setregid(UserGroupBase, r)
#define			setgid(r...) setgid(UserGroupBase, r)
#define		     getgroups(r...) getgroups(UserGroupBase, r)
#define		     setgroups(r...) setgroups(UserGroupBase, r)
#define		    initgroups(r...) initgroups(UserGroupBase, r)
/* User database */
#define		      getpwnam(r...) getpwnam(UserGroupBase, r)
#define		      getpwuid(r...) getpwuid(UserGroupBase, r)
#define		      setpwent(r)    setpwent(UserGroupBase)
#define		      getpwent(r)    getpwent(UserGroupBase)
#define		      endpwent(r)    endpwent(UserGroupBase)
/* Group database */
#define		      getgrnam(r...) getgrnam(UserGroupBase, r)
#define		      getgrgid(r...) getgrgid(UserGroupBase, r)
#define		      setgrent(r)    setgrent(UserGroupBase)
#define		      getgrent(r)    getgrent(UserGroupBase)
#define		      endgrent(r)    endgrent(UserGroupBase)
/* Password handling */
#define			 crypt(r...) crypt(UserGroupBase, r)
#define		    ug_GetSalt(r...) ug_GetSalt(UserGroupBase, r)
#define		       getpass(r...) getpass(UserGroupBase, r)
/* Default protections */
#define			 umask(r...) umask(UserGroupBase, r)
#define		      getumask(r)    getumask(UserGroupBas)
/* Sessions */
#define			setsid(r)    setsid(UserGroupBase)
#define		       getpgrp(r)    getpgrp(UserGroupBase)
#define		      getlogin(r)    getlogin(UserGroupBase)
#define		      setlogin(r...) setlogin(UserGroupBase, r)
/* Sessions, user login database (utmp) */
#define		      setutent(r)    setutent(UserGroupBase)
#define		      getutent(r)    getutent(UserGroupBase)
#define		      endutent(r)    endutent(UserGroupBase)
#define		    getlastlog(r...) getlastlog(UserGroupBase, r)
#define		    setlastlog(r...) setlastlog(UserGroupBase, r)

#endif /* _INLINE_USERGROUP_CONTEXT_H */
