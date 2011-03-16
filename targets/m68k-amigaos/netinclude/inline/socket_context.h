#ifndef _INLINE_SOCKET_CONTEXT_H
#define _INLINE_SOCKET_CONTEXT_H \
       "$Id: socket_context.h,v 1.1 1996/02/16 15:42:55 too Exp $"
/*
 *	Gcc base context defines for bsdsocket.library. 
 *
 *	Copyright © 1996 AmiTCP/IP Group,
 *			 Network Solutions Development, Inc.
 *			 All rights reserved.
 */

#ifdef BASE_EXT_DECL
#error BASE_EXT_DECL etc declarations not allowed
#endif

#define BASE_EXT_DECL
#define BASE_EXT_DECL0
#define BASE_PAR_DECL struct Library * SocketBase,
#define BASE_PAR_DECL0 struct Library * SocketBase
#define BASE_NAME SocketBase

#include <inline/socket.h>	 

/* BASE_* macros are undefined at the end of <inline/socket.h> */
       
#define		     socket(r...)	socket(SocketBase, r)
#define		       bind(r...)	bind(SocketBase, r)
#define		     listen(r...)	listen(SocketBase, r)
#define		     accept(r...)	accept(SocketBase, r)
#define		    connect(r...)	connect(SocketBase, r)
#define		     sendto(r...)	sendto(SocketBase, r)
#define		       send(r...)	send(SocketBase, r)
#define		   recvfrom(r...)	recvfrom(SocketBase, r)
#define		       recv(r...)	recv(SocketBase, r)
#define		   shutdown(r...)	shutdown(SocketBase, r)
#define		 setsockopt(r...)	setsockopt(SocketBase, r)
#define		 getsockopt(r...)	getsockopt(SocketBase, r)
#define		getsockname(r...)	getsockname(SocketBase, r)
#define		getpeername(r...)	getpeername(SocketBase, r)

#define	      getdtablesize()		getdtablesize(SocketBase)

#define		  inet_addr(r...)	inet_addr(SocketBase, r)
#define	       inet_network(r...)	inet_network(SocketBase, r)

#define	      gethostbyname(r...)	gethostbyname(SocketBase, r)
#define	      gethostbyaddr(r...)	gethostbyaddr(SocketBase, r)
#define	       getnetbyname(r...)	getnetbyname(SocketBase, r)
#define	       getnetbyaddr(r...)	getnetbyaddr(SocketBase, r)
#define	      getservbyname(r...)	getservbyname(SocketBase, r)
#define	      getservbyport(r...)	getservbyport(SocketBase, r)
#define	     getprotobyname(r...)	getprotobyname(SocketBase, r)
#define	   getprotobynumber(r...)	getprotobynumber(SocketBase, r)

#define		    vsyslog(r...)	vsyslog(SocketBase, r)

#define		    sendmsg(r...)	sendmsg(SocketBase, r)
#define		    recvmsg(r...)	recvmsg(SocketBase, r)

#define		gethostname(r...)	gethostname(SocketBase, r)
#define		  gethostid()		gethostid(SocketBase)

#define		IoctlSocket(r...)	IoctlSocket(SocketBase, r)
#define		CloseSocket(r...)	CloseSocket(SocketBase, r)

#define		 WaitSelect(r...)	WaitSelect(SocketBase, r)
#define	   SetSocketSignals(r...)	SetSocketSignals(SocketBase, r)

#define	       ObtainSocket(r...)	ObtainSocket(SocketBase, r)
#define	      ReleaseSocket(r...)	ReleaseSocket(SocketBase, r)
#define ReleaseCopyOfSocket(r...)	ReleaseCopyOfSocket(SocketBase, r)
#define		      Errno()		Errno(SocketBase)
#define		SetErrnoPtr(r...)	SetErrnoPtr(SocketBase, r)

#define		  Inet_NtoA(r...)	Inet_NtoA(SocketBase, r)
#define		 Inet_LnaOf(r...)	Inet_LnaOf(SocketBase, r)
#define		 Inet_NetOf(r...)	Inet_NetOf(SocketBase, r)
#define	      Inet_MakeAddr(r...)	Inet_MakeAddr(SocketBase, r)

#define	  SocketBaseTagList(r...)	SocketBaseTagList(SocketBase, r)

#define	    GetSocketEvents(r...)	GetSocketEvents(SocketBase, r)

       
#define	      InheritSocket(r...)	InheritSocket(SocketBase, r)


#define	       select(r...)	select(SocketBase, r)
#define	    inet_ntoa(r...)	inet_ntoa(SocketBase, r)
#define inet_makeaddr(r...)	inet_makeaddr(SocketBase, r)
#define	   inet_lnaof(r...)	inet_lnaof(SocketBase, r)
#define	   inet_netof(r...)	inet_netof(SocketBase, r)


#endif /* _INLINE_SOCKET_CONTEXT_H */
