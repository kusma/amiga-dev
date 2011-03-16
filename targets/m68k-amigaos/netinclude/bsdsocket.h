#ifndef BSDSOCKET_H
#define BSDSOCKET_H \
       "$Id: bsdsocket.h,v 4.3 1996/03/26 07:45:52 jraja Exp $"
/*
 *      Compiler dependent prototypes and inlines for bsdsocket.library
 *
 *      Copyright © 1996 AmiTCP/IP Group, Network Solutions Development, Inc.
 *                       All rights reserved.
 */

#ifndef UNISTD_H
#include <unistd.h>		/* These should be compiler independent */
#endif

#ifndef CLIB_NETLIB_PROTOS_H
#include <clib/netlib_protos.h>
#endif

#ifndef CLIB_SOCKET_PROTOS_H
#if defined(__SASC) || defined(__GNUC__)
#include <proto/socket.h>
#else
#include <clib/socket_protos.h>
#endif
#endif

#ifndef CLIB_USERGROUP_PROTOS_H
#if defined(__SASC) || defined(__GNUC__)
#include <proto/usergroup.h>
#else
#include <clib/usergroup_protos.h>
#endif
#endif

#endif /* !BSDSOCKET_H */
