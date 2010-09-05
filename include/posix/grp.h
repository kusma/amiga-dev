/*
 * AmiTCP/IP compatibility headers for PosixLib
 *
 * Written by Frank Wille <frank@phoenix.owl.de> in 2005
 *
 * $Id: grp.h,v 1.2 2005/04/23 19:18:16 phx Exp $
 */

#ifndef _GRP_H_
#define _GRP_H_

#ifndef _SYS_TYPES_H_
#include <sys/types.h>
#endif

/* structure is compatible with usergroup.library,
   although not required at the moment */

struct group {
  char   *gr_name;              /* Group name.  */
  char   *gr_passwd;            /* Password.    */
  gid_t   gr_gid;               /* Group ID.    */
  char  **gr_mem;               /* Member list. */
};

/* prototypes */
struct group *getgrgid(gid_t gid);
struct group *getgrnam(const char * name);

void setgrent(void);
struct group *getgrent(void);
void endgrent(void);

#endif /* _GRP_H_ */
