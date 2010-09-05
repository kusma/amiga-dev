/*
 * POSIX Compatibility Library for AmigaOS
 *
 * Written by Frank Wille <frank@phoenix.owl.de> in 2003
 *
 * $Id: ansi.h,v 1.2 2005/02/27 13:50:45 phx Exp $
 */

#ifndef _SYS_ANSI_H_
#define _SYS_ANSI_H_

#include <machine/types.h>


typedef char *          __caddr_t;      /* core address */
typedef __uint32_t      __gid_t;        /* group id */
typedef __uint32_t      __in_addr_t;    /* IP(v4) address */
typedef __uint16_t      __in_port_t;    /* "Internet" port number */
typedef __uint32_t      __mode_t;       /* file permissions */
typedef __int32_t       __off_t;        /* file offset */
typedef __int32_t       __pid_t;        /* process id */
typedef __uint8_t       __sa_family_t;  /* socket address family */
typedef unsigned int    __socklen_t;    /* socket-related datum length */
typedef __uint32_t      __uid_t;        /* user id */

#endif  /* _SYS_ANSI_H_ */
