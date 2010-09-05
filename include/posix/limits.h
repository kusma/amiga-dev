/*
 * POSIX Compatibility Library for AmigaOS
 *
 * Written by Frank Wille <frank@phoenix.owl.de> in 2006
 *
 * $Id: limits.h,v 1.3 2006/08/09 11:05:34 phx Exp $
 */

#ifndef _LIMITS_H_
#define _LIMITS_H_

#if !defined(_ANSI_SOURCE)
#define _POSIX_ARG_MAX          4096
#define _POSIX_CHILD_MAX        6
#define _POSIX_LINK_MAX         8
#define _POSIX_LOGIN_NAME_MAX   9
#define _POSIX_MAX_CANON        255
#define _POSIX_MAX_INPUT        255
#define _POSIX_NAME_MAX         14
#define _POSIX_NGROUPS_MAX      0
#define _POSIX_OPEN_MAX         16
#define _POSIX_PATH_MAX         255
#define _POSIX_PIPE_BUF         512
#define _POSIX_SSIZE_MAX        32767
#define _POSIX_STREAM_MAX       8
#define _POSIX_TZNAME_MAX       3

#define _POSIX2_BC_BASE_MAX     99
#define _POSIX2_BC_DIM_MAX      2048
#define _POSIX2_BC_SCALE_MAX    99
#define _POSIX2_BC_STRING_MAX   1000
#define _POSIX2_COLL_WEIGHTS_MAX        2
#define _POSIX2_EXPR_NEST_MAX   32
#define _POSIX2_LINE_MAX        2048
#define _POSIX2_RE_DUP_MAX      255
#endif /* !_ANSI_SOURCE */

#include <sys/syslimits.h>
#include <machine/limits.h>

#endif /* _LIMITS_H_ */
