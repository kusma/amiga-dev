#ifndef PRAGMAS_SOCKET_PRAGMAS_H
#define PRAGMAS_SOCKET_PRAGMAS_H \
       "$Id: socket_pragmas.h,v 4.3 1996/03/02 15:29:28 too Exp $"
/*
 *      Pragma file for the "bsdsocket.library" version 4
 *
 *      Copyright © 1994 AmiTCP/IP Group,
 *                       Network Solutions Development, Inc.
 *                       All rights reserved.
 */

#ifndef AMITCP_BIG_NAMES

/*------ BSD Socket System Calls ------*/
#pragma libcall SocketBase socket 1E 21003
#pragma libcall SocketBase bind 24 18003
#pragma libcall SocketBase listen 2A 1002
#pragma libcall SocketBase accept 30 98003
#pragma libcall SocketBase connect 36 18003
#pragma libcall SocketBase sendto 3C 39218006
#pragma libcall SocketBase send 42 218004
#pragma libcall SocketBase recvfrom 48 A9218006
#pragma libcall SocketBase recv 4E 218004
#pragma libcall SocketBase shutdown 54 1002
#pragma libcall SocketBase setsockopt 5A 3821005
#pragma libcall SocketBase getsockopt 60 9821005
#pragma libcall SocketBase getsockname 66 98003
#pragma libcall SocketBase getpeername 6C 98003
/*------ Generic System Calls Related to Sockets*/
#pragma libcall SocketBase IoctlSocket 72 81003
#pragma libcall SocketBase CloseSocket 78 001
/*------ AmiTCP/IP specific stuff ------*/
#pragma libcall SocketBase WaitSelect 7E 1BA98006
#pragma libcall SocketBase SetSocketSignals 84 21003
#pragma libcall SocketBase getdtablesize 8A 0
#pragma libcall SocketBase ObtainSocket 90 321004
#pragma libcall SocketBase ReleaseSocket 96 1002
#pragma libcall SocketBase ReleaseCopyOfSocket 9C 1002
#pragma libcall SocketBase Errno A2 0
#pragma libcall SocketBase SetErrnoPtr A8 0802
/*------ INet library calls related to INet address manipulation ------*/
#pragma libcall SocketBase Inet_NtoA AE 001
#pragma libcall SocketBase inet_addr B4 801
#pragma libcall SocketBase Inet_LnaOf BA 001
#pragma libcall SocketBase Inet_NetOf C0 001
#pragma libcall SocketBase Inet_MakeAddr C6 1002
#pragma libcall SocketBase inet_network CC 801
/*------ NetDB Functions ------ */
#pragma libcall SocketBase gethostbyname D2 801
#pragma libcall SocketBase gethostbyaddr D8 10803
#pragma libcall SocketBase getnetbyname DE 801
#pragma libcall SocketBase getnetbyaddr E4 1002
#pragma libcall SocketBase getservbyname EA 9802
#pragma libcall SocketBase getservbyport F0 8002
#pragma libcall SocketBase getprotobyname F6 801
#pragma libcall SocketBase getprotobynumber FC 001
/*------ Syslog function ------*/
#pragma libcall SocketBase vsyslog 102 98003
#pragma tagcall SocketBase syslog 102 98003
/*------ AmiTCP/IP 2 extensions ------*/
#pragma libcall SocketBase Dup2Socket 108 1002
/*------ AmiTCP/IP version 3 extensions below ------*/
#pragma libcall SocketBase sendmsg 10E 18003
#pragma libcall SocketBase recvmsg 114 18003
/*------ Host identification ------*/
#pragma libcall SocketBase gethostname 11A 0802
#pragma libcall SocketBase gethostid 120 0
/*------ Socket Base manipulation ------*/
#pragma libcall SocketBase SocketBaseTagList 126 801
#pragma tagcall SocketBase SocketBaseTags 126 801
/*------ AmiTCP/IP version 4 extensions below ------*/
#pragma libcall SocketBase GetSocketEvents 12C 801

#else /* AMITCP_BIG_NAMES (don't use in new projects) */

/*------ BSD Socket System Calls ------*/
#pragma libcall SocketBase Socket 1E 21003
#pragma libcall SocketBase Bind 24 18003
#pragma libcall SocketBase Listen 2A 1002
#pragma libcall SocketBase Accept 30 98003
#pragma libcall SocketBase Connect 36 18003
#pragma libcall SocketBase SendTo 3C 39218006
#pragma libcall SocketBase Send 42 218004
#pragma libcall SocketBase RecvFrom 48 A9218006
#pragma libcall SocketBase Recv 4E 218004
#pragma libcall SocketBase Shutdown 54 1002
#pragma libcall SocketBase SetSockOpt 5A 3821005
#pragma libcall SocketBase GetSockOpt 60 9821005
#pragma libcall SocketBase GetSockName 66 98003
#pragma libcall SocketBase GetPeerName 6C 98003
/*------ Generic System Calls Related to Sockets*/
#pragma libcall SocketBase IoctlSocket 72 81003
#pragma libcall SocketBase CloseSocket 78 001
/*------ AmiTCP/IP specific stuff ------*/
#pragma libcall SocketBase WaitSelect 7E 1BA98006
#pragma libcall SocketBase SetSocketSignals 84 21003
#pragma libcall SocketBase GetDTableSize 8A 0
#pragma libcall SocketBase ObtainSocket 90 321004
#pragma libcall SocketBase ReleaseSocket 96 1002
#pragma libcall SocketBase ReleaseCopyOfSocket 9C 1002
#pragma libcall SocketBase Errno A2 0
#pragma libcall SocketBase SetErrnoPtr A8 0802
/*------ INet library calls related to INet address manipulation ------*/
#pragma libcall SocketBase Inet_NtoA AE 001
#pragma libcall SocketBase Inet_Addr B4 801
#pragma libcall SocketBase Inet_LnaOf BA 001
#pragma libcall SocketBase Inet_NetOf C0 001
#pragma libcall SocketBase Inet_MakeAddr C6 1002
#pragma libcall SocketBase Inet_Network CC 801
/*------ NetDB Functions ------ */
#pragma libcall SocketBase GetHostByName D2 801
#pragma libcall SocketBase GetHostByAddr D8 10803
#pragma libcall SocketBase GetNetByName DE 801
#pragma libcall SocketBase GetNetByAddr E4 1002
#pragma libcall SocketBase GetServByName EA 9802
#pragma libcall SocketBase GetServByPort F0 8002
#pragma libcall SocketBase GetProtoByName F6 801
#pragma libcall SocketBase GetProtoByNumber FC 001
/*------ Syslog function ------*/
#pragma libcall SocketBase SyslogA 102 98003
#pragma tagcall SocketBase Syslog 102 98003
/*------ AmiTCP/IP 2 extensions ------*/
#pragma libcall SocketBase Dup2Socket 108 1002
/*------ AmiTCP/IP version 3 extensions below ------*/
#pragma libcall SocketBase SendMsg 10E 18003
#pragma libcall SocketBase RecvMsg 114 18003
/*------ Host identification ------*/
#pragma libcall SocketBase GetHostName 11A 0802
#pragma libcall SocketBase GetHostId 120 0
/*------ Socket Base manipulation ------*/
#pragma libcall SocketBase SocketBaseTagList 126 801
#pragma tagcall SocketBase SocketBaseTags 126 801
/*------ AmiTCP/IP version 4 extensions below ------*/
#pragma libcall SocketBase GetSocketEvents 12C 801

#endif /* ! AMITCP_BIG_NAMES */

#endif /* PRAGMAS_SOCKET_PRAGMAS_H */
