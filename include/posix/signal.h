/*
 * POSIX Compatibility Library for AmigaOS
 *
 * Written by Frank Wille <frank@phoenix.owl.de> in 2003-2004
 *
 * $Id: signal.h,v 1.1 2004/11/28 21:29:08 phx Exp $
 */

#ifndef _SIGNAL_H_
#define _SIGNAL_H_

#include_next <signal.h>
#include <sys/types.h>


/* Prototypes */
int kill(pid_t, int);
int killpg(pid_t, int);

#endif /* _SIGNAL_H_ */
