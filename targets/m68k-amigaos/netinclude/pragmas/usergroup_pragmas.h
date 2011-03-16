/*
**	$Filename: pragmas/usergroup_pragmas.h $
**	$Release$
**	$Revision: 4.2 $
**	$Date: 1994/10/05 20:30:38 $
**
**	Pragmas file for usergroup.library
**
**	Copyright © 1994 AmiTCP/IP Group, 
**	                 Network Solutions Development Inc.
**	                 All rights reserved.
*/
/* Setup functions */
#pragma libcall UserGroupBase ug_SetupContextTagList 1E 9802
#pragma tagcall UserGroupBase ug_SetupContextTags 1E 9802
#pragma libcall UserGroupBase ug_GetErr 24 00
#pragma libcall UserGroupBase ug_StrError 2A 101
/* User identification */
#pragma libcall UserGroupBase getuid 30 00
#pragma libcall UserGroupBase geteuid 36 00
#pragma libcall UserGroupBase setreuid 3C 1002
#pragma libcall UserGroupBase setuid 42 001
/* Group membership */
#pragma libcall UserGroupBase getgid 48 00
#pragma libcall UserGroupBase getegid 4E 00
#pragma libcall UserGroupBase setregid 54 1002
#pragma libcall UserGroupBase setgid 5A 001
#pragma libcall UserGroupBase getgroups 60 9002
#pragma libcall UserGroupBase setgroups 66 9002
#pragma libcall UserGroupBase initgroups 6C 0902
/* User database */
#pragma libcall UserGroupBase getpwnam 72 901
#pragma libcall UserGroupBase getpwuid 78 001
#pragma libcall UserGroupBase setpwent 7E 00
#pragma libcall UserGroupBase getpwent 84 00
#pragma libcall UserGroupBase endpwent 8A 00
/* Group database */
#pragma libcall UserGroupBase getgrnam 90 901
#pragma libcall UserGroupBase getgrgid 96 001
#pragma libcall UserGroupBase setgrent 9C 00
#pragma libcall UserGroupBase getgrent A2 00
#pragma libcall UserGroupBase endgrent A8 00
/* Password handling */
#pragma libcall UserGroupBase crypt AE 9802
#pragma libcall UserGroupBase ug_GetSalt B4 09803
#pragma libcall UserGroupBase getpass BA 901
/* Default protections */
#pragma libcall UserGroupBase umask C0 001
#pragma libcall UserGroupBase getumask C6 00
/* Sessions */
#pragma libcall UserGroupBase setsid CC 00
#pragma libcall UserGroupBase getpgrp D2 00
#pragma libcall UserGroupBase getlogin D8 00
#pragma libcall UserGroupBase setlogin DE 901
/* Sessions, user login database (utmp) */
#pragma libcall UserGroupBase setutent E4 00
#pragma libcall UserGroupBase getutent EA 00
#pragma libcall UserGroupBase endutent F0 00
#pragma libcall UserGroupBase getlastlog F6 001
#pragma libcall UserGroupBase setlastlog FC 98003
/* Credentials */
#pragma libcall UserGroupBase getcredentials 102 801
