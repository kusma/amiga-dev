#ifndef PWD_H
#define PWD_H \
       "$Id: pwd.h,v 4.1 1994/10/03 20:52:20 ppessi Exp $"
/*
 *	Definitions of uid_t and passwd structure for 32 bit C compilers
 *
 *      Copyright © 1994 AmiTCP/IP Group,
 *                       Network Solutions Development, Inc.
 *                       All rights reserved.
 */

#ifndef SYS_TYPES_H
#include <sys/types.h>
#endif

/* The passwd structure */
struct passwd
{
  char  *pw_name;               /* Username */
  char  *pw_passwd;             /* Encrypted password */
  uid_t  pw_uid;                /* User ID */
  gid_t  pw_gid;                /* Group ID */
  char  *pw_gecos;		/* Real name etc */
  char  *pw_dir;                /* Home directory */
  char  *pw_shell;              /* Shell */
};

struct passwd *getpwuid(uid_t uid);
struct passwd *getpwnam(const char *name);

void setpwent(void);
struct passwd *getpwent(void);
void endpwent(void);

#endif /* PWD_H */
