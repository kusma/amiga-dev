/*
 * AmiTCP/IP compatibility headers for PosixLib
 *
 * Written by Frank Wille <frank@phoenix.owl.de> in 2005
 *
 * $Id: pwd.h,v 1.2 2005/04/23 18:26:56 phx Exp $
 */

#ifndef _PWD_H_
#define _PWD_H_

#ifndef _SYS_TYPES_H_
#include <sys/types.h>
#endif

#if !defined(_POSIX_C_SOURCE)
#define _PASSWORD_LEN           128     /* max length, not counting NUL */ 
#endif

/* structure is compatible with usergroup.library,
   although not required at the moment */

struct passwd
{
  char  *pw_name;               /* Username */
  char  *pw_passwd;             /* Encrypted password */
  uid_t  pw_uid;                /* User ID */
  gid_t  pw_gid;                /* Group ID */
  char  *pw_gecos;              /* Real name etc */
  char  *pw_dir;                /* Home directory */
  char  *pw_shell;              /* Shell */
};

/* prototypes */
struct passwd *getpwuid(uid_t uid);
struct passwd *getpwnam(const char *name);
void setpwent(void);
struct passwd *getpwent(void);
void endpwent(void);

#endif /* _PWD_H_ */
