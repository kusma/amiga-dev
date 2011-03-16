#ifndef _INLINE_SOCKET_H
#define _INLINE_SOCKET_H \
	"$Id: socket.h,v 4.6 1996/03/02 15:31:01 too Exp $"

/* If you have used Capitalized socket function names, please include
   <amitcp/bignames.h> as first file in your c programs. Use of
   capitalized function names is discouraged in future projects */

#ifndef _CDEFS_H_
#include <sys/cdefs.h>
#endif
#ifndef _INLINE_STUBS_H_
#include <inline/stubs.h>
#endif

__BEGIN_DECLS

#ifndef BASE_EXT_DECL
#define BASE_EXT_DECL
#define BASE_EXT_DECL0 extern struct Library * SocketBase;
#endif
#ifndef BASE_PAR_DECL
#define BASE_PAR_DECL
#define BASE_PAR_DECL0 void
#endif
#ifndef BASE_NAME
#define BASE_NAME SocketBase
#endif

BASE_EXT_DECL0

extern __inline LONG
CloseSocket (BASE_PAR_DECL LONG d)
{
	BASE_EXT_DECL
	register res __asm("d0");
	register struct Library *a6 __asm("a6") = BASE_NAME;
	register LONG d0 __asm("d0") = d;
	__asm __volatile ("jsr a6@(-0x78)"
	: "=r" (res)
	: "r" (a6), "r" (d0)
	: "d0", "d1", "a0", "a1");
	return res;
}
extern __inline LONG
Dup2Socket (BASE_PAR_DECL LONG fd1, LONG fd2)
{
	BASE_EXT_DECL
	register res __asm("d0");
	register struct Library *a6 __asm("a6") = BASE_NAME;
	register LONG d0 __asm("d0") = fd1;
	register LONG d1 __asm("d1") = fd2;
	__asm __volatile ("jsr a6@(-0x108)"
	: "=r" (res)
	: "r" (a6), "r" (d0), "r" (d1)
	: "d0", "d1", "a0", "a1");
	return res;
}
extern __inline LONG
Errno (BASE_PAR_DECL0)
{
	BASE_EXT_DECL
	register res __asm("d0");
	register struct Library *a6 __asm("a6") = BASE_NAME;
	__asm __volatile ("jsr a6@(-0xa2)"
	: "=r" (res)
	: "r" (a6)
	: "d0", "d1", "a0", "a1");
	return res;
}
extern __inline LONG
GetSocketEvents (BASE_PAR_DECL ULONG * eventmaskp)
{
	BASE_EXT_DECL
	register res __asm("d0");
	register struct Library *a6 __asm("a6") = BASE_NAME;
	register ULONG * a0 __asm("a0") = eventmaskp;
	__asm __volatile ("jsr a6@(-0x12c)"
	: "=r" (res)
	: "r" (a6), "r" (a0)
	: "d0", "d1", "a0", "a1");
	return res;
}
extern __inline ULONG
Inet_LnaOf (BASE_PAR_DECL LONG in)
{
	BASE_EXT_DECL
	register res __asm("d0");
	register struct Library *a6 __asm("a6") = BASE_NAME;
	register LONG d0 __asm("d0") = in;
	__asm __volatile ("jsr a6@(-0xba)"
	: "=r" (res)
	: "r" (a6), "r" (d0)
	: "d0", "d1", "a0", "a1");
	return res;
}
extern __inline ULONG
Inet_MakeAddr (BASE_PAR_DECL ULONG net, ULONG host)
{
	BASE_EXT_DECL
	register res __asm("d0");
	register struct Library *a6 __asm("a6") = BASE_NAME;
	register ULONG d0 __asm("d0") = net;
	register ULONG d1 __asm("d1") = host;
	__asm __volatile ("jsr a6@(-0xc6)"
	: "=r" (res)
	: "r" (a6), "r" (d0), "r" (d1)
	: "d0", "d1", "a0", "a1");
	return res;
}
extern __inline ULONG
Inet_NetOf (BASE_PAR_DECL LONG in)
{
	BASE_EXT_DECL
	register res __asm("d0");
	register struct Library *a6 __asm("a6") = BASE_NAME;
	register LONG d0 __asm("d0") = in;
	__asm __volatile ("jsr a6@(-0xc0)"
	: "=r" (res)
	: "r" (a6), "r" (d0)
	: "d0", "d1", "a0", "a1");
	return res;
}
extern __inline char *
Inet_NtoA (BASE_PAR_DECL ULONG in)
{
	BASE_EXT_DECL
	register char * res __asm("d0");
	register struct Library *a6 __asm("a6") = BASE_NAME;
	register ULONG d0 __asm("d0") = in;
	__asm __volatile ("jsr a6@(-0xae)"
	: "=r" (res)
	: "r" (a6), "r" (d0)
	: "d0", "d1", "a0", "a1");
	return res;
}
extern __inline LONG
IoctlSocket (BASE_PAR_DECL LONG d, ULONG request, char * argp)
{
	BASE_EXT_DECL
	register res __asm("d0");
	register struct Library *a6 __asm("a6") = BASE_NAME;
	register LONG d0 __asm("d0") = d;
	register ULONG d1 __asm("d1") = request;
	register char * a0 __asm("a0") = argp;
	__asm __volatile ("jsr a6@(-0x72)"
	: "=r" (res)
	: "r" (a6), "r" (d0), "r" (d1), "r" (a0)
	: "d0", "d1", "a0", "a1");
	return res;
}
extern __inline LONG
ObtainSocket (BASE_PAR_DECL LONG id, LONG domain, LONG type, LONG protocol)
{
	BASE_EXT_DECL
	register res __asm("d0");
	register struct Library *a6 __asm("a6") = BASE_NAME;
	register LONG d0 __asm("d0") = id;
	register LONG d1 __asm("d1") = domain;
	register LONG d2 __asm("d2") = type;
	register LONG d3 __asm("d3") = protocol;
	__asm __volatile ("jsr a6@(-0x90)"
	: "=r" (res)
	: "r" (a6), "r" (d0), "r" (d1), "r" (d2), "r" (d3)
	: "d0", "d1", "a0", "a1", "d2", "d3");
	return res;
}
extern __inline LONG
ReleaseCopyOfSocket (BASE_PAR_DECL LONG fd, LONG id)
{
	BASE_EXT_DECL
	register res __asm("d0");
	register struct Library *a6 __asm("a6") = BASE_NAME;
	register LONG d0 __asm("d0") = fd;
	register LONG d1 __asm("d1") = id;
	__asm __volatile ("jsr a6@(-0x9c)"
	: "=r" (res)
	: "r" (a6), "r" (d0), "r" (d1)
	: "d0", "d1", "a0", "a1");
	return res;
}
extern __inline LONG
ReleaseSocket (BASE_PAR_DECL LONG fd, LONG id)
{
	BASE_EXT_DECL
	register res __asm("d0");
	register struct Library *a6 __asm("a6") = BASE_NAME;
	register LONG d0 __asm("d0") = fd;
	register LONG d1 __asm("d1") = id;
	__asm __volatile ("jsr a6@(-0x96)"
	: "=r" (res)
	: "r" (a6), "r" (d0), "r" (d1)
	: "d0", "d1", "a0", "a1");
	return res;
}
extern __inline LONG
SetErrnoPtr (BASE_PAR_DECL void * errno_p, LONG size)
{
	BASE_EXT_DECL
	register res __asm("d0");
	register struct Library *a6 __asm("a6") = BASE_NAME;
	register void * a0 __asm("a0") = errno_p;
	register LONG d0 __asm("d0") = size;
	__asm __volatile ("jsr a6@(-0xa8)"
	: "=r" (res)
	: "r" (a6), "r" (a0), "r" (d0)
	: "d0", "d1", "a0", "a1");
	return res;
}
extern __inline void
SetSocketSignals (BASE_PAR_DECL ULONG SIGINTR, ULONG SIGIO, ULONG SIGURG)
{
	BASE_EXT_DECL
	register struct Library *a6 __asm("a6") = BASE_NAME;
	register ULONG d0 __asm("d0") = SIGINTR;
	register ULONG d1 __asm("d1") = SIGIO;
	register ULONG d2 __asm("d2") = SIGURG;
	__asm __volatile ("jsr a6@(-0x84)"
	: /* No Output */
	: "r" (a6), "r" (d0), "r" (d1), "r" (d2)
	: "d0", "d1", "a0", "a1", "d2");
}
extern __inline LONG
SocketBaseTagList (BASE_PAR_DECL struct TagItem * taglist)
{
	BASE_EXT_DECL
	register res __asm("d0");
	register struct Library *a6 __asm("a6") = BASE_NAME;
	register struct TagItem * a0 __asm("a0") = taglist;
	__asm __volatile ("jsr a6@(-0x126)"
	: "=r" (res)
	: "r" (a6), "r" (a0)
	: "d0", "d1", "a0", "a1");
	return res;
}
#ifndef NO_INLINE_STDARG
#define SocketBaseTags(tags...) \
  ({ struct TagItem _tags[] = { tags }; SocketBaseTagList (_tags); })
#endif /* not NO_INLINE_STDARG */

extern __inline LONG
WaitSelect (BASE_PAR_DECL LONG nfds, fd_set * readfds, fd_set * writefds, fd_set * execptfds, struct timeval * timeout, ULONG * maskp)
{
	BASE_EXT_DECL
	register res __asm("d0");
	register struct Library *a6 __asm("a6") = BASE_NAME;
	register LONG d0 __asm("d0") = nfds;
	register fd_set * a0 __asm("a0") = readfds;
	register fd_set * a1 __asm("a1") = writefds;
	register fd_set * a2 __asm("a2") = execptfds;
	register struct timeval * a3 __asm("a3") = timeout;
	register ULONG * d1 __asm("d1") = maskp;
	__asm __volatile ("jsr a6@(-0x7e)"
	: "=r" (res)
	: "r" (a6), "r" (d0), "r" (a0), "r" (a1), "r" (a2), "r" (a3), "r" (d1)
	: "d0", "d1", "a0", "a1", "a2", "a3");
	return res;
}
extern __inline LONG
accept (BASE_PAR_DECL LONG s, struct sockaddr * addr, LONG * addrlen)
{
	BASE_EXT_DECL
	register res __asm("d0");
	register struct Library *a6 __asm("a6") = BASE_NAME;
	register LONG d0 __asm("d0") = s;
	register struct sockaddr * a0 __asm("a0") = addr;
	register LONG * a1 __asm("a1") = addrlen;
	__asm __volatile ("jsr a6@(-0x30)"
	: "=r" (res)
	: "r" (a6), "r" (d0), "r" (a0), "r" (a1)
	: "d0", "d1", "a0", "a1");
	return res;
}
extern __inline LONG
bind (BASE_PAR_DECL LONG s, const struct sockaddr * name, LONG namelen)
{
	BASE_EXT_DECL
	register res __asm("d0");
	register struct Library *a6 __asm("a6") = BASE_NAME;
	register LONG d0 __asm("d0") = s;
	register const struct sockaddr * a0 __asm("a0") = name;
	register LONG d1 __asm("d1") = namelen;
	__asm __volatile ("jsr a6@(-0x24)"
	: "=r" (res)
	: "r" (a6), "r" (d0), "r" (a0), "r" (d1)
	: "d0", "d1", "a0", "a1");
	return res;
}
extern __inline LONG
connect (BASE_PAR_DECL LONG s, const struct sockaddr * name, LONG namelen)
{
	BASE_EXT_DECL
	register res __asm("d0");
	register struct Library *a6 __asm("a6") = BASE_NAME;
	register LONG d0 __asm("d0") = s;
	register const struct sockaddr * a0 __asm("a0") = name;
	register LONG d1 __asm("d1") = namelen;
	__asm __volatile ("jsr a6@(-0x36)"
	: "=r" (res)
	: "r" (a6), "r" (d0), "r" (a0), "r" (d1)
	: "d0", "d1", "a0", "a1");
	return res;
}
extern __inline LONG
getdtablesize (BASE_PAR_DECL0)
{
	BASE_EXT_DECL
	register res __asm("d0");
	register struct Library *a6 __asm("a6") = BASE_NAME;
	__asm __volatile ("jsr a6@(-0x8a)"
	: "=r" (res)
	: "r" (a6)
	: "d0", "d1", "a0", "a1");
	return res;
}
extern __inline struct hostent  *
gethostbyaddr (BASE_PAR_DECL const UBYTE * addr, LONG len, LONG type)
{
	BASE_EXT_DECL
	register struct hostent * res __asm("d0");
	register struct Library *a6 __asm("a6") = BASE_NAME;
	register const UBYTE * a0 __asm("a0") = addr;
	register LONG d0 __asm("d0") = len;
	register LONG d1 __asm("d1") = type;
	__asm __volatile ("jsr a6@(-0xd8)"
	: "=r" (res)
	: "r" (a6), "r" (a0), "r" (d0), "r" (d1)
	: "d0", "d1", "a0", "a1");
	return res;
}
extern __inline struct hostent  *
gethostbyname (BASE_PAR_DECL const UBYTE * name)
{
	BASE_EXT_DECL
	register struct hostent * res __asm("d0");
	register struct Library *a6 __asm("a6") = BASE_NAME;
	register const UBYTE * a0 __asm("a0") = name;
	__asm __volatile ("jsr a6@(-0xd2)"
	: "=r" (res)
	: "r" (a6), "r" (a0)
	: "d0", "d1", "a0", "a1");
	return res;
}
extern __inline ULONG
gethostid (BASE_PAR_DECL0)
{
	BASE_EXT_DECL
	register res __asm("d0");
	register struct Library *a6 __asm("a6") = BASE_NAME;
	__asm __volatile ("jsr a6@(-0x120)"
	: "=r" (res)
	: "r" (a6)
	: "d0", "d1", "a0", "a1");
	return res;
}
extern __inline LONG
gethostname (BASE_PAR_DECL STRPTR hostname, LONG size)
{
	BASE_EXT_DECL
	register res __asm("d0");
	register struct Library *a6 __asm("a6") = BASE_NAME;
	register STRPTR a0 __asm("a0") = hostname;
	register LONG d0 __asm("d0") = size;
	__asm __volatile ("jsr a6@(-0x11a)"
	: "=r" (res)
	: "r" (a6), "r" (a0), "r" (d0)
	: "d0", "d1", "a0", "a1");
	return res;
}
extern __inline struct netent   *
getnetbyaddr (BASE_PAR_DECL LONG net, LONG type)
{
	BASE_EXT_DECL
	register struct netent * res __asm("d0");
	register struct Library *a6 __asm("a6") = BASE_NAME;
	register LONG d0 __asm("d0") = net;
	register LONG d1 __asm("d1") = type;
	__asm __volatile ("jsr a6@(-0xe4)"
	: "=r" (res)
	: "r" (a6), "r" (d0), "r" (d1)
	: "d0", "d1", "a0", "a1");
	return res;
}
extern __inline struct netent   *
getnetbyname (BASE_PAR_DECL const UBYTE * name)
{
	BASE_EXT_DECL
	register struct netent * res __asm("d0");
	register struct Library *a6 __asm("a6") = BASE_NAME;
	register const UBYTE * a0 __asm("a0") = name;
	__asm __volatile ("jsr a6@(-0xde)"
	: "=r" (res)
	: "r" (a6), "r" (a0)
	: "d0", "d1", "a0", "a1");
	return res;
}
extern __inline LONG
getpeername (BASE_PAR_DECL LONG s, struct sockaddr * hostname, LONG * namelen)
{
	BASE_EXT_DECL
	register res __asm("d0");
	register struct Library *a6 __asm("a6") = BASE_NAME;
	register LONG d0 __asm("d0") = s;
	register struct sockaddr * a0 __asm("a0") = hostname;
	register LONG * a1 __asm("a1") = namelen;
	__asm __volatile ("jsr a6@(-0x6c)"
	: "=r" (res)
	: "r" (a6), "r" (d0), "r" (a0), "r" (a1)
	: "d0", "d1", "a0", "a1");
	return res;
}
extern __inline struct protoent *
getprotobyname (BASE_PAR_DECL const UBYTE * name)
{
	BASE_EXT_DECL
	register struct protoent * res __asm("d0");
	register struct Library *a6 __asm("a6") = BASE_NAME;
	register const UBYTE * a0 __asm("a0") = name;
	__asm __volatile ("jsr a6@(-0xf6)"
	: "=r" (res)
	: "r" (a6), "r" (a0)
	: "d0", "d1", "a0", "a1");
	return res;
}
extern __inline struct protoent *
getprotobynumber (BASE_PAR_DECL LONG proto)
{
	BASE_EXT_DECL
	register struct protoent * res __asm("d0");
	register struct Library *a6 __asm("a6") = BASE_NAME;
	register LONG d0 __asm("d0") = proto;
	__asm __volatile ("jsr a6@(-0xfc)"
	: "=r" (res)
	: "r" (a6), "r" (d0)
	: "d0", "d1", "a0", "a1");
	return res;
}
extern __inline struct servent  *
getservbyname (BASE_PAR_DECL const UBYTE * name, const UBYTE * proto)
{
	BASE_EXT_DECL
	register struct servent * res __asm("d0");
	register struct Library *a6 __asm("a6") = BASE_NAME;
	register const UBYTE * a0 __asm("a0") = name;
	register const UBYTE * a1 __asm("a1") = proto;
	__asm __volatile ("jsr a6@(-0xea)"
	: "=r" (res)
	: "r" (a6), "r" (a0), "r" (a1)
	: "d0", "d1", "a0", "a1");
	return res;
}
extern __inline struct servent  *
getservbyport (BASE_PAR_DECL LONG port, const UBYTE * proto)
{
	BASE_EXT_DECL
	register struct servent * res __asm("d0");
	register struct Library *a6 __asm("a6") = BASE_NAME;
	register LONG d0 __asm("d0") = port;
	register const UBYTE * a0 __asm("a0") = proto;
	__asm __volatile ("jsr a6@(-0xf0)"
	: "=r" (res)
	: "r" (a6), "r" (d0), "r" (a0)
	: "d0", "d1", "a0", "a1");
	return res;
}
extern __inline LONG
getsockname (BASE_PAR_DECL LONG s, struct sockaddr * hostname, LONG * namelen)
{
	BASE_EXT_DECL
	register res __asm("d0");
	register struct Library *a6 __asm("a6") = BASE_NAME;
	register LONG d0 __asm("d0") = s;
	register struct sockaddr * a0 __asm("a0") = hostname;
	register LONG * a1 __asm("a1") = namelen;
	__asm __volatile ("jsr a6@(-0x66)"
	: "=r" (res)
	: "r" (a6), "r" (d0), "r" (a0), "r" (a1)
	: "d0", "d1", "a0", "a1");
	return res;
}
extern __inline LONG
getsockopt (BASE_PAR_DECL LONG s, LONG level, LONG optname, void * optval, LONG * optlen)
{
	BASE_EXT_DECL
	register res __asm("d0");
	register struct Library *a6 __asm("a6") = BASE_NAME;
	register LONG d0 __asm("d0") = s;
	register LONG d1 __asm("d1") = level;
	register LONG d2 __asm("d2") = optname;
	register void * a0 __asm("a0") = optval;
	register LONG * a1 __asm("a1") = optlen;
	__asm __volatile ("jsr a6@(-0x60)"
	: "=r" (res)
	: "r" (a6), "r" (d0), "r" (d1), "r" (d2), "r" (a0), "r" (a1)
	: "d0", "d1", "a0", "a1", "d2");
	return res;
}
extern __inline ULONG
inet_addr (BASE_PAR_DECL const UBYTE * cp)
{
	BASE_EXT_DECL
	register res __asm("d0");
	register struct Library *a6 __asm("a6") = BASE_NAME;
	register const UBYTE * a0 __asm("a0") = cp;
	__asm __volatile ("jsr a6@(-0xb4)"
	: "=r" (res)
	: "r" (a6), "r" (a0)
	: "d0", "d1", "a0", "a1");
	return res;
}
extern __inline ULONG
inet_network (BASE_PAR_DECL const UBYTE * cp)
{
	BASE_EXT_DECL
	register res __asm("d0");
	register struct Library *a6 __asm("a6") = BASE_NAME;
	register const UBYTE * a0 __asm("a0") = cp;
	__asm __volatile ("jsr a6@(-0xcc)"
	: "=r" (res)
	: "r" (a6), "r" (a0)
	: "d0", "d1", "a0", "a1");
	return res;
}
extern __inline LONG
listen (BASE_PAR_DECL LONG s, LONG backlog)
{
	BASE_EXT_DECL
	register res __asm("d0");
	register struct Library *a6 __asm("a6") = BASE_NAME;
	register LONG d0 __asm("d0") = s;
	register LONG d1 __asm("d1") = backlog;
	__asm __volatile ("jsr a6@(-0x2a)"
	: "=r" (res)
	: "r" (a6), "r" (d0), "r" (d1)
	: "d0", "d1", "a0", "a1");
	return res;
}
extern __inline LONG
recv (BASE_PAR_DECL LONG s, UBYTE * buf, LONG len, LONG flags)
{
	BASE_EXT_DECL
	register res __asm("d0");
	register struct Library *a6 __asm("a6") = BASE_NAME;
	register LONG d0 __asm("d0") = s;
	register UBYTE * a0 __asm("a0") = buf;
	register LONG d1 __asm("d1") = len;
	register LONG d2 __asm("d2") = flags;
	__asm __volatile ("jsr a6@(-0x4e)"
	: "=r" (res)
	: "r" (a6), "r" (d0), "r" (a0), "r" (d1), "r" (d2)
	: "d0", "d1", "a0", "a1", "d2");
	return res;
}
extern __inline LONG
recvfrom (BASE_PAR_DECL LONG s, UBYTE * buf, LONG len, LONG flags, struct sockaddr * from, LONG * fromlen)
{
	BASE_EXT_DECL
	register res __asm("d0");
	register struct Library *a6 __asm("a6") = BASE_NAME;
	register LONG d0 __asm("d0") = s;
	register UBYTE * a0 __asm("a0") = buf;
	register LONG d1 __asm("d1") = len;
	register LONG d2 __asm("d2") = flags;
	register struct sockaddr * a1 __asm("a1") = from;
	register LONG * a2 __asm("a2") = fromlen;
	__asm __volatile ("jsr a6@(-0x48)"
	: "=r" (res)
	: "r" (a6), "r" (d0), "r" (a0), "r" (d1), "r" (d2), "r" (a1), "r" (a2)
	: "d0", "d1", "a0", "a1", "d2", "a2");
	return res;
}
extern __inline LONG
recvmsg (BASE_PAR_DECL LONG s, struct msghdr * msg, LONG flags)
{
	BASE_EXT_DECL
	register res __asm("d0");
	register struct Library *a6 __asm("a6") = BASE_NAME;
	register LONG d0 __asm("d0") = s;
	register struct msghdr * a0 __asm("a0") = msg;
	register LONG d1 __asm("d1") = flags;
	__asm __volatile ("jsr a6@(-0x114)"
	: "=r" (res)
	: "r" (a6), "r" (d0), "r" (a0), "r" (d1)
	: "d0", "d1", "a0", "a1");
	return res;
}
extern __inline LONG
send (BASE_PAR_DECL LONG s, const UBYTE * msg, LONG len, LONG flags)
{
	BASE_EXT_DECL
	register res __asm("d0");
	register struct Library *a6 __asm("a6") = BASE_NAME;
	register LONG d0 __asm("d0") = s;
	register const UBYTE * a0 __asm("a0") = msg;
	register LONG d1 __asm("d1") = len;
	register LONG d2 __asm("d2") = flags;
	__asm __volatile ("jsr a6@(-0x42)"
	: "=r" (res)
	: "r" (a6), "r" (d0), "r" (a0), "r" (d1), "r" (d2)
	: "d0", "d1", "a0", "a1", "d2");
	return res;
}
extern __inline LONG
sendmsg (BASE_PAR_DECL LONG s, struct msghdr * msg, LONG flags)
{
	BASE_EXT_DECL
	register res __asm("d0");
	register struct Library *a6 __asm("a6") = BASE_NAME;
	register LONG d0 __asm("d0") = s;
	register struct msghdr * a0 __asm("a0") = msg;
	register LONG d1 __asm("d1") = flags;
	__asm __volatile ("jsr a6@(-0x10e)"
	: "=r" (res)
	: "r" (a6), "r" (d0), "r" (a0), "r" (d1)
	: "d0", "d1", "a0", "a1");
	return res;
}
extern __inline LONG
sendto (BASE_PAR_DECL LONG s, const UBYTE * msg, LONG len, LONG flags, const struct sockaddr * to, LONG tolen)
{
	BASE_EXT_DECL
	register res __asm("d0");
	register struct Library *a6 __asm("a6") = BASE_NAME;
	register LONG d0 __asm("d0") = s;
	register const UBYTE * a0 __asm("a0") = msg;
	register LONG d1 __asm("d1") = len;
	register LONG d2 __asm("d2") = flags;
	register const struct sockaddr * a1 __asm("a1") = to;
	register LONG d3 __asm("d3") = tolen;
	__asm __volatile ("jsr a6@(-0x3c)"
	: "=r" (res)
	: "r" (a6), "r" (d0), "r" (a0), "r" (d1), "r" (d2), "r" (a1), "r" (d3)
	: "d0", "d1", "a0", "a1", "d2", "d3");
	return res;
}
extern __inline LONG
setsockopt (BASE_PAR_DECL LONG s, LONG level, LONG optname, const void * optval, LONG optlen)
{
	BASE_EXT_DECL
	register res __asm("d0");
	register struct Library *a6 __asm("a6") = BASE_NAME;
	register LONG d0 __asm("d0") = s;
	register LONG d1 __asm("d1") = level;
	register LONG d2 __asm("d2") = optname;
	register const void * a0 __asm("a0") = optval;
	register LONG d3 __asm("d3") = optlen;
	__asm __volatile ("jsr a6@(-0x5a)"
	: "=r" (res)
	: "r" (a6), "r" (d0), "r" (d1), "r" (d2), "r" (a0), "r" (d3)
	: "d0", "d1", "a0", "a1", "d2", "d3");
	return res;
}
extern __inline LONG
shutdown (BASE_PAR_DECL LONG s, LONG how)
{
	BASE_EXT_DECL
	register res __asm("d0");
	register struct Library *a6 __asm("a6") = BASE_NAME;
	register LONG d0 __asm("d0") = s;
	register LONG d1 __asm("d1") = how;
	__asm __volatile ("jsr a6@(-0x54)"
	: "=r" (res)
	: "r" (a6), "r" (d0), "r" (d1)
	: "d0", "d1", "a0", "a1");
	return res;
}
extern __inline LONG
socket (BASE_PAR_DECL LONG domain, LONG type, LONG protocol)
{
	BASE_EXT_DECL
	register res __asm("d0");
	register struct Library *a6 __asm("a6") = BASE_NAME;
	register LONG d0 __asm("d0") = domain;
	register LONG d1 __asm("d1") = type;
	register LONG d2 __asm("d2") = protocol;
	__asm __volatile ("jsr a6@(-0x1e)"
	: "=r" (res)
	: "r" (a6), "r" (d0), "r" (d1), "r" (d2)
	: "d0", "d1", "a0", "a1", "d2");
	return res;
}
extern __inline void
vsyslog (BASE_PAR_DECL ULONG level, const char * format, LONG * ap)
{
	BASE_EXT_DECL
	register struct Library *a6 __asm("a6") = BASE_NAME;
	register ULONG d0 __asm("d0") = level;
	register const char * a0 __asm("a0") = format;
	register LONG * a1 __asm("a1") = ap;
	__asm __volatile ("jsr a6@(-0x102)"
	: /* No Output */
	: "r" (a6), "r" (d0), "r" (a0), "r" (a1)
	: "d0", "d1", "a0", "a1");
}


extern __inline LONG 
InheritSocket (BASE_PAR_DECL LONG id)
{
  BASE_EXT_DECL
  register LONG  _res  __asm("d0");
  register struct Library * a6 __asm("a6") = BASE_NAME;
  register LONG d0 __asm("d0") = id;
  register LONG d1 __asm("d1") =  0;
  __asm __volatile ("jsr a6@(-0x90)"
  : "=r" (_res)
  : "r" (a6), "r" (d0), "r" (d1)
  : "a0", "a1", "d0", "d1");
  return _res;
}

#ifndef NETINET_IN_H
#include <netinet/in.h>
#endif

extern __inline char * 
  inet_ntoa(BASE_PAR_DECL struct in_addr addr) 
{
  return Inet_NtoA(addr.s_addr);
}

extern __inline struct in_addr
  inet_makeaddr(BASE_PAR_DECL int net, int host)
{
  struct in_addr addr;

  addr.s_addr = Inet_MakeAddr(net, host);
  return addr;
}

extern __inline unsigned long
  inet_lnaof(BASE_PAR_DECL struct in_addr addr) 
{
  return Inet_LnaOf(addr.s_addr);
}

extern __inline unsigned long
  inet_netof(BASE_PAR_DECL struct in_addr addr)
{
  return Inet_NetOf(addr.s_addr);
}

#undef BASE_EXT_DECL
#undef BASE_EXT_DECL0
#undef BASE_PAR_DECL
#undef BASE_PAR_DECL0
#undef BASE_NAME

__END_DECLS

#endif /* _INLINE_SOCKET_H */
