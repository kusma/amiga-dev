/*
 * POSIX Compatibility Library for AmigaOS
 *
 * Written by Frank Wille <frank@phoenix.owl.de> in 2003
 *
 * $Id: syslimits.h,v 1.2 2005/02/27 13:50:46 phx Exp $
 */

#ifndef _SYS_SYSLIMITS_H_
#define _SYS_SYSLIMITS_H_

#define GID_MAX            2147483647U  /* max value for a gid_t (2^31-2) */
#define MAX_INPUT                 511   /* max bytes in terminal input */
#define NAME_MAX                   30   /* max bytes in a file name */
#define UID_MAX            2147483647U  /* max value for a uid_t (2^31-2) */
#define OPEN_MAX                   64   /* max open files per process */
#define PATH_MAX                 1024   /* max bytes in pathname */

#endif  /* _SYS_SYSLIMITS_H_ */
