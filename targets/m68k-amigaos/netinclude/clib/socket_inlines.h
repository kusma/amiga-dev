#ifndef CLIB_SOCKET_INLINES_H
#define CLIB_SOCKET_INLINES_H \
       "$Id: socket_inlines.h,v 4.1 1994/10/03 20:54:51 ppessi Exp $"
/*
 *	Some inlines to replace common stubs 
 *
 *      Copyright © 1994 AmiTCP/IP Group,
 *                       Network Solutions Development, Inc.
 *                       All rights reserved.
 */

#define select        _static_inline_select
#define inet_ntoa     _static_inline_inet_ntoa
#define inet_makeaddr _static_inline_inet_makeaddr
#define inet_lnaof    _static_inline_inet_lnaof
#define inet_netof    _static_inline_inet_netof

static int __inline 
select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exeptfds,
	 struct timeval *timeout)
{
  /* call WaitSelect with NULL signal mask pointer */
  return WaitSelect(nfds, readfds, writefds, exeptfds, timeout, NULL);
}

static char * __inline
inet_ntoa(struct in_addr addr) 
{
  return Inet_NtoA(addr.s_addr);
}
static struct in_addr __inline
inet_makeaddr(int net, int host)
{
  struct in_addr addr;
  addr.s_addr = Inet_MakeAddr(net, host);
  return addr;
}
static unsigned long __inline
inet_lnaof(struct in_addr addr) 
{
  return Inet_LnaOf(addr.s_addr);
}
static unsigned long  __inline
inet_netof(struct in_addr addr)
{
  return Inet_NetOf(addr.s_addr);
}
#endif /* CLIB_SOCKET_INLINES_H */
