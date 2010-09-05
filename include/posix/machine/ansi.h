/*
 * POSIX Compatibility Library for AmigaOS
 *
 * Written by Frank Wille <frank@phoenix.owl.de> in 2005
 *
 * $Id: ansi.h,v 1.1 2005/10/16 18:57:08 phx Exp $
 */

#ifndef _MACHINE_ANSI_H_
#define _MACHINE_ANSI_H_

#include <machine/int_types.h>

#define _BSD_CLOCK_T_           unsigned long   /* clock() */
#define _BSD_PTRDIFF_T_         long            /* ptr1 - ptr2 */
#define _BSD_SIZE_T_            unsigned long   /* sizeof() */
#define _BSD_SSIZE_T_           long            /* byte count or error */
#define _BSD_TIME_T_            long            /* time() */
#define _BSD_VA_LIST_           char *          /* va_list */
#define _BSD_CLOCKID_T_         int             /* clockid_t */
#define _BSD_TIMER_T_           int             /* timer_t */
#define _BSD_SUSECONDS_T_       int             /* suseconds_t */
#define _BSD_USECONDS_T_        unsigned int    /* useconds_t */

#endif  /* _MACHINE_ANSI_H_ */
