/*
 * POSIX Compatibility Library for AmigaOS
 *
 * Written by Frank Wille <frank@phoenix.owl.de> in 2003
 *
 * $Id: param.h,v 1.2 2005/02/27 13:50:46 phx Exp $
 */

#ifndef _SYS_PARAM_H_
#define _SYS_PARAM_H_

#include <sys/null.h>

#include <sys/inttypes.h>
#include <sys/types.h>

#include <sys/syslimits.h>

#define MAXCOMLEN       16              /* max command name remembered */
#define MAXINTERP       64              /* max interpreter file name length */
#define NOFILE          OPEN_MAX        /* max open files per process */
#define MAXPATHLEN      PATH_MAX 

/* Macros for min/max. */
#ifndef MIN
#define MIN(a,b)        (((a)<(b))?(a):(b))
#endif
#ifndef MAX
#define MAX(a,b)        (((a)>(b))?(a):(b))
#endif

#endif  /* _SYS_PARAM_H_ */
