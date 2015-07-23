/*
 * POSIX Compatibility Library for AmigaOS
 *
 * Written by Frank Wille <frank@phoenix.owl.de> in 2005
 *
 * $Id: bsdsocket.h,v 1.3 2005/10/19 21:42:00 phx Exp $
 */

#define KERNEL
#ifdef __amigaos4__
#include <proto/bsdsocket.h>
#else
#pragma amiga-align
#include <proto/socket.h>
#pragma default-align
#endif
#include <sys/types.h>
#include <sys/time.h>


/* Prototypes */

int __init_bsdsocket(int);
void __close_socket(int);
int __socket2fd(int);
int __socket_select(int,fd_set *,fd_set *,fd_set *,
                    struct timeval *,unsigned long *);
