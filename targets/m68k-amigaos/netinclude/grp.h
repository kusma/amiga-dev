#ifndef GRP_H
#define GRP_H \
       "$Id: grp.h,v 4.1 1994/10/03 20:52:20 ppessi Exp $"
/*
 *	Definitions of gid_t and group structure for 32 bit C compilers
 *
 *      Copyright © 1994 AmiTCP/IP Group,
 *                       Network Solutions Development, Inc.
 *                       All rights reserved.
 */

#ifndef SYS_TYPES_H
#include <sys/types.h>
#endif

/* The group structure */
struct group {
  char   *gr_name;              /* Group name.  */
  char   *gr_passwd;            /* Password.    */
  gid_t   gr_gid;               /* Group ID.    */
  char  **gr_mem;               /* Member list. */
};

struct group *getgrgid(gid_t gid);
struct group *getgrnam(const char * name);

void setgrent(void);
struct group *getgrent(void);
void endgrent(void);

#endif /* GRP_H */
