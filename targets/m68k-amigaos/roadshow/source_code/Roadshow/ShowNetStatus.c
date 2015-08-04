/*
 *	$Id: ShowNetStatus.c,v 1.13 2007-08-26 12:30:26 obarthel Exp $
 *
 *	:ts=4
 *
 *	Copyright © 2001-2007 by Olaf Barthel. All Rights Reserved.
 */

#include <libraries/bsdsocket.h>

#include <exec/execbase.h>
#include <exec/memory.h>

#include <dos/dosextens.h>
#include <dos/dostags.h>
#include <dos/dosasl.h>
#include <dos/rdargs.h>
#include <dos/stdio.h>

#include <devices/sana2.h>

/****************************************************************************/

#if !defined(__amigaos4__)
#define NO_INLINE_STDARG
#endif /* __amigaos4__ */

#define __NOLIBBASE__
#define __NOGLOBALIFACE__
#define __USE_INLINE__

/****************************************************************************/

#include <proto/bsdsocket.h>
#include <proto/utility.h>
#include <proto/locale.h>
#include <proto/exec.h>
#include <proto/dos.h>

/****************************************************************************/

#include <string.h>
#include <stdarg.h>

/****************************************************************************/

#include <netdb.h>
#include <net/route.h>
#include <net/if_dl.h>
#include <netinet/icmp_var.h>
#include <netinet/igmp_var.h>
#include <netinet/ip_mroute.h>
#include <netinet/if_ether.h>

#include <netinet/ip_var.h>
#include <netinet/tcp_var.h>
#include <netinet/udp_var.h>
#include <netinet/tcp_fsm.h>
#include <sys/mbuf.h>

/****************************************************************************/

#include "macros.h"

/****************************************************************************/

#include "ShowNetStatus_rev.h"

/****************************************************************************/

#ifndef AbsExecBase
#define AbsExecBase (*(struct ExecBase **)4)
#endif /* AbsExecBase */

/****************************************************************************/

#ifndef FORMAT_DEF
#define FORMAT_DEF 4
#endif /* FORMAT_DEF */

/****************************************************************************/

typedef LONG *	NUMBER;
typedef LONG	SWITCH;
typedef STRPTR	KEY;

/****************************************************************************/

struct CommandContext
{
	struct Library *		cc_SysBase;
	struct Library *		cc_DOSBase;
	struct Library *		cc_UtilityBase;
	struct Library *		cc_SocketBase;
	struct Library *		cc_LocaleBase;

	#if defined(__amigaos4__)

	/************************************************************************/

	struct ExecIFace *		cc_IExec;
	struct DOSIFace *		cc_IDOS;
	struct UtilityIFace *	cc_IUtility;
	struct SocketIFace *	cc_ISocket;
	struct LocaleIFace *	cc_ILocale;

	/************************************************************************/

	#endif /* __amigaos4__ */

	struct Catalog *		cc_Catalog;
	struct Locale *			cc_Locale;
	UBYTE					cc_ProgramName[256];
	UBYTE *					cc_QueryBuffer;
	LONG					cc_QueryBufferSize;
	BOOL					cc_Stopped;
	BOOL					cc_HaveDomain;
	STRPTR					cc_Domain;
	STRPTR					cc_Line1;
	STRPTR					cc_Line2;
	BOOL					cc_NoNames;
	BOOL					cc_Something;
	BOOL					cc_Quiet;
};

/****************************************************************************/

#if defined(__amigaos4__)

/****************************************************************************/

#define DECLARE_SYSBASE(cc) \
	struct ExecIFace * IExec = cc->cc_IExec; \
	struct Library * SysBase = cc->cc_SysBase

#define DECLARE_DOSBASE(cc) \
	struct DOSIFace * IDOS = cc->cc_IDOS; \
	struct Library * DOSBase = cc->cc_DOSBase

#define DECLARE_UTILITYBASE(cc) \
	struct UtilityIFace * IUtility = cc->cc_IUtility; \
	struct Library * UtilityBase = cc->cc_UtilityBase

#define DECLARE_LOCALEBASE(cc) \
	struct LocaleIFace * ILocale = cc->cc_ILocale; \
	struct Library * LocaleBase = cc->cc_LocaleBase

#define DECLARE_SOCKETBASE(cc) \
	struct SocketIFace * ISocket = cc->cc_ISocket; \
	struct Library * SocketBase = cc->cc_SocketBase

/****************************************************************************/

#else

/****************************************************************************/

#define DECLARE_SYSBASE(cc) \
	struct Library * SysBase = cc->cc_SysBase

#define DECLARE_DOSBASE(cc) \
	struct Library * DOSBase = cc->cc_DOSBase

#define DECLARE_UTILITYBASE(cc) \
	struct Library * UtilityBase = cc->cc_UtilityBase

#define DECLARE_LOCALEBASE(cc) \
	struct Library * LocaleBase = cc->cc_LocaleBase

#define DECLARE_SOCKETBASE(cc) \
	struct Library * SocketBase = cc->cc_SocketBase

/****************************************************************************/

#endif /* __amigaos4__ */

/****************************************************************************/

LONG _start(VOID);

/****************************************************************************/

STATIC VOID close_libs(struct CommandContext *cc);
STATIC LONG open_libs(struct CommandContext *cc, struct Library *SysBase);
LONG cmd(struct CommandContext *cc);
STATIC STRPTR begin_underline(struct CommandContext *cc);
STATIC STRPTR end_underline(struct CommandContext *cc);
STATIC ULONG divide_64_by_32(SBQUAD_T *dividend, ULONG divisor, SBQUAD_T *quotient);
STATIC VOID insert_grouping_characters(struct Locale *loc, STRPTR buffer);
STATIC STRPTR convert_quad_to_string(struct Locale *loc, const SBQUAD_T *const number, STRPTR string, LONG max_string_len);
STATIC LONG fill_query_buffer(struct CommandContext *cc, LONG type);
STATIC VARARGS68K VOID lprintf(struct CommandContext *cc, STRPTR format, ...);
STATIC VARARGS68K VOID lsprintf(struct CommandContext *cc, STRPTR buffer, STRPTR formatString, ...);
STATIC BOOL is_blank_space(UBYTE c);
STATIC VOID strip_extra_blank_spaces(STRPTR s);
STATIC STRPTR routename(struct CommandContext *cc, ULONG in, STRPTR line, LONG line_size);
STATIC ULONG forgemask(ULONG a);
STATIC STRPTR netname(struct CommandContext *cc, ULONG in, STRPTR line, LONG line_size);
STATIC STRPTR get_sockaddr(struct CommandContext *cc, struct sockaddr *sa, int flags, STRPTR line, LONG line_size);
STATIC VOID print_arp_table(struct CommandContext *cc, struct rt_msghdr *rtm);
STATIC VOID print_route_table(struct CommandContext *cc, struct rt_msghdr *rtm);
STATIC VOID print_dns_list(struct CommandContext *cc, struct List *dns_list);
STATIC VOID print_interface_list(struct CommandContext *cc, struct List *interface_list);
STATIC BOOL print_interface_info(struct CommandContext *cc, STRPTR name);
STATIC VOID print_icmp_status(struct CommandContext *cc, struct icmpstat *icps);
STATIC VOID print_igmp_status(struct CommandContext *cc, struct igmpstat *igps);
STATIC VOID print_ip_status(struct CommandContext *cc, struct ipstat *ips);
STATIC VOID print_mb_status(struct CommandContext *cc, struct mbstat *m);
STATIC VOID print_mrt_status(struct CommandContext *cc, struct mrtstat *mrts);
STATIC VOID print_rt_status(struct CommandContext *cc, struct rtstat *rts);
STATIC VOID print_tcp_status(struct CommandContext *cc, struct tcpstat *tcps);
STATIC VOID print_udp_status(struct CommandContext *cc, struct udpstat *udps);
STATIC VOID print_tcp_socket_status(struct CommandContext *cc, BOOL all_sockets, struct protocol_connection_data *pcd, LONG size);
STATIC VOID print_udp_socket_status(struct CommandContext *cc, BOOL all_sockets, struct protocol_connection_data *pcd, LONG size);
STATIC VOID print_net_status_summary(struct CommandContext *cc);
STATIC LONG VARARGS68K Local_ErrorPrintf(struct CommandContext *cc, STRPTR format, ...);
STATIC STRPTR get_builtin_str(LONG id);
STRPTR get_str(struct CommandContext *cc, LONG id);
STATIC VARARGS68K LONG Local_QueryInterfaceTags(struct CommandContext *cc, STRPTR interface_name, ...);
STATIC VARARGS68K LONG Local_SocketBaseTags(struct CommandContext *cc, ...);

/****************************************************************************/

/****** ROADSHOW/SHOWNETSTATUS **************************************************
*
*   NAME
*	ShowNetStatus - Display various information about the status of the
*	                network configuration.
*
*   FORMAT
*	ShowNetStatus [INTERFACE=<itf>[,<itf>...]] [INTERFACES] [ARPCACHE=ARP]
*	[ROUTES] [DNS=DOMAINNAMESERVERS] [ICMP] [IGMP] [IP] [MB=MEMORY]
*	[MR=MULTICASTROUTING] [RT=ROUTING] [TCP] [UDP] [TCPSOCKETS]
*	[UDPSOCKETS] [NAMES] [ALL] [REPEAT] [QUIET]
*
*   TEMPLATE
*	INTERFACE/M,INTERFACES/S,ARPCACHE=ARP/S,ROUTES/S,
*	DNS=DOMAINNAMESERVERS/S,ICMP/S,IGMP/S,IP/S,MB=MEMORY/S,
*	MR=MULTICASTROUTING/S,RT=ROUTING/S,TCP/S,UDP/S,TCPSOCKETS/S,
*	UDPSOCKETS/S,NAMES/S,ALL/S,REPEAT/S,QUIET/S
*
*   PATH
*	C:SHOWNETSTATUS
*
*   FUNCTION
*	This command can display a lot of informations about the active
*	interfaces, either for one interface or for all them. It can be used
*	to display the current settings or to give details on the quality of
*	the network: transfer speed, number of errors, etc.
*
*	You can request which details should be displayed. If you provide
*	no options to this command, it will print a general summary of the
*	current network status.
*
*   OPTIONS
*	INTERFACE/M
*	    For each interface provided in this list, show detailed
*	    configuration information and statistics.
*
*	INTERFACES/S
*	    Show the list of all interfaces known to the TCP/IP stack.
*
*	ARP/S
*	    Show the contents of the address resolution protocol (ARP) cache.
*
*	ROUTES/S
*	    Show information on the routes that are configured. The
*	    default route is indicated by having a gateway address
*	    of 'default'.
*
*	DNS=DOMAINNAMESERVERS/S
*	    Show a list of domain name servers known to the TCP/IP
*	    stack. Servers are either dynamically configured or
*	    statically (through the 'DEVS:Interfaces/name_resolution' file).
*
*	ICMP/S
*	    Display Internet Control Message Protocol statistics.
*
*	IGMP/S
*	    Display Internet Group Management Protocol statistics.
*
*	IP/S
*	    Display Internet Protocol statistics.
*
*	MB=MEMORY/S
*	    Display memory buffer statistics.
*
*	MR=MULTICASTROUTING/S
*	    Display multicast routing statistics.
*
*	RT=ROUTING/S
*	    Display routing statistics.
*
*	TCP/S
*	    Display Transmission Control Protocol statistics.
*
*	UDP/S
*	    Display User Datagram Protocol statistics.
*
*	TCPSOCKETS/S
*	    Display information about the TCP sockets currently
*	    in use. Note that unless you use the 'ALL' option,
*	    sockets bound to local addresses will not be shown.
*
*	UDPSOCKETS/S
*	    Display information about the UDP sockets currently
*	    in use. Note that unless you use the 'ALL' option,
*	    sockets bound to local addresses will not be shown.
*
*	NAMES/S
*	    Attempt to figure out which symbolic names are associated
*	    with the IP addresses to be printed, and also attempt to
*	    associate port numbers with service names.
*
*	ALL/S
*	    This option works in conjunction with the TCPSOCKETS and
*	    UDPSOCKETS parameters. It tells the respective display
*	    routines to show all currently active sockets, including
*	    those bound to local addresses.
*
*	REPEAT/S
*	    Repeat the query and display process each second; the
*	    screen will be cleared before the new information is
*	    printed. To stop printing the data, press [Ctrl]+C.
*
*	QUIET/S
*	    Do not show any error messages.
*
*   EXAMPLES
*	Show details on interface "itf".
*
*	    1> SHOWNETSTATUS INTERFACE itf
*
*	Show details on the Internet Group Management Protocol and routing
*	statistics
*	
*	    1> SHOWNETSTATUS ROUTING IGMP
*
*   NOTES
*	This command works like a combination of the Unix "route", "ifconfig"
*	and "netstat" commands.
*
*	Most of the information printed is rather cryptic and not of much
*	use to the casual user. It can help in debugging, though.
*
******************************************************************************
*/
LONG
_start(VOID)
{
	struct Library *SysBase = (struct Library *)AbsExecBase;
	#if defined(__amigaos4__)
	struct ExecIFace * IExec = (struct ExecIFace *)((struct ExecBase *)SysBase)->MainInterface;
	#endif /* __amigaos4__ */

	struct CommandContext _cc,*cc = &_cc;
	LONG result = RETURN_FAIL;
	struct Process * pr;
	LONG error;

	memset(cc,0,sizeof(*cc));

	pr = (struct Process *)FindTask(NULL);
	if(pr->pr_CLI == ZERO)
	{
		struct MsgPort * mp = &pr->pr_MsgPort;
		struct Message * mn;

		WaitPort(mp);
		mn = GetMsg(mp);

		Forbid();
		ReplyMsg(mn);
		goto out;
	}

	error = open_libs(cc,SysBase);
	if(error == OK)
	{
		DECLARE_LOCALEBASE(cc);

		if(cc->cc_LocaleBase != NULL)
		{
			cc->cc_Catalog	= OpenCatalogA(NULL,"roadshow.catalog",NULL);
			cc->cc_Locale	= OpenLocale(NULL);
		}

		result = cmd(cc);

		if(cc->cc_QueryBuffer != NULL)
			FreeVec(cc->cc_QueryBuffer);

		if(cc->cc_Catalog != NULL)
			CloseCatalog(cc->cc_Catalog);

		if(cc->cc_Locale != NULL)
			CloseLocale(cc->cc_Locale);
	}
	else
	{
		pr->pr_Result2 = error;
	}

	close_libs(cc);

 out:

	return(result);
}

/****************************************************************************/

STATIC VOID
close_libs(struct CommandContext * cc)
{
	DECLARE_SYSBASE(cc);

	#if defined(__amigaos4__)
	{
		if(cc->cc_IDOS != NULL)
			DropInterface((struct Interface *)cc->cc_IDOS);

		if(cc->cc_ILocale != NULL)
			DropInterface((struct Interface *)cc->cc_ILocale);

		if(cc->cc_ISocket != NULL)
			DropInterface((struct Interface *)cc->cc_ISocket);

		if(cc->cc_IUtility != NULL)
			DropInterface((struct Interface *)cc->cc_IUtility);
	}
	#endif /* __amigaos4__ */

	if(cc->cc_UtilityBase != NULL)
		CloseLibrary(cc->cc_UtilityBase);

	if(cc->cc_SocketBase != NULL)
		CloseLibrary(cc->cc_SocketBase);

	if(cc->cc_LocaleBase != NULL)
		CloseLibrary(cc->cc_LocaleBase);

	if(cc->cc_DOSBase != NULL)
		CloseLibrary(cc->cc_DOSBase);
}

/****************************************************************************/

STATIC LONG
open_libs(struct CommandContext * cc,struct Library *SysBase)
{
	#if defined(__amigaos4__)
	struct ExecIFace * IExec = (struct ExecIFace *)((struct ExecBase *)SysBase)->MainInterface;
	#endif /* __amigaos4__ */

	LONG error;

	cc->cc_SysBase = SysBase;

	#if defined(__amigaos4__)
	{
		cc->cc_IExec = IExec;
	}
	#endif /* __amigaos4__ */

	cc->cc_DOSBase = OpenLibrary("dos.library",36);

	#if defined(__amigaos4__)
	{
		if(cc->cc_DOSBase != NULL)
		{
			cc->cc_IDOS = (struct DOSIFace *)GetInterface(cc->cc_DOSBase, "main", 1, 0);
			if(cc->cc_IDOS == NULL)
			{
				CloseLibrary(cc->cc_DOSBase);
				cc->cc_DOSBase = NULL;
			}
		}
	}
	#endif /* __amigaos4__ */

	if(cc->cc_DOSBase == NULL)
	{
		error = ERROR_INVALID_RESIDENT_LIBRARY;
		goto out;
	}

	cc->cc_LocaleBase = OpenLibrary("locale.library",38);

	#if defined(__amigaos4__)
	{
		if(cc->cc_LocaleBase != NULL)
		{
			cc->cc_ILocale = (struct LocaleIFace *)GetInterface(cc->cc_LocaleBase, "main", 1, 0);
			if(cc->cc_ILocale == NULL)
			{
				CloseLibrary(cc->cc_LocaleBase);
				cc->cc_LocaleBase = NULL;
			}
		}
	}
	#endif /* __amigaos4__ */

	cc->cc_UtilityBase = OpenLibrary("utility.library",37);

	#if defined(__amigaos4__)
	{
		if(cc->cc_UtilityBase != NULL)
		{
			cc->cc_IUtility = (struct UtilityIFace *)GetInterface(cc->cc_UtilityBase, "main", 1, 0);
			if(cc->cc_IUtility == NULL)
			{
				CloseLibrary(cc->cc_UtilityBase);
				cc->cc_UtilityBase = NULL;
			}
		}
	}
	#endif /* __amigaos4__ */

	/* The following may be necessary to flush out an inoperable
	   bsdsocket.library which reached the end of the shutdown
	   process. */
	#if 1
	{
		struct ExecBase * ex = (struct ExecBase *)SysBase;
		struct Library * lib;

		Forbid();

		lib = (struct Library *)FindName(&ex->LibList,"bsdsocket.library");
		if(lib != NULL)
			RemLibrary(lib);

		Permit();
	}
	#endif

	cc->cc_SocketBase = OpenLibrary("bsdsocket.library",4);

	#if defined(__amigaos4__)
	{
		if(cc->cc_SocketBase != NULL)
		{
			cc->cc_ISocket = (struct SocketIFace *)GetInterface(cc->cc_SocketBase, "main", 1, 0);
			if(cc->cc_ISocket == NULL)
			{
				CloseLibrary(cc->cc_SocketBase);
				cc->cc_SocketBase = NULL;
			}
		}
	}
	#endif /* __amigaos4__ */

	error = OK;

 out:

	return(error);
}

/****************************************************************************/

#define CATCOMP_ARRAY
#define SHOWNETSTATUS_CATALOG_STRINGS
#include "roadshow.h"

/****************************************************************************/

LONG
cmd(struct CommandContext * cc)
{
	struct
	{
		KEY *	Interface;
		SWITCH	Interfaces;
		SWITCH	ARP;
		SWITCH	Routes;
		SWITCH	DNS;
		SWITCH	ICMP;
		SWITCH	IGMP;
		SWITCH	IP;
		SWITCH	Memory;
		SWITCH	MulticastRouting;
		SWITCH	Routing;
		SWITCH	TCP;
		SWITCH	UDP;
		SWITCH	TCP_Sockets;
		SWITCH	UDP_Sockets;
		SWITCH	Names;
		SWITCH	All;
		SWITCH	Repeat;
		SWITCH	Quiet;
	} args;

	STRPTR args_template =
		"INTERFACE/M,"
		"INTERFACES/S,"
		"ARPCACHE=ARP/S,"
		"ROUTES/S,"
		"DNS=DOMAINNAMESERVERS/S,"
		"ICMP/S,"
		"IGMP/S,"
		"IP/S,"
		"MB=MEMORY/S,"
		"MR=MULTICASTROUTING/S,"
		"RT=ROUTING/S,"
		"TCP/S,"
		"UDP/S,"
		"TCPSOCKETS/S,"
		"UDPSOCKETS/S,"
		"NAMES/S,"
		"ALL/S,"
		"REPEAT/S,"
		"QUIET/S"
		VERSTAG;

	DECLARE_SYSBASE(cc);
	DECLARE_DOSBASE(cc);
	DECLARE_UTILITYBASE(cc);
	DECLARE_SOCKETBASE(cc);

	struct List * interface_list = NULL;
	struct List * dns_list = NULL;
	struct rt_msghdr * route_table = NULL;
	LONG result = RETURN_FAIL;
	LONG have_status_api = FALSE;
	LONG have_routing_api = FALSE;
	LONG have_interface_api = FALSE;
	LONG have_dns_api = FALSE;
	BOOL show_net_status_summary = FALSE;
	struct RDArgs * rda;

	GetProgramName(cc->cc_ProgramName,sizeof(cc->cc_ProgramName));

	memset(&args,0,sizeof(args));

	rda = ReadArgs(args_template,(LONG *)&args,NULL);
	if(rda == NULL)
	{
		PrintFault(IoErr(),cc->cc_ProgramName);
		goto out;
	}

	if(args.Interface			== NULL &&
	   args.Interfaces			== FALSE &&
	   args.ARP					== FALSE &&
	   args.Routes				== FALSE &&
	   args.DNS					== FALSE &&
	   args.ICMP				== FALSE &&
	   args.IGMP				== FALSE &&
	   args.IP					== FALSE &&
	   args.Memory				== FALSE &&
	   args.MulticastRouting	== FALSE &&
	   args.Routing				== FALSE &&
	   args.TCP					== FALSE &&
	   args.UDP					== FALSE &&
	   args.TCP_Sockets			== FALSE &&
	   args.UDP_Sockets			== FALSE)
	{
		show_net_status_summary = TRUE;
	}

	cc->cc_Quiet = args.Quiet;

	if(UtilityBase == NULL)
	{
		if(NOT cc->cc_Quiet)
		{
			Local_ErrorPrintf(cc,get_str(cc,MSG_SHOWNETSTATUS_NO_UTILITY_TXT),
				cc->cc_ProgramName);
		}

		goto out;
	}

	if(SocketBase == NULL)
	{
		if(NOT cc->cc_Quiet)
		{
			Local_ErrorPrintf(cc,get_str(cc,MSG_SHOWNETSTATUS_CANNOT_OPEN_BSDSOCKET_TXT),
				cc->cc_ProgramName);
		}

		goto out;
	}

	if(Local_SocketBaseTags(cc,
	    SBTM_GETREF(SBTC_HAVE_STATUS_API),&have_status_api,
	TAG_END) != 0)
	{
		have_status_api = FALSE;
	}

	if(NOT have_status_api)
	{
		if(NOT cc->cc_Quiet)
		{
			Local_ErrorPrintf(cc,get_str(cc,MSG_SHOWNETSTATUS_BSDSOCKET_HAS_NO_STATUS_API_TXT),
				cc->cc_ProgramName, SocketBase->lib_Node.ln_Name, SocketBase->lib_Version, SocketBase->lib_Revision);
		}

		goto out;
	}

	if(Local_SocketBaseTags(cc,
	    SBTM_GETREF(SBTC_HAVE_ROUTING_API),&have_routing_api,
	TAG_END) != 0)
	{
		have_routing_api = FALSE;
	}

	if(NOT have_routing_api)
	{
		if(NOT cc->cc_Quiet)
		{
			Local_ErrorPrintf(cc,get_str(cc,MSG_SHOWNETSTATUS_BSDSOCKET_HAS_NO_ROUTE_API_TXT),
				cc->cc_ProgramName, SocketBase->lib_Node.ln_Name, SocketBase->lib_Version, SocketBase->lib_Revision);
		}

		goto out;
	}

	if(Local_SocketBaseTags(cc,
	    SBTM_GETREF(SBTC_HAVE_INTERFACE_API),&have_interface_api,
	TAG_END) != 0)
	{
		have_interface_api = FALSE;
	}

	if(NOT have_interface_api)
	{
		if(NOT cc->cc_Quiet)
		{
			Local_ErrorPrintf(cc,get_str(cc,MSG_SHOWNETSTATUS_BSDSOCKET_HAS_NO_INTERFACE_API_TXT),
				cc->cc_ProgramName, SocketBase->lib_Node.ln_Name, SocketBase->lib_Version, SocketBase->lib_Revision);
		}

		goto out;
	}

	if(Local_SocketBaseTags(cc,
	    SBTM_GETREF(SBTC_HAVE_DNS_API),&have_dns_api,
	TAG_END) != 0)
	{
		have_dns_api = FALSE;
	}

	if(NOT have_dns_api)
	{
		if(NOT cc->cc_Quiet)
		{
			Local_ErrorPrintf(cc,get_str(cc,MSG_SHOWNETSTATUS_BSDSOCKET_HAS_NO_DNS_API_TXT),
				cc->cc_ProgramName, SocketBase->lib_Node.ln_Name, SocketBase->lib_Version, SocketBase->lib_Revision);
		}

		goto out;
	}

	cc->cc_Domain = AllocVec(3 * (MAXHOSTNAMELEN + 1),MEMF_ANY|MEMF_PUBLIC);
	if(cc->cc_Domain == NULL)
	{
		PrintFault(ERROR_NO_FREE_STORE,cc->cc_ProgramName);
		goto out;
	}

	cc->cc_Line1 = cc->cc_Domain + MAXHOSTNAMELEN + 1;
	cc->cc_Line2 = cc->cc_Line1 + MAXHOSTNAMELEN + 1;

	if(args.Names)
		cc->cc_NoNames = FALSE;
	else
		cc->cc_NoNames = TRUE;

	while(TRUE)
	{
		if(show_net_status_summary)
			print_net_status_summary(cc);

		if(args.Interface != NULL)
		{
			STRPTR * key = args.Interface;
			STRPTR name;
			BOOL failed;

			failed = FALSE;

			while((name = (*key++)) != NULL)
			{
				if(CANNOT print_interface_info(cc,name))
				{
					if(NOT cc->cc_Quiet)
					{
						Local_ErrorPrintf(cc,get_str(cc,MSG_SHOWNETSTATUS_CANNOT_GET_INFO_ON_INTERFACE_TXT),
							cc->cc_ProgramName,name);
					}

					failed = TRUE;
				}
			}

			if(failed)
				goto out;
		}

		if(args.Interfaces)
		{
			if(interface_list != NULL)
				ReleaseInterfaceList(interface_list);

			interface_list = ObtainInterfaceList();
			if(interface_list == NULL)
			{
				if(NOT cc->cc_Quiet)
				{
					Local_ErrorPrintf(cc,get_str(cc,MSG_SHOWNETSTATUS_CANNOT_GET_INTERFACE_LIST_TXT),
						cc->cc_ProgramName);
				}

				goto out;
			}

			print_interface_list(cc,interface_list);
		}

		if(args.DNS)
		{
			if(dns_list != NULL)
				ReleaseDomainNameServerList(dns_list);

			dns_list = ObtainDomainNameServerList();
			if(dns_list == NULL)
			{
				if(NOT cc->cc_Quiet)
				{
					Local_ErrorPrintf(cc,get_str(cc,MSG_SHOWNETSTATUS_CANNOT_GET_DNS_LIST_TXT),
						cc->cc_ProgramName);
				}

				goto out;
			}

			print_dns_list(cc,dns_list);
		}

		if(args.Routes)
		{
			if(route_table != NULL)
				FreeRouteInfo(route_table);

			route_table = GetRouteInfo(AF_UNSPEC,0);
			if(route_table == NULL)
			{
				if(NOT cc->cc_Quiet)
				{
					Local_ErrorPrintf(cc,get_str(cc,MSG_SHOWNETSTATUS_CANNOT_GET_ROUTE_TABLE_TXT),
						cc->cc_ProgramName);
				}

				goto out;
			}

			print_route_table(cc,route_table);
		}

		if(args.ARP)
		{
			if(route_table != NULL)
				FreeRouteInfo(route_table);

			route_table = GetRouteInfo(AF_INET,RTF_LLINFO);
			if(route_table == NULL)
			{
				if(NOT cc->cc_Quiet)
				{
					Local_ErrorPrintf(cc,get_str(cc,MSG_SHOWNETSTATUS_NO_ARP_INFORMATION_TXT),
						cc->cc_ProgramName);
				}

				goto out;
			}

			print_arp_table(cc,route_table);
		}

		if(args.ICMP)
		{
			if(fill_query_buffer(cc,NETSTATUS_icmp) < 0)
				goto out;

			print_icmp_status(cc,(struct icmpstat *)cc->cc_QueryBuffer);
			if(cc->cc_Stopped)
			{
				result = RETURN_WARN;
				goto out;
			}
		}

		if(args.IGMP)
		{
			if(fill_query_buffer(cc,NETSTATUS_igmp) < 0)
				goto out;

			print_igmp_status(cc,(struct igmpstat *)cc->cc_QueryBuffer);

			if(cc->cc_Stopped)
			{
				result = RETURN_WARN;
				goto out;
			}
		}

		if(args.IP)
		{
			if(fill_query_buffer(cc,NETSTATUS_ip) < 0)
				goto out;

			print_ip_status(cc,(struct ipstat *)cc->cc_QueryBuffer);

			if(cc->cc_Stopped)
			{
				result = RETURN_WARN;
				goto out;
			}
		}

		if(args.Memory)
		{
			if(fill_query_buffer(cc,NETSTATUS_mb) < 0)
				goto out;

			print_mb_status(cc,(struct mbstat *)cc->cc_QueryBuffer);

			if(cc->cc_Stopped)
			{
				result = RETURN_WARN;
				goto out;
			}
		}

		if(args.MulticastRouting)
		{
			if(fill_query_buffer(cc,NETSTATUS_mrt) < 0)
				goto out;

			print_mrt_status(cc,(struct mrtstat *)cc->cc_QueryBuffer);

			if(cc->cc_Stopped)
			{
				result = RETURN_WARN;
				goto out;
			}
		}

		if(args.Routing)
		{
			if(fill_query_buffer(cc,NETSTATUS_rt) < 0)
				goto out;

			print_rt_status(cc,(struct rtstat *)cc->cc_QueryBuffer);

			if(cc->cc_Stopped)
			{
				result = RETURN_WARN;
				goto out;
			}
		}

		if(args.TCP)
		{
			if(fill_query_buffer(cc,NETSTATUS_tcp) < 0)
				goto out;

			print_tcp_status(cc,(struct tcpstat *)cc->cc_QueryBuffer);

			if(cc->cc_Stopped)
			{
				result = RETURN_WARN;
				goto out;
			}
		}

		if(args.UDP)
		{
			if(fill_query_buffer(cc,NETSTATUS_udp) < 0)
				goto out;

			print_udp_status(cc,(struct udpstat *)cc->cc_QueryBuffer);

			if(cc->cc_Stopped)
			{
				result = RETURN_WARN;
				goto out;
			}
		}

		if(args.TCP_Sockets)
		{
			LONG size;

			size = fill_query_buffer(cc,NETSTATUS_tcp_sockets);
			if(size < 0)
				goto out;

			print_tcp_socket_status(cc,args.All,(struct protocol_connection_data *)cc->cc_QueryBuffer,size);

			if(cc->cc_Stopped)
			{
				result = RETURN_WARN;
				goto out;
			}
		}

		if(args.UDP_Sockets)
		{
			LONG size;

			size = fill_query_buffer(cc,NETSTATUS_udp_sockets);
			if(size < 0)
				goto out;

			print_udp_socket_status(cc,args.All,(struct protocol_connection_data *)cc->cc_QueryBuffer,size);

			if(cc->cc_Stopped)
			{
				result = RETURN_WARN;
				goto out;
			}
		}

		if(args.Repeat)
		{
			LONG i;

			for(i = 0 ; i < 10 ; i++)
			{
				Delay(TICKS_PER_SECOND / 10);

				if(CheckSignal(SIGBREAKF_CTRL_C))
				{
					cc->cc_Stopped = TRUE;

					PrintFault(ERROR_BREAK,cc->cc_ProgramName);

					result = RETURN_WARN;

					break;
				}
			}

			if(cc->cc_Stopped)
			{
				result = RETURN_WARN;
				goto out;
			}

			cc->cc_Something = FALSE;
			lprintf(cc,"\f");
		}
		else
		{
			break;
		}
	}

	result = RETURN_OK;

 out:

	FreeVec(cc->cc_Domain);

	if(interface_list != NULL)
		ReleaseInterfaceList(interface_list);

	if(dns_list != NULL)
		ReleaseDomainNameServerList(dns_list);

	if(route_table != NULL)
		FreeRouteInfo(route_table);

	if(rda != NULL)
		FreeArgs(rda);

	return(result);
}

STATIC STRPTR
begin_underline(struct CommandContext * cc)
{
	DECLARE_DOSBASE(cc);

	STRPTR result;

	if(IsInteractive(Output()))
		result = "\33[4m";
	else
		result = "";

	return(result);
}

STATIC STRPTR
end_underline(struct CommandContext * cc)
{
	DECLARE_DOSBASE(cc);

	STRPTR result;

	if(IsInteractive(Output()))
		result = "\33[0m";
	else
		result = "";

	return(result);
}

/****************************************************************************/

/* Divide a 64 bit integer by a 32 bit integer, filling in a 64 bit quotient
   and returning a 32 bit remainder. */
STATIC ULONG
divide_64_by_32(SBQUAD_T * dividend,ULONG divisor,SBQUAD_T * quotient)
{
	SBQUAD_T dividend_cdef = (*dividend);
	ULONG dividend_ab = 0;
	LONG i;

	quotient->sbq_High = quotient->sbq_Low = 0;

	for(i = 0 ; i < 64 ; i++)
	{
		/* Shift the quotient left by one bit. */
		quotient->sbq_High = (quotient->sbq_High << 1);

		if((quotient->sbq_Low & 0x80000000UL) != 0)
			quotient->sbq_High |= 1;

		quotient->sbq_Low = (quotient->sbq_Low << 1);

		/* Shift the dividend left by one bit. We start
		   with the most significant 32 bit portion. */
		dividend_ab = (dividend_ab << 1);

		if((dividend_cdef.sbq_High & 0x80000000UL) != 0)
			dividend_ab |= 1;

		/* Now for the middle 32 bit portion. */
		dividend_cdef.sbq_High = (dividend_cdef.sbq_High << 1);

		if((dividend_cdef.sbq_Low & 0x80000000UL) != 0)
			dividend_cdef.sbq_High |= 1;

		/* Finally, the least significant portion. */
		dividend_cdef.sbq_Low = (dividend_cdef.sbq_Low << 1);

		/* Does the divisor actually divide the dividend? */
		if(dividend_ab >= divisor)
		{
			dividend_ab -= divisor;

			/* We could divide the divisor. Keep track of
			   this and take care of an overflow condition. */
			quotient->sbq_Low++;
			if(quotient->sbq_Low == 0)
				quotient->sbq_High++;
		}
	}

	return(dividend_ab);
}

/****************************************************************************/

STATIC VOID
insert_grouping_characters(struct Locale * loc,STRPTR buffer)
{
	if(loc != NULL && loc->loc_Grouping != NULL && loc->loc_GroupSeparator != NULL)
	{
		UBYTE * grouping = loc->loc_Grouping;
		UBYTE * group_separator = loc->loc_GroupSeparator;
		UBYTE local_buffer[80];
		LONG num_group_digits;
		UBYTE * s;
		LONG i,len;

		/* Check which kind of grouping should be performed
		   for the first group. */
		switch(*grouping)
		{
			/* Repeat current grouping scheme; there is nothing
			   to repeat in the first place since we don't even
			   know yet what we are going to repeat. */
			case 0:

				return;

			/* Perform no further grouping. That means our job is
			   done! */
			case 255:

				return;

			/* Use this initial group size. */
			default:

				num_group_digits = (*grouping++);
				break;
		}

		/* Run down the number string, inserting grouping
		   characters where necessary. */
		s = local_buffer;
		len = strlen(buffer);
		for(i = len-1 ; i >= 0 ; i--)
		{
			(*s++) = buffer[i];

			/* Is this not the last character to be stored?
			   If so, check if we should insert a grouping
			   character right now. */
			if(i != 0 && num_group_digits > 0)
			{
				if(--num_group_digits == 0)
				{
					int j;

					/* Insert the group separator in reverse order. */
					for(j = strlen(group_separator)-1 ; j >= 0 ; j--)
						(*s++) = group_separator[j];

					/* Now check which kind of grouping we should
					   perform next. */
					switch(*grouping)
					{
						/* Repeat current grouping scheme. */
						case 0:

							num_group_digits = grouping[-1];
							break;

						/* Perform no further grouping. */
						case 255:

							break;

						/* Use new grouping scheme. */
						default:

							num_group_digits = (*grouping++);
							break;
					}
				}
			}
		}

		len = (int)(((ULONG)s) - ((ULONG)local_buffer));

		for(i = len-1 ; i >= 0 ; i--)
			(*buffer++) = local_buffer[i];

		(*buffer) = '\0';
	}
}

/****************************************************************************/

/* Convert a 64 bit number into a textual representation, using base=10,
   just like sprintf(...,"%lD",...) would do if it could handle multiprecision
   numbers... */
STATIC STRPTR
convert_quad_to_string(struct Locale * loc,const SBQUAD_T * const number,STRPTR string,LONG max_string_len)
{
	SBQUAD_T q;
	STRPTR s;
	UBYTE c;
	ULONG r;
	LONG i,len;

	/* Make a local copy of the number. */
	q = (*number);

	s = string;

	len = 0;

	/* Build the number string in reverse order, calculating
	   the single digits: */
	while(len < max_string_len)
	{
		/* Divide by ten and remember the remainder. */
		r = divide_64_by_32(&q,10,&q);

		(*s++) = '0' + r;
		len++;

		/* Stop when we hit zero. */
		if(q.sbq_High == 0 && q.sbq_Low == 0)
			break;		
	}

	/* Don't forget to terminate the string. */
	(*s) = '\0';

	/* Reverse the string in place. */
	for(i = 0 ; i < len/2 ; i++)
	{
		c = string[len-1-i];
		string[len-1-i] = string[i];
		string[i] = c;
	}

	insert_grouping_characters(loc,string);

	return(string);
}

/****************************************************************************/

STATIC LONG
fill_query_buffer(struct CommandContext * cc,LONG type)
{
	DECLARE_SYSBASE(cc);
	DECLARE_SOCKETBASE(cc);

	LONG size;

	size = GetNetworkStatistics(type,NETWORKSTATUS_VERSION,NULL,0);
	if(size > 0)
	{
		if(size > cc->cc_QueryBufferSize)
		{
			FreeVec(cc->cc_QueryBuffer);

			cc->cc_QueryBuffer = AllocVec(size,MEMF_ANY|MEMF_CLEAR);
			if(cc->cc_QueryBuffer != NULL)
			{
				cc->cc_QueryBufferSize = size;
			}
			else
			{
				if(NOT cc->cc_Quiet)
				{
					Local_ErrorPrintf(cc,get_str(cc,MSG_SHOWNETSTATUS_NOT_ENOUGH_MEMORY_TO_QUERY_STATUS_TXT),
						cc->cc_ProgramName,get_str(cc,MSG_SHOWNETSTATUS_STATUS_NAME1_TXT + type - NETSTATUS_icmp));
				}

				size = -1;
				goto out;
			}
		}

		size = GetNetworkStatistics(type,NETWORKSTATUS_VERSION,cc->cc_QueryBuffer,size);
	}

	if(size < 0)
	{
		if(NOT cc->cc_Quiet)
		{
			Local_ErrorPrintf(cc,get_str(cc,MSG_SHOWNETSTATUS_CANNOT_GET_STATUS_INFORMATION_FOR_TXT),
				cc->cc_ProgramName,get_str(cc,MSG_SHOWNETSTATUS_STATUS_NAME1_TXT + type - NETSTATUS_icmp));
		}
	}

 out:

	return(size);
}

/****************************************************************************/

STATIC VARARGS68K VOID
lprintf(struct CommandContext * cc,STRPTR format,...)
{
	DECLARE_DOSBASE(cc);

	if(NOT cc->cc_Stopped)
	{
		if(CheckSignal(SIGBREAKF_CTRL_C))
		{
			PrintFault(ERROR_BREAK,cc->cc_ProgramName);
			cc->cc_Stopped = TRUE;
		}
	}

	if(NOT cc->cc_Stopped)
	{
		va_list args;

		#if defined(__amigaos4__)
		{
			va_startlinear(args,format);
			VPrintf(format,va_getlinearva(args,APTR));
			va_end(args);
		}
		#else
		{
			va_start(args,format);
			VPrintf(format,(APTR)args);
			va_end(args);
		}
		#endif /* __amigaos4__ */
	}
}

/****************************************************************************/

STATIC VARARGS68K VOID
lsprintf(struct CommandContext * cc,STRPTR buffer,STRPTR formatString,...)
{
	DECLARE_SYSBASE(cc);

	va_list varArgs;

	#if defined(__amigaos4__)
	{
		va_startlinear(varArgs,formatString);
		RawDoFmt(formatString,va_getlinearva(varArgs,APTR),NULL,buffer);
		va_end(varArgs);
	}
	#else
	{
		va_start(varArgs,formatString);
		RawDoFmt(formatString,varArgs,(VOID (*)())"\x16\xC0\x4E\x75",buffer);
		va_end(varArgs);
	}
	#endif /* __amigaos4__ */
}

/****************************************************************************/

STATIC BOOL
is_blank_space(UBYTE c)
{
	BOOL result;

	result = (BOOL)(c == ' ' || c == '\t' || c == (UBYTE)'\240');

	return(result);
}

/****************************************************************************/

STATIC VOID
strip_extra_blank_spaces(STRPTR s)
{
	LONG num_leading_spaces;
	LONG num_trailing_spaces;
	LONG len,i;

	len = strlen(s);

	num_leading_spaces = 0;

	for(i = 0 ; i < len ; i++)
	{
		if(NOT is_blank_space(s[i]))
			break;

		num_leading_spaces++;
	}

	num_trailing_spaces = 0;

	for(i = len-1 ; i >= 0 ; i--)
	{
		if(NOT is_blank_space(s[i]))
			break;

		num_trailing_spaces++;
	}

	if(num_trailing_spaces > 0)
	{
		len -= num_trailing_spaces;
		s[len] = '\0';
	}

	if(num_leading_spaces > 0)
		memmove(s,&s[num_leading_spaces],len+1);
}

/****************************************************************************/

#define C(x) ((x) & 0xff)

/****************************************************************************/

STATIC STRPTR
routename(struct CommandContext * cc,ULONG in,STRPTR line,LONG line_size)
{
	DECLARE_UTILITYBASE(cc);
	DECLARE_SOCKETBASE(cc);

	struct hostent *hp;
	STRPTR cp;

	cp = NULL;

	if(NOT cc->cc_NoNames)
	{
		if(NOT cc->cc_HaveDomain)
		{
			cc->cc_HaveDomain = TRUE;

			if(gethostname(cc->cc_Domain, MAXHOSTNAMELEN) == 0 && (cp = index(cc->cc_Domain, '.')) != NULL)
				strcpy(cc->cc_Domain, cp + 1);
			else
				cc->cc_Domain[0] = '\0';
		}

		hp = gethostbyaddr((char *)&in, sizeof(struct in_addr), AF_INET);
		if(hp != NULL)
		{
			if((cp = index(hp->h_name, '.')) != NULL && Stricmp(cp + 1, cc->cc_Domain) == SAME)
				(*cp) = 0;

			cp = hp->h_name;
		}
	}

	if(cp != NULL)
	{
		strncpy(line, cp, line_size - 1);
		line[line_size - 1] = '\0';
	}
	else
	{
		in = ntohl(in);
		lsprintf(cc, line, "%ld.%ld.%ld.%ld", C(in >> 24), C(in >> 16), C(in >> 8), C(in));
	}

	return(line);
}

STATIC ULONG
forgemask(ULONG a)
{
	ULONG m;

	if(IN_CLASSA(a))
		m = IN_CLASSA_NET;
	else if (IN_CLASSB(a))
		m = IN_CLASSB_NET;
	else
		m = IN_CLASSC_NET;

	return (m);
}

/* Return the name of the network whose address is given.
   The address is assumed to be that of a net or subnet, not a host. */
STATIC STRPTR
netname(struct CommandContext * cc, ULONG in, STRPTR line, LONG line_size)
{
	DECLARE_SOCKETBASE(cc);

	STRPTR cp = NULL;
	ULONG net;
	ULONG i;

	i = ntohl(in);

	if(NOT cc->cc_NoNames && i)
	{
		ULONG mask;
		int subnetshift;

		mask = forgemask(i);
		switch(mask)
		{
			case IN_CLASSA_NET:

				subnetshift = 8;
				break;

			case IN_CLASSB_NET:

				subnetshift = 8;
				break;

			case IN_CLASSC_NET:

				subnetshift = 4;
				break;

			default:

				subnetshift = 0;
				break;
		}

		if(subnetshift > 0)
		{
			struct netent *np;

			/* If there are more bits than the standard mask
			   would suggest, subnets must be in use.
			   Guess at the subnet mask, assuming reasonable
			   width subnet fields. */
			while(i &~ mask)
				mask = (long)mask >> subnetshift;

			net = i & mask;
			while((mask & 1) == 0)
				mask >>= 1, net >>= 1;

			np = getnetbyaddr(net, AF_INET);
			if(np != NULL)
				cp = np->n_name;
		}
	}

	if(cp != NULL)
	{
		strncpy(line, cp, line_size - 1);
		line[line_size - 1] = '\0';
	}
	else if ((i & 0xffffff) == 0)
	{
		lsprintf(cc, line, "%ld", C(i >> 24));
	}
	else if ((i & 0xffff) == 0)
	{
		lsprintf(cc, line, "%ld.%ld", C(i >> 24) , C(i >> 16));
	}
	else if ((i & 0xff) == 0)
	{
		lsprintf(cc, line, "%ld.%ld.%ld", C(i >> 24), C(i >> 16), C(i >> 8));
	}
	else
	{
		lsprintf(cc, line, "%ld.%ld.%ld.%ld", C(i >> 24), C(i >> 16), C(i >> 8), C(i));
	}

	return (line);
}

STATIC STRPTR
get_sockaddr(struct CommandContext * cc,struct sockaddr * sa,int flags,STRPTR line,LONG line_size)
{
	struct sockaddr_in *sin = (struct sockaddr_in *)sa;
	STRPTR result;

	if(sin->sin_addr.s_addr == INADDR_ANY)
		result = get_str(cc,MSG_SHOWNETSTATUS_DEFAULT_TXT);
	else if (flags & RTF_HOST)
		result = routename(cc,sin->sin_addr.s_addr,line,line_size);
	else
		result = netname(cc,sin->sin_addr.s_addr,line,line_size);

	return(result);
}

/****************************************************************************/

STATIC VOID
print_arp_table(struct CommandContext * cc,struct rt_msghdr *rtm)
{
	DECLARE_SOCKETBASE(cc);

	char *host;
	struct sockaddr_inarp *sin;
	struct sockaddr_dl *sdl;
	struct hostent *hp;
	char address[24];
	LONG n,c;
	BOOL no_names;

	if(cc->cc_Something)
		lprintf(cc,"\n");
	else
		cc->cc_Something = TRUE;

	no_names = cc->cc_NoNames;

	for(n = 0 ;
		rtm->rtm_msglen > 0 ;
		rtm = (struct rt_msghdr *)(((ULONG)rtm) + rtm->rtm_msglen))
	{
		sin = (struct sockaddr_inarp *)(rtm + 1);
		sdl = (struct sockaddr_dl *)(sin + 1);

		host = get_str(cc,MSG_SHOWNETSTATUS_UNKNOWN_HOST_TXT);

		if(NOT no_names)
		{
			hp = gethostbyaddr((APTR)&sin->sin_addr, sizeof (sin->sin_addr), AF_INET);
			if(hp != NULL)
			{
				host = hp->h_name;
			}
			else
			{
				LONG h_errno;

				Local_SocketBaseTags(cc,
				    SBTM_GETREF(SBTC_HERRNO),&h_errno,
				TAG_END);

				if(h_errno == TRY_AGAIN)
					no_names = TRUE;
			}
		}

		if(n++ == 0)
		{
			lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_ARP_HEADER_TXT),
				begin_underline(cc),
					get_str(cc,MSG_SHOWNETSTATUS_HOST_TXT),
					get_str(cc,MSG_SHOWNETSTATUS_IP_ADDRESS_TXT),
					get_str(cc,MSG_SHOWNETSTATUS_HARDWARE_ADDRESS_TXT),
					get_str(cc,MSG_SHOWNETSTATUS_ATTRIBUTES_TXT),
				end_underline(cc));

			lprintf(cc,"\n");
		}

		if(sdl->sdl_alen > 0)
		{
			UBYTE * m;

			m = (UBYTE *)LLADDR(sdl);

			lsprintf(cc,address,"%02lx:%02lx:%02lx:%02lx:%02lx:%02lx",m[0],m[1],m[2],m[3],m[4],m[5]);
		}
		else
		{
			strcpy(address,get_str(cc,MSG_SHOWNETSTATUS_INCOMPLETE_HARDWARE_ADDRESS_TXT));
		}

		lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_ARP_HEADER_TXT),
			"",
				host,
				Inet_NtoA(sin->sin_addr.s_addr),
				address,
				"",
			"");

		c = 0;

		if (rtm->rtm_rmx.rmx_expire == 0)
		{
			lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_ARP_ENTRY_PERMANENT_TXT));
			c++;
		}

		if (sin->sin_other & SIN_PROXY)
		{
			if(c != 0)
				lprintf(cc,", ");

			lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_ARP_ENTRY_PUBLISHED_PROXY_TXT));
			c++;
		}

		if (rtm->rtm_addrs & RTA_NETMASK)
		{
			sin = (struct sockaddr_inarp *)(sdl->sdl_len + (char *)sdl);

			if (sin->sin_addr.s_addr == 0xffffffff)
			{
				if(c != 0)
					lprintf(cc,", ");

				lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_ARP_ENTRY_PUBLISHED_TXT));
				c++;
			}
		}

		if(c == 0)
			lprintf(cc,"-");

		lprintf(cc,"\n");
	}

	if(n == 0)
		lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_ARP_CACHE_EMPTY_TXT));
}

/****************************************************************************/

STATIC VOID
print_route_table(struct CommandContext * cc,struct rt_msghdr * rtm)
{
	struct sockaddr * sa;
	STRPTR gateway,destination;
	LONG n;

	if(cc->cc_Something)
		lprintf(cc,"\n");
	else
		cc->cc_Something = TRUE;

	for(n = 0 ;
		rtm->rtm_msglen > 0 ;
		rtm = (struct rt_msghdr *)(((ULONG)rtm) + rtm->rtm_msglen))
	{
		if(rtm->rtm_version != 3 || (rtm->rtm_addrs & RTA_DST) == 0)
			continue;

		sa = (struct sockaddr *)(rtm + 1);
		if(sa->sa_family != AF_INET)
			continue;

		if((rtm->rtm_addrs & RTA_GATEWAY) != 0)
		{
			LONG len;

			destination = get_sockaddr(cc,sa,rtm->rtm_flags,cc->cc_Line1,MAXHOSTNAMELEN);

			if(sa->sa_len == 0)
				len = sizeof(long);
			else
				len = sa->sa_len;

			sa = (struct sockaddr *)(((ULONG)sa) + len);
			if(sa->sa_family != AF_INET)
				continue;

			gateway = get_sockaddr(cc,sa,rtm->rtm_flags,cc->cc_Line2,MAXHOSTNAMELEN);
		}
		else
		{
			destination = get_sockaddr(cc,sa,rtm->rtm_flags,cc->cc_Line1,MAXHOSTNAMELEN);

			gateway = "-";
		}

		if(n++ == 0)
		{
			lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_DESTINATION_GATEWAY_ATTRIBUTES_TITLE_TXT),
				begin_underline(cc),
					get_str(cc,MSG_SHOWNETSTATUS_DESTINATION_TXT),
					get_str(cc,MSG_SHOWNETSTATUS_GATEWAY_TXT),
					get_str(cc,MSG_SHOWNETSTATUS_ATTRIBUTES_TXT),
				end_underline(cc));
		}

		lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_DESTINATION_GATEWAY_LINE_TXT),destination,gateway);

		lprintf(cc," ");

		if(rtm->rtm_flags & RTF_UP)
			lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_UP_TXT));
		else
			lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_DOWN_TXT));

		if(rtm->rtm_flags & RTF_GATEWAY)
		{
			lprintf(cc," ");
			lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_GATEWAY_TXT));
		}

		if(rtm->rtm_flags & RTF_HOST)
		{
			lprintf(cc," ");
			lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_HOST_TXT));
		}

		lprintf(cc,"\n");
	}

	if(n == 0)
		lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_NO_ROUTES_ARE_CONFIGURED_TXT));
}

/****************************************************************************/

STATIC VOID
print_dns_list(struct CommandContext * cc,struct List * dns_list)
{
	DECLARE_SOCKETBASE(cc);

	struct DomainNameServerNode * dnsn;
	UBYTE default_domain_name[MAXHOSTNAMELEN];
	LONG n;

	if(cc->cc_Something)
		lprintf(cc,"\n");
	else
		cc->cc_Something = TRUE;

	n = 0;

	for(dnsn = (struct DomainNameServerNode *)dns_list->lh_Head ;
	    dnsn->dnsn_MinNode.mln_Succ != NULL ;
	    dnsn = (struct DomainNameServerNode *)dnsn->dnsn_MinNode.mln_Succ)
	{
		if(n++ == 0)
		{
			lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_ADDRESS_TYPE_TITLE_TXT),
				begin_underline(cc),
					get_str(cc,MSG_SHOWNETSTATUS_ADDRESS_TXT),
					get_str(cc,MSG_SHOWNETSTATUS_TYPE_TXT),
				end_underline(cc));
		}

		lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_ADDRESS_TYPE_LINE_TXT),routename(cc,inet_addr(dnsn->dnsn_Address),cc->cc_Line1,MAXHOSTNAMELEN),
			get_str(cc,(dnsn->dnsn_UseCount < 0) ? MSG_SHOWNETSTATUS_STATIC_TXT : MSG_SHOWNETSTATUS_DYNAMIC_TXT));
	}

	if(n == 0)
		lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_NO_DNS_CONFIGURED_TXT));

	strcpy(default_domain_name,"");

	GetDefaultDomainName(default_domain_name,sizeof(default_domain_name));

	lprintf(cc,
		get_str(cc,default_domain_name[0] != '\0' ? MSG_SHOWNETSTATUS_DEFAULT_DOMAIN_NAME_TXT : MSG_SHOWNETSTATUS_DEFAULT_DOMAIN_NAME_UNSET_TXT),
		default_domain_name);
}

/****************************************************************************/

struct { LONG Number; STRPTR Name;} hardware_types[] =
{
	{ S2WireType_Ethernet,	"Ethernet" },
	{ S2WireType_IEEE802,	"IEEE 802" },
	{ S2WireType_Arcnet,	"Arcnet" },
	{ S2WireType_LocalTalk,	"LocalTalk" },
	{ S2WireType_DyLAN,		"DyLAN" },
	{ S2WireType_AmokNet,	"AmokNet" },
	{ S2WireType_Liana,		"Liana" },
	{ S2WireType_PPP,		"PPP" },
	{ S2WireType_SLIP,		"SLIP" },
	{ S2WireType_CSLIP,		"CSLIP" },
	{ S2WireType_PLIP,		"PLIP" }
};

/****************************************************************************/

STATIC VOID
print_interface_list(struct CommandContext * cc,struct List * interface_list)
{
	UBYTE type_buffer[80];
	STRPTR type;
	struct Node * node;
	struct sockaddr_in sin;
	LONG mtu;
	LONG hardware_type;
	ULONG sent;
	ULONG received;
	ULONG bad;
	ULONG overrun;
	ULONG unknown;
	LONG state;
	LONG i;
	STRPTR addr;
	LONG n = 0;

	if(cc->cc_Something)
		lprintf(cc,"\n");
	else
		cc->cc_Something = TRUE;

	for(node = interface_list->lh_Head ;
	    node->ln_Succ != NULL ;
	    node = node->ln_Succ)
	{
		memset(&sin,0,sizeof(sin));
		mtu = hardware_type = received = sent = bad = overrun = unknown = state = 0;

		if(Local_QueryInterfaceTags(cc,node->ln_Name,
			IFQ_MTU,				&mtu,
			IFQ_HardwareType,		&hardware_type,
			IFQ_PacketsReceived,	&received,
			IFQ_PacketsSent,		&sent,
			IFQ_BadData,			&bad,
			IFQ_Overruns,			&overrun,
			IFQ_UnknownTypes,		&unknown,
			IFQ_Address,			&sin,
			IFQ_State,				&state,
		TAG_DONE) != 0)
		{
			continue;
		}

		type = NULL;
		for(i = 0 ; i < (LONG)NUM_ENTRIES(hardware_types) ; i++)
		{
			if(hardware_types[i].Number == hardware_type)
			{
				type = hardware_types[i].Name;
				break;
			}
		}

		if(type == NULL)
			lsprintf(cc,type = type_buffer,get_str(cc,MSG_SHOWNETSTATUS_UNKNOWN_HARDWARE_TYPE_TXT),hardware_type);

		if(sin.sin_addr.s_addr != INADDR_ANY)
			addr = routename(cc,sin.sin_addr.s_addr,cc->cc_Line1,MAXHOSTNAMELEN);
		else
			addr = "-";

		if(n++ == 0)
		{
			lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_INTERFACE_INFO_TITLE_TXT),
				begin_underline(cc),
					get_str(cc,MSG_SHOWNETSTATUS_NAME_TXT),
					get_str(cc,MSG_SHOWNETSTATUS_MTU_TXT),
					get_str(cc,MSG_SHOWNETSTATUS_TYPE_TXT),
					get_str(cc,MSG_SHOWNETSTATUS_ADDRESS_TXT),
					get_str(cc,MSG_SHOWNETSTATUS_RECEIVED_TXT),
					get_str(cc,MSG_SHOWNETSTATUS_SENT_TXT),
					get_str(cc,MSG_SHOWNETSTATUS_DROPPED_TXT),
					get_str(cc,MSG_SHOWNETSTATUS_OVERRUNS_TXT),
					get_str(cc,MSG_SHOWNETSTATUS_UNKNOWN_TXT),
					get_str(cc,MSG_SHOWNETSTATUS_STATUS_TXT),
				end_underline(cc));
		}

		lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_INTERFACE_INFO_LINE_TXT),
			node->ln_Name,mtu,type,addr,received,sent,bad,overrun,unknown,get_str(cc,(state == SM_Up) ? MSG_SHOWNETSTATUS_UP_TXT : MSG_SHOWNETSTATUS_DOWN_TXT));
	}

	if(n == 0)
		lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_NO_INTERFACES_AVAILABLE_TXT));
}

STATIC BOOL
print_interface_info(struct CommandContext * cc,STRPTR name)
{
	DECLARE_DOSBASE(cc);
	DECLARE_SOCKETBASE(cc);

	struct SANA2CopyStats s2cs;
	UBYTE type_buffer[80];
	STRPTR type;
	struct sockaddr_in sin;
	struct sockaddr_in sin_peer;
	struct sockaddr_in sin_mask;
	LONG bps;
	UBYTE hardware_address[16];
	LONG hardware_address_size;
	STRPTR device_name;
	LONG device_unit;
	LONG mtu;
	LONG hardware_type;
	ULONG sent;
	ULONG received;
	ULONG bad;
	ULONG overrun;
	ULONG unknown;
	LONG bind_type;
	struct DateStamp lease_expire;
	LONG num_read_requests;
	LONG max_read_requests;
	LONG num_pending_read_requests;
	LONG num_write_requests;
	LONG max_write_requests;
	LONG num_pending_write_requests;
	LONG state;
	LONG i;
	SBQUAD_T bytes_in;
	SBQUAD_T bytes_out;
	UBYTE byte_count[80];
	BOOL success;
	BOOL have_peer_address;
	LONG input_dropped = 0;
	LONG output_dropped = 0;

	if(cc->cc_Something)
		lprintf(cc,"\n");
	else
		cc->cc_Something = TRUE;

	memset(&sin,0,sizeof(sin));
	memset(&sin_peer,0,sizeof(sin_peer));
	memset(&sin_mask,0,sizeof(sin_mask));
	memset(hardware_address,0,sizeof(hardware_address));
	device_name = NULL;
	device_unit = 0;
	hardware_address_size = mtu = bps = hardware_type = received = sent = bad = overrun = unknown = state = 0;
	bind_type = IFABT_Unknown;
	memset(&lease_expire,0,sizeof(lease_expire));
	memset(&s2cs,0,sizeof(s2cs));

	if(Local_QueryInterfaceTags(cc,name,
		IFQ_DeviceName,					&device_name,
		IFQ_DeviceUnit,					&device_unit,
		IFQ_HardwareAddressSize,		&hardware_address_size,
		IFQ_HardwareAddress,			hardware_address,
		IFQ_MTU,						&mtu,
		IFQ_BPS,						&bps,
		IFQ_HardwareType,				&hardware_type,
		IFQ_PacketsReceived,			&received,
		IFQ_PacketsSent,				&sent,
		IFQ_BadData,					&bad,
		IFQ_Overruns,					&overrun,
		IFQ_UnknownTypes,				&unknown,
		IFQ_Address,					&sin,
		IFQ_NetMask,					&sin_mask,
		IFQ_State,						&state,
		IFQ_AddressBindType,			&bind_type,
		IFQ_AddressLeaseExpires,		&lease_expire,
		IFQ_NumReadRequests,			&num_read_requests,
		IFQ_MaxReadRequests,			&max_read_requests,
		IFQ_NumReadRequestsPending,		&num_pending_read_requests,
		IFQ_NumWriteRequests,			&num_write_requests,
		IFQ_MaxWriteRequests,			&max_write_requests,
		IFQ_NumWriteRequestsPending,	&num_pending_write_requests,
		IFQ_GetBytesIn,					&bytes_in,
		IFQ_GetBytesOut,				&bytes_out,
		IFQ_GetSANA2CopyStats,			&s2cs,
		IFQ_InputDrops,					&input_dropped,
		IFQ_OutputDrops,				&output_dropped,
	TAG_DONE) != 0)
	{
		success = FALSE;
		goto out;
	}

	if(hardware_address_size > (LONG)sizeof(hardware_address) * 8)
		hardware_address_size = sizeof(hardware_address) * 8;

	if(Local_QueryInterfaceTags(cc,name,
		IFQ_DestinationAddress,&sin_peer,
	TAG_DONE) == 0)
	{
		have_peer_address = TRUE;
	}
	else
	{
		have_peer_address = FALSE;
	}

	type = NULL;
	for(i = 0 ; i < (LONG)NUM_ENTRIES(hardware_types) ; i++)
	{
		if(hardware_types[i].Number == hardware_type)
		{
			type = hardware_types[i].Name;
			break;
		}
	}

	if(type == NULL)
		lsprintf(cc,type = type_buffer,get_str(cc,MSG_SHOWNETSTATUS_UNKNOWN_HARDWARE_TYPE_TXT),hardware_type);

	lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_INTERFACE_TITLE_TXT),
		begin_underline(cc),
			name,
		end_underline(cc));

	lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_INTERFACE_INFO_DEVICE_NAME_TXT),device_name);
	lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_INTERFACE_INFO_DEVICE_UNIT_TXT),device_unit);
	lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_INTERFACE_INFO_HARDWARE_ADDRESS_TXT));

	if(hardware_address_size == 32)
	{
		lprintf(cc,"%s",Inet_NtoA(sin.sin_addr.s_addr));
	}
	else
	{
		for(i = 0 ; i < hardware_address_size/8 ; i++)
		{
			if(i != 0)
				lprintf(cc,":");

			lprintf(cc,"%02lx",hardware_address[i]);
		}
	}

	lprintf(cc,"\n");

	lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_INTERFACE_INFO_MTU_TXT),mtu);
	lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_INTERFACE_INFO_TRANSMISSION_SPEED_TXT),bps);
	lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_INTERFACE_INFO_HARDWARE_TYPE_TXT),type);
	lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_INTERFACE_INFO_PACKETS_SENT_TXT),sent);
	lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_INTERFACE_INFO_PACKETS_RECEIVED_TXT),received);
	lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_INTERFACE_INFO_PACKETS_DROPPED_TXT),bad,input_dropped,output_dropped);
	lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_INTERFACE_INFO_BUFFER_OVERRUNS_TXT),overrun);
	lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_INTERFACE_INFO_UNKNOWN_PACKETS_TXT),unknown);
	lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_INTERFACE_INFO_ADDRESS_TXT),(sin.sin_addr.s_addr != INADDR_ANY) ? routename(cc,sin.sin_addr.s_addr,cc->cc_Line1,MAXHOSTNAMELEN) : get_str(cc,MSG_SHOWNETSTATUS_NOT_CONFIGURED_TXT));
	lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_INTERFACE_INFO_NETWORK_MASK_TXT),(sin_mask.sin_addr.s_addr != INADDR_ANY) ? Inet_NtoA(sin_mask.sin_addr.s_addr) : get_str(cc,MSG_SHOWNETSTATUS_NOT_CONFIGURED_TXT));
	lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_INTERFACE_INFO_NUM_READ_REQUESTS_TXT),num_read_requests,max_read_requests,num_pending_read_requests);
	lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_INTERFACE_INFO_NUM_WRITE_REQUESTS_TXT),num_write_requests,max_write_requests,num_pending_write_requests);
	lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_INTERFACE_INFO_NUM_BYTES_RECEIVED_TXT),convert_quad_to_string(cc->cc_Locale,&bytes_in,byte_count,sizeof(byte_count)));
	lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_INTERFACE_INFO_NUM_BYTES_SENT_TXT),convert_quad_to_string(cc->cc_Locale,&bytes_out,byte_count,sizeof(byte_count)));
	lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_INTERFACE_INFO_TRANSFER_STATISTICS_TXT),s2cs.s2cs_DMAIn,s2cs.s2cs_DMAOut,s2cs.s2cs_ByteIn,s2cs.s2cs_ByteOut,s2cs.s2cs_WordOut);

	if(bind_type == IFABT_Static || bind_type == IFABT_Dynamic)
	{
		LONG id = (bind_type == IFABT_Static) ? MSG_SHOWNETSTATUS_INTERFACE_INFO_BINDING_STATIC_TXT : MSG_SHOWNETSTATUS_INTERFACE_INFO_BINDING_DYNAMIC_TXT;

		lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_INTERFACE_INFO_ADDRESS_BINDING_TXT),get_str(cc,id));

		if(bind_type == IFABT_Dynamic)
		{
			UBYTE buffer[2*LEN_DATSTRING+3];
			STRPTR str;

			if(lease_expire.ds_Days		== 0 &&
			   lease_expire.ds_Minute	== 0 &&
			   lease_expire.ds_Tick		== 0)
			{
				str = get_str(cc,MSG_SHOWNETSTATUS_INTERFACE_INFO_LEASE_NEVER_EXPIRES_TXT);
			}
			else
			{
				struct DateTime dat;
				UBYTE date[LEN_DATSTRING+1];
				UBYTE time[LEN_DATSTRING+1];

				memset(&dat,0,sizeof(dat));

				dat.dat_Stamp	= lease_expire;
				dat.dat_Format	= FORMAT_DEF;
				dat.dat_StrDate	= date;
				dat.dat_StrTime	= time;

				DateToStr(&dat);

				strip_extra_blank_spaces(date);
				strip_extra_blank_spaces(time);

				strcpy(buffer,date);
				strcat(buffer," ");
				strcat(buffer,time);

				str = buffer;
			}

			lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_INTERFACE_INFO_LEASE_EXPIRES_TXT),str);
		}
	}

	if(have_peer_address)
		lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_INTERFACE_INFO_PEER_ADDRESS_TXT),(sin_peer.sin_addr.s_addr != INADDR_ANY) ? routename(cc,sin_peer.sin_addr.s_addr,cc->cc_Line1,MAXHOSTNAMELEN) : get_str(cc,MSG_SHOWNETSTATUS_NOT_CONFIGURED_TXT));

	lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_INTERFACE_INFO_STATUS_TXT),get_str(cc,(state == SM_Up) ? MSG_SHOWNETSTATUS_UP_TXT : MSG_SHOWNETSTATUS_DOWN_TXT));

	success = TRUE;

 out:

	return(success);
}

/****************************************************************************/

STATIC VOID
print_icmp_status(struct CommandContext * cc,struct icmpstat *icps)
{
	if(cc->cc_Something)
		lprintf(cc,"\n");
	else
		cc->cc_Something = TRUE;

	lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_ICMP_TITLE_TXT),begin_underline(cc),end_underline(cc));

	lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_ICMP_ERRORS_GENERATED_TXT),icps->icps_error);
	lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_ICMP_IP_PACKETS_TOO_SHORT_TXT),icps->icps_oldshort);
	lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_ICMP_PACKETS_NOT_RESPONDED_TO_TXT),icps->icps_oldicmp);
	lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_ICMP_ECHO_REPLY_PACKETS_SENT_TXT),icps->icps_outhist[ICMP_ECHOREPLY]);
	lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_ICMP_DESTINATION_UNREACHABLE_TXT),icps->icps_outhist[ICMP_UNREACH]);
	lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_ICMP_SOURCE_QUENCH_TXT),icps->icps_outhist[ICMP_SOURCEQUENCH]);
	lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_ICMP_REDIRECT_TXT),icps->icps_outhist[ICMP_REDIRECT]);
	lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_ICMP_ECHO_TXT),icps->icps_outhist[ICMP_ECHO]);
	lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_ICMP_ROUTER_ADVERTIZEMENT_TXT),icps->icps_outhist[ICMP_ROUTERADVERT]);
	lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_ICMP_ROUTER_SOLICITATION_TXT),icps->icps_outhist[ICMP_ROUTERSOLICIT]);
	lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_ICMP_TIME_EXCEEDED_TXT),icps->icps_outhist[ICMP_TIMXCEED]);
	lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_ICMP_BAD_IP_HEADER_TXT),icps->icps_outhist[ICMP_PARAMPROB]);
	lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_ICMP_TIMESTAMP_REQUEST_TXT),icps->icps_outhist[ICMP_TSTAMP]);
	lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_ICMP_TIMESTAMP_REPLY_TXT),icps->icps_outhist[ICMP_TSTAMPREPLY]);
	lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_ICMP_INFORMATION_REQUEST_TXT),icps->icps_outhist[ICMP_IREQ]);
	lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_ICMP_INFORMATION_REPLY_TXT),icps->icps_outhist[ICMP_IREQREPLY]);
	lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_ICMP_ADDRESS_MASK_REQUEST_TXT),icps->icps_outhist[ICMP_MASKREQ]);
	lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_ICMP_ADDRESS_MASK_REPLY_TXT),icps->icps_outhist[ICMP_MASKREPLY]);
	lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_ICMP_RECEIVED_CODES_OUT_OF_RANGE_TXT),icps->icps_badcode);
	lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_ICMP_RECEIVED_PACKETS_TOO_SHORT_TXT),icps->icps_tooshort);
	lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_ICMP_RECEIVED_PACKET_CHECKSUM_ERRORS_TXT),icps->icps_checksum);
	lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_ICMP_RECEIVED_CODES_BOUND_MISMATCH_TXT),icps->icps_badlen);
	lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_ICMP_RESPONSES_SENT_TXT),icps->icps_reflect);
	lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_ICMP_ECHO_REPLIES_RECEIVED_TXT),icps->icps_inhist[ICMP_ECHOREPLY]);
	lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_ICMP_DESTINATION_UNREACHABLE_RECEIVED_TXT),icps->icps_inhist[ICMP_UNREACH]);
	lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_ICMP_SOURCE_QUENCH_RECEIVED_TXT),icps->icps_inhist[ICMP_SOURCEQUENCH]);
	lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_ICMP_REDIRECT_RECEIVED_TXT),icps->icps_inhist[ICMP_REDIRECT]);
	lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_ICMP_ECHO_RECEIVED_TXT),icps->icps_inhist[ICMP_ECHO]);
	lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_ICMP_ROUTER_ADVERTIZEMENT_RECEIVED_TXT),icps->icps_inhist[ICMP_ROUTERADVERT]);
	lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_ICMP_ROUTER_SOLICITATION_RECEIVED_TXT),icps->icps_inhist[ICMP_ROUTERSOLICIT]);
	lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_ICMP_TIME_EXCEEDED_RECEIVED_TXT),icps->icps_inhist[ICMP_TIMXCEED]);
	lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_ICMP_BAD_IP_HEADER_RECEIVED_TXT),icps->icps_inhist[ICMP_PARAMPROB]);
	lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_ICMP_TIMESTAMP_REQUEST_RECEIVED_TXT),icps->icps_inhist[ICMP_TSTAMP]);
	lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_ICMP_TIMESTAMP_REPLY_RECEIVED_TXT),icps->icps_inhist[ICMP_TSTAMPREPLY]);
	lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_ICMP_INFORMATION_REQUEST_RECEIVED_TXT),icps->icps_inhist[ICMP_IREQ]);
	lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_ICMP_INFORMATION_REPLY_RECEIVED_TXT),icps->icps_inhist[ICMP_IREQREPLY]);
	lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_ICMP_ADDRESS_MASK_REQUEST_RECEIVED_TXT),icps->icps_inhist[ICMP_MASKREQ]);
	lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_ICMP_ADDRESS_MASK_REPLY_RECEIVED_TXT),icps->icps_inhist[ICMP_MASKREPLY]);
}

STATIC VOID
print_igmp_status(struct CommandContext * cc,struct igmpstat *igps)
{
	if(cc->cc_Something)
		lprintf(cc,"\n");
	else
		cc->cc_Something = TRUE;

	lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_IGMP_TITLE_TXT),begin_underline(cc),end_underline(cc));

	lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_IGMP_TOTAL_MESSAGES_RECEIVED_TXT),igps->igps_rcv_total);
	lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_IGMP_MESSAGES_RECEIVED_TOO_SHORT_TXT),igps->igps_rcv_tooshort);
	lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_IGMP_MESSAGES_RECEIVED_BAD_CHECKSUM_TXT),igps->igps_rcv_badsum);
	lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_IGMP_MEMBERSHIP_QUERIES_RECEIVED_TXT),igps->igps_rcv_queries);
	lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_IGMP_INVALID_QUERIES_RECEIVED_TXT),igps->igps_rcv_badqueries);
	lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_IGMP_MEMBERSHIP_REPORTS_RECEIVED_TXT),igps->igps_rcv_reports);
	lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_IGMP_INVALID_REPORTS_RECEIVED_TXT),igps->igps_rcv_badreports);
	lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_IGMP_MEMBERSHIP_REPORTS_RECEIVED_FOR_THIS_GROUP_TXT),igps->igps_rcv_ourreports);
	lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_IGMP_MEMBERSHIP_REPORTS_SENT_TXT),igps->igps_snd_reports);
}

STATIC VOID
print_ip_status(struct CommandContext * cc,struct ipstat *ips)
{
	if(cc->cc_Something)
		lprintf(cc,"\n");
	else
		cc->cc_Something = TRUE;

	lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_IP_TITLE_TXT),begin_underline(cc),end_underline(cc));

	lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_IP_TOTAL_PACKETS_RECEIVED_TXT),ips->ips_total);
	lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_IP_PACKETS_WITH_CHECKSUM_ERRORS_TXT),ips->ips_badsum);
	lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_IP_PACKETS_TOO_SHORT_TXT),ips->ips_tooshort);
	lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_IP_PACKETS_TOO_SMALL_TXT),ips->ips_toosmall);
	lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_IP_PACKETS_WITH_BAD_DATA_SIZE_TXT),ips->ips_badhlen);
	lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_IP_PACKETS_WITH_BAD_HEADER_SIZE_TXT),ips->ips_badlen);
	lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_IP_FRAGMENTS_RECEIVED_TXT),ips->ips_fragments);
	lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_IP_FRAGMENTS_DROPPED_TXT),ips->ips_fragdropped);
	lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_IP_FRAGMENTS_TIMED_OUT_TXT),ips->ips_fragtimeout);
	lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_IP_PACKETS_FORWARDED_TXT),ips->ips_forward);
	lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_IP_PACKETS_CANNOT_FORWARD_TXT),ips->ips_cantforward);
	lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_IP_PACKETS_FORWARDED_ON_SAME_NETWORK_TXT),ips->ips_redirectsent);
	lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_IP_PACKETS_WITH_UNKNOWN_PROTOCOLS_TXT),ips->ips_noproto);
	lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_IP_DATAGRAMS_DELIVERED_TXT),ips->ips_delivered);
	lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_IP_PACKETS_GENERATED_TXT),ips->ips_localout);
	lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_IP_PACKETS_LOST_TXT),ips->ips_odropped);
	lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_IP_PACKETS_REASSEMBLED_TXT),ips->ips_reassembled);
	lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_IP_PACKETS_FRAGMENTED_TXT),ips->ips_fragmented);
	lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_IP_OUTPUT_FRAGMENTS_CREATED_TXT),ips->ips_ofragments);
	lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_IP_PACKETS_CANNOT_FRAGMENT_TXT),ips->ips_cantfrag);
	lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_IP_ERRORS_IN_OPTION_PROCESSING_TXT),ips->ips_badoptions);
	lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_IP_PACKETS_NO_ROUTE_TXT),ips->ips_noroute);
	lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_IP_PACKETS_WITH_BAD_IP_VERSION_TXT),ips->ips_badvers);
	lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_IP_RAW_PACKETS_SENT_TXT),ips->ips_rawout);
}

STATIC VOID
print_mb_status(struct CommandContext * cc,struct mbstat *m)
{
	if(cc->cc_Something)
		lprintf(cc,"\n");
	else
		cc->cc_Something = TRUE;

	lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_MBUF_TITLE_TXT),begin_underline(cc),end_underline(cc));

	lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_MBUF_BUFFERS_FROM_PAGE_POOL_TXT),m->m_mbufs);
	lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_MBUF_CLUSTERS_FROM_PAGE_POOL_TXT),m->m_clusters);
	lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_MBUF_FREE_CLUSTERS_TXT),m->m_clfree);
	lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_MBUF_DROPS_TXT),m->m_drops);
	lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_MBUF_WAIT_TXT),m->m_wait);
	lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_MBUF_DRAIN_TXT),m->m_drain);
}

STATIC VOID
print_mrt_status(struct CommandContext * cc,struct mrtstat *mrts)
{
	if(cc->cc_Something)
		lprintf(cc,"\n");
	else
		cc->cc_Something = TRUE;

	lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_MULTICAST_TITLE_TXT),begin_underline(cc),end_underline(cc));

	lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_MULTICAST_ROUTE_LOOKUPS_TXT),mrts->mrts_mrt_lookups);
	lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_MULTICAST_ROUTE_CACHE_MISSES_TXT),mrts->mrts_mrt_misses);
	lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_MULTICAST_GROUP_ADDRESS_LOOKUPS_TXT),mrts->mrts_grp_lookups);
	lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_MULTICAST_GROUP_ADDRESS_CACHE_MISSES_TXT),mrts->mrts_grp_misses);
	lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_MULTICAST_NO_ROUTE_TXT),mrts->mrts_no_route);
	lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_MULTICAST_BAD_TUNNEL_TXT),mrts->mrts_bad_tunnel);
	lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_MULTICAST_CANNOT_TUNNEL_TXT),mrts->mrts_cant_tunnel);
	lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_MULTICAST_CANNOT_WRONG_INTERFACE_TXT),mrts->mrts_wrong_if);
}

STATIC VOID
print_rt_status(struct CommandContext * cc,struct rtstat *rts)
{
	if(cc->cc_Something)
		lprintf(cc,"\n");
	else
		cc->cc_Something = TRUE;

	lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_ROUTING_TITLE_TXT),begin_underline(cc),end_underline(cc));

	lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_ROUTING_BAD_REDIRECT_TXT),rts->rts_badredirect);
	lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_ROUTING_DYNAMIC_TXT),rts->rts_dynamic);
	lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_ROUTING_NEW_GATEWAY_TXT),rts->rts_newgateway);
	lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_ROUTING_UNREACH_TXT),rts->rts_unreach);
	lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_ROUTING_WILDCARD_TXT),rts->rts_wildcard);
}

STATIC VOID
print_tcp_status(struct CommandContext * cc,struct tcpstat *tcps)
{
	if(cc->cc_Something)
		lprintf(cc,"\n");
	else
		cc->cc_Something = TRUE;

	lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_TCP_TITLE_TXT),begin_underline(cc),end_underline(cc));

	lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_TCP_CONNATTEMPT_TXT),tcps->tcps_connattempt);
	lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_TCP_ACCEPTS_TXT),tcps->tcps_accepts);
	lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_TCP_CONNECTS_TXT),tcps->tcps_connects);
	lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_TCP_DROPS_TXT),tcps->tcps_drops);
	lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_TCP_CONNDROPS_TXT),tcps->tcps_conndrops);
	lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_TCP_CLOSED_TXT),tcps->tcps_closed);
	lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_TCP_SEGSTIMED_TXT),tcps->tcps_segstimed);
	lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_TCP_RTTUPDATED_TXT),tcps->tcps_rttupdated);
	lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_TCP_DELACK_TXT),tcps->tcps_delack);
	lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_TCP_TIMEOUTDROP_TXT),tcps->tcps_timeoutdrop);
	lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_TCP_PERSISTTIMEO_TXT),tcps->tcps_persisttimeo);
	lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_TCP_KEEPTIMEO_TXT),tcps->tcps_keeptimeo);
	lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_TCP_KEEPPROBE_TXT),tcps->tcps_keepprobe);
	lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_TCP_KEEPDROPS_TXT),tcps->tcps_keepdrops);

	lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_TCP_SNDTOTAL_TXT),tcps->tcps_sndtotal);
	lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_TCP_SNDPACK_TXT),tcps->tcps_sndpack);
	lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_TCP_SNDBYTE_TXT),tcps->tcps_sndbyte);
	lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_TCP_SNDREXMITPACK_TXT),tcps->tcps_sndrexmitpack);
	lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_TCP_SNDREXMITBYTE_TXT),tcps->tcps_sndrexmitbyte);
	lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_TCP_SNDACKS_TXT),tcps->tcps_sndacks);
	lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_TCP_SNDPROBE_TXT),tcps->tcps_sndprobe);
	lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_TCP_SNDURG_TXT),tcps->tcps_sndurg);
	lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_TCP_SNDWINUP_TXT),tcps->tcps_sndwinup);
	lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_TCP_SNDCTRL_TXT),tcps->tcps_sndctrl);

	lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_TCP_RCVTOTAL_TXT),tcps->tcps_rcvtotal);
	lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_TCP_RCVPACK_TXT),tcps->tcps_rcvpack);
	lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_TCP_RCVBYTE_TXT),tcps->tcps_rcvbyte);
	lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_TCP_RCVBADSUM_TXT),tcps->tcps_rcvbadsum);
	lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_TCP_RCVBADOFF_TXT),tcps->tcps_rcvbadoff);
	lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_TCP_RCVSHORT_TXT),tcps->tcps_rcvshort);
	lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_TCP_RCVDUPPACK_TXT),tcps->tcps_rcvduppack);
	lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_TCP_RCVDUPBYTE_TXT),tcps->tcps_rcvdupbyte);
	lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_TCP_RCVOOPACK_TXT),tcps->tcps_rcvoopack);
	lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_TCP_RCVOOBYTE_TXT),tcps->tcps_rcvoobyte);
	lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_TCP_RCVPACKAFTERWIN_TXT),tcps->tcps_rcvpackafterwin);
	lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_TCP_RCVBYTEAFTERWIN_TXT),tcps->tcps_rcvbyteafterwin);
	lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_TCP_RCVAFTERCLOSE_TXT),tcps->tcps_rcvafterclose);
	lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_TCP_RCVWINPROBE_TXT),tcps->tcps_rcvwinprobe);
	lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_TCP_RCVDUPACK_TXT),tcps->tcps_rcvdupack);
	lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_TCP_RCVACKTOOMUCH_TXT),tcps->tcps_rcvacktoomuch);
	lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_TCP_RCVACKPACK_TXT),tcps->tcps_rcvackpack);
	lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_TCP_RCVACKBYTE_TXT),tcps->tcps_rcvackbyte);
	lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_TCP_RCVWINUPD_TXT),tcps->tcps_rcvwinupd);
	lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_TCP_PAWSDROP_TXT),tcps->tcps_pawsdrop);
	lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_TCP_PREDACK_TXT),tcps->tcps_predack);
	lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_TCP_PREDDAT_TXT),tcps->tcps_preddat);
	lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_TCP_PERSISTDROP_TXT),tcps->tcps_persistdrop);
	lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_TCP_BADSYN_TXT),tcps->tcps_badsyn);
}

STATIC VOID
print_udp_status(struct CommandContext * cc,struct udpstat *udps)
{
	if(cc->cc_Something)
		lprintf(cc,"\n");
	else
		cc->cc_Something = TRUE;

	lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_UDP_TITLE_TXT),begin_underline(cc),end_underline(cc));

	lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_UDP_IPACKETS_TXT),udps->udps_ipackets);
	lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_UDP_HDROPS_TXT),udps->udps_hdrops);
	lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_UDP_BADSUM_TXT),udps->udps_badsum);
	lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_UDP_BADLEN_TXT),udps->udps_badlen);
	lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_UDP_NOPORT_TXT),udps->udps_noport);
	lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_UDP_NOPORTBCAST_TXT),udps->udps_noportbcast);
	lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_UDP_FULLSOCK_TXT),udps->udps_fullsock);
	lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_UDP_OPACKETS_TXT),udps->udps_opackets);
}

STATIC VOID
print_tcp_socket_status(struct CommandContext * cc,BOOL all_sockets,struct protocol_connection_data *pcd,LONG size)
{
	DECLARE_SOCKETBASE(cc);

	struct servent * s;
	STRPTR status;
	LONG i,m,n;

	if(cc->cc_Something)
		lprintf(cc,"\n");
	else
		cc->cc_Something = TRUE;

	if(size >= (int)sizeof(*pcd))
	{
		lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_TCP_SOCKET_TITLE_TXT),
			begin_underline(cc),
				get_str(cc,MSG_SHOWNETSTATUS_LOCAL_TXT),
				get_str(cc,MSG_SHOWNETSTATUS_REMOTE_TXT),
				get_str(cc,MSG_SHOWNETSTATUS_RECEIVE_LEN_TXT),
				get_str(cc,MSG_SHOWNETSTATUS_SEND_LEN_TXT),
				get_str(cc,MSG_SHOWNETSTATUS_STATUS_TXT),
			end_underline(cc));
	}

	for(i = m = n = 0 ; i < size / (int)sizeof(*pcd) ; i++, pcd++, n++)
	{
		if(pcd->pcd_local_address.s_addr == INADDR_ANY && NOT all_sockets)
		{
			m++;
			continue;
		}

		if(pcd->pcd_local_address.s_addr != INADDR_ANY || cc->cc_NoNames)
			routename(cc,pcd->pcd_local_address.s_addr,cc->cc_Line1,MAXHOSTNAMELEN);
		else
			strcpy(cc->cc_Line1,get_str(cc,MSG_SHOWNETSTATUS_WILDCARD_TXT));

		if(cc->cc_NoNames)
			s = NULL;
		else
			s = getservbyport(pcd->pcd_local_port,"tcp");

		if(s != NULL)
			lsprintf(cc,&cc->cc_Line1[strlen(cc->cc_Line1)],":%s",s->s_name);
		else
			lsprintf(cc,&cc->cc_Line1[strlen(cc->cc_Line1)],":%ld",pcd->pcd_local_port);

		routename(cc,pcd->pcd_foreign_address.s_addr,cc->cc_Line2,MAXHOSTNAMELEN);

		if(pcd->pcd_foreign_address.s_addr != INADDR_ANY)
		{
			if(cc->cc_NoNames)
				s = NULL;
			else
				s = getservbyport(pcd->pcd_foreign_port,"tcp");

			if(s != NULL)
				lsprintf(cc,&cc->cc_Line2[strlen(cc->cc_Line2)],":%s",s->s_name);
			else
				lsprintf(cc,&cc->cc_Line2[strlen(cc->cc_Line2)],":%ld",pcd->pcd_foreign_port);
		}
		else
		{
			strcpy(cc->cc_Line2,"-");
		}

		if(TCPS_CLOSED <= pcd->pcd_tcp_state && pcd->pcd_tcp_state <= TCPS_TIME_WAIT)
			status = get_str(cc,MSG_SHOWNETSTATUS_TCP_FSM_STATE1_TXT + pcd->pcd_tcp_state - TCPS_CLOSED);
		else
			status = "-";

		lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_TCP_SOCKET_LINE_TXT),cc->cc_Line1,cc->cc_Line2,pcd->pcd_receive_queue_size,pcd->pcd_send_queue_size,status);
	}

	if(n == 0)
	{
		if(m == 0)
			lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_NO_TCP_SOCKETS_IN_USE_TXT));
		else
			lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_SOME_TCP_SOCKETS_IN_USE_TXT),m);
	}
}

STATIC VOID
print_udp_socket_status(struct CommandContext * cc,BOOL all_sockets,struct protocol_connection_data *pcd,LONG size)
{
	DECLARE_SOCKETBASE(cc);

	struct servent * s;
	LONG i,m,n;

	if(cc->cc_Something)
		lprintf(cc,"\n");
	else
		cc->cc_Something = TRUE;

	if(size >= (int)sizeof(*pcd))
	{
		lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_UDP_SOCKET_TITLE_TXT),
			begin_underline(cc),
				get_str(cc,MSG_SHOWNETSTATUS_LOCAL_TXT),
				get_str(cc,MSG_SHOWNETSTATUS_REMOTE_TXT),
				get_str(cc,MSG_SHOWNETSTATUS_RECEIVE_LEN_TXT),
				get_str(cc,MSG_SHOWNETSTATUS_SEND_LEN_TXT),
			end_underline(cc));
	}

	for(i = m = n = 0 ; i < size / (int)sizeof(*pcd) ; i++, pcd++, n++)
	{
		if(pcd->pcd_local_address.s_addr == INADDR_ANY && NOT all_sockets)
		{
			m++;
			continue;
		}

		if(pcd->pcd_local_address.s_addr != INADDR_ANY || cc->cc_NoNames)
			routename(cc,pcd->pcd_local_address.s_addr,cc->cc_Line1,MAXHOSTNAMELEN);
		else
			strcpy(cc->cc_Line1,get_str(cc,MSG_SHOWNETSTATUS_WILDCARD_TXT));

		if(cc->cc_NoNames)
			s = NULL;
		else
			s = getservbyport(pcd->pcd_local_port,"udp");

		if(s != NULL)
			lsprintf(cc,&cc->cc_Line1[strlen(cc->cc_Line1)],":%s",s->s_name);
		else
			lsprintf(cc,&cc->cc_Line1[strlen(cc->cc_Line1)],":%ld",pcd->pcd_local_port);

		if(pcd->pcd_foreign_address.s_addr != INADDR_ANY)
		{
			routename(cc,pcd->pcd_foreign_address.s_addr,cc->cc_Line2,MAXHOSTNAMELEN);

			if(cc->cc_NoNames)
				s = NULL;
			else
				s = getservbyport(pcd->pcd_foreign_port,"udp");

			if(s != NULL)
				lsprintf(cc,&cc->cc_Line2[strlen(cc->cc_Line2)],":%s",s->s_name);
			else
				lsprintf(cc,&cc->cc_Line2[strlen(cc->cc_Line2)],":%ld",pcd->pcd_foreign_port);
		}
		else
		{
			strcpy(cc->cc_Line2,"-");
		}

		lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_UDP_SOCKET_LINE_TXT),cc->cc_Line1,cc->cc_Line2,pcd->pcd_receive_queue_size,pcd->pcd_send_queue_size);
	}

	if(n == 0)
	{
		if(m == 0)
			lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_NO_UDP_SOCKETS_IN_USE_TXT));
		else
			lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_SOME_UDP_SOCKETS_IN_USE_TXT),m);
	}
}

/****************************************************************************/

STATIC VOID
print_net_status_summary(struct CommandContext * cc)
{
	DECLARE_SOCKETBASE(cc);

	struct List * interface_list;
	struct List * dns_list;
	struct rt_msghdr * route_table;
	struct sockaddr_in * default_route;
	char * default_interface_name;
	struct sockaddr_in default_interface_address;

	lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_STATUS_SUMMARY_TXT),begin_underline(cc),end_underline(cc));

	default_interface_name = NULL;
	memset(&default_interface_address,0,sizeof(default_interface_address));

	interface_list = ObtainInterfaceList();
	if(interface_list != NULL && NOT IsListEmpty(interface_list))
	{
		struct Node * node;
		ULONG host_id;

		host_id = gethostid();

		for(node = interface_list->lh_Head ;
		    node->ln_Succ != NULL ;
		    node = node->ln_Succ)
		{
			if(Local_QueryInterfaceTags(cc,node->ln_Name,
				IFQ_Address,&default_interface_address,
			TAG_DONE) != 0)
			{
				continue;
			}

			if(default_interface_address.sin_addr.s_addr == host_id)
			{
				default_interface_name = node->ln_Name;
				break;
			}
		}
	}

	lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_LOCAL_HOST_ADDRESS_TXT));

	if(default_interface_name != NULL)
		lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_LOCAL_HOST_ADDRESS_ON_INTERFACE_TXT),Inet_NtoA(default_interface_address.sin_addr.s_addr),default_interface_name);
	else
		lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_NOT_CONFIGURED_TXT));

	lprintf(cc,"\n");

	default_route = NULL;

	route_table = GetRouteInfo(AF_UNSPEC,0);
	if(route_table != NULL)
	{
		struct sockaddr_in * sin;
		struct rt_msghdr * rtm;
		struct sockaddr * sa;
		LONG len;

		for(rtm = route_table ;
			rtm->rtm_msglen > 0 ;
			rtm = (struct rt_msghdr *)(((ULONG)rtm) + rtm->rtm_msglen))
		{
			/* We need a route with a destination and a gateway address. */
			if(rtm->rtm_version != 3 || (rtm->rtm_addrs & RTA_DST) == 0 || (rtm->rtm_addrs & RTA_GATEWAY) == 0)
				continue;

			/* The destination address follows the message header. */
			sa = (struct sockaddr *)(rtm + 1);

			/* The destination should be an Internet address */
			if(sa->sa_family != AF_INET)
				continue;

			/* The default route is identified by having a 0.0.0.0 destination
			   address, which indicates a route that matches any destination
			   address. */
			sin = (struct sockaddr_in *)sa;
			if(sin->sin_addr.s_addr != INADDR_ANY)
				continue;

			/* Skip the destination address. */
			if(sa->sa_len == 0)
				len = sizeof(long);
			else
				len = sa->sa_len;

			sa = (struct sockaddr *)(((ULONG)sa) + len);

			/* This should be the IP address of the gateway. It should be
			   an Internet address. */
			if(sa->sa_family != AF_INET)
				continue;

			default_route = (struct sockaddr_in *)sa;
			break;
		}
	}

	lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_DEFAULT_GATEWAY_ADDRESS_TXT));

	if(default_route != NULL)
		lprintf(cc,"%s",Inet_NtoA(default_route->sin_addr.s_addr));
	else
		lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_NOT_CONFIGURED_TXT));

	lprintf(cc,"\n");

	lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_DOMAIN_NAME_SYSTEM_SERVERS_TXT));

	dns_list = ObtainDomainNameServerList();
	if(dns_list != NULL && NOT IsListEmpty(dns_list))
	{
		struct DomainNameServerNode * dnsn;
		BOOL is_first = TRUE;

		for(dnsn = (struct DomainNameServerNode *)dns_list->lh_Head ;
		    dnsn->dnsn_MinNode.mln_Succ != NULL ;
		    dnsn = (struct DomainNameServerNode *)dnsn->dnsn_MinNode.mln_Succ)
		{
			if(NOT is_first)
				lprintf(cc,", ");

			lprintf(cc,"%s",dnsn->dnsn_Address);

			is_first = FALSE;
		}
	}
	else
	{
		lprintf(cc,get_str(cc,MSG_SHOWNETSTATUS_NOT_CONFIGURED_TXT));
	}

	lprintf(cc,"\n");

	if(interface_list != NULL)
		ReleaseInterfaceList(interface_list);

	if(dns_list != NULL)
		ReleaseDomainNameServerList(dns_list);

	if(route_table != NULL)
		FreeRouteInfo(route_table);
}

/****************************************************************************/

STATIC LONG VARARGS68K
Local_ErrorPrintf(struct CommandContext * cc,STRPTR format,...)
{
	DECLARE_DOSBASE(cc);
	DECLARE_SYSBASE(cc);

	va_list args;
	LONG result;
	BPTR fh;

	#if defined(__amigaos4__)
	{
		fh = ErrorOutput();
	}
	#else
	{
		struct Process * this_process = (struct Process *)FindTask(NULL);

		fh = this_process->pr_CES;
	}
	#endif /* __amigaos4__ */

	if(fh == ZERO)
		fh = Output();

	#if defined(__amigaos4__)
	{
		va_startlinear(args,format);
		result = VFPrintf(fh,format,va_getlinearva(args,APTR));
		va_end(args);
	}
	#else
	{
		va_start(args,format);
		result = VFPrintf(fh,format,args);
		va_end(args);
	}
	#endif /* __amigaos4__ */

	return(result);
}

/****************************************************************************/

/* This looks up a locale string ID in the builtin database; adapted
   from CygnusEd because I couldn't find my own implementation for this
   application... */
STATIC STRPTR
get_builtin_str(LONG id)
{
	LONG top,middle,bottom;
	STRPTR builtin_string;

	/* The search area is all those message between bottom and top, inclusive. */
	bottom = 0;
	top = NUM_ENTRIES(CatCompArray) - 1;

	/* Binary search through the CatCompArray to find the requested string.
	   Note that this assumes that the strings are sorted. Catcomp defaults
	   to creating sorted arrays, but it does _not_ force it. If in the .cd
	   file you specify out of order string numbers this routine will fail. */
	while(bottom != top)
	{
		middle = (bottom + top) / 2;

		if(CatCompArray[middle].cca_ID >= id)
			top = middle;
		else
			bottom = middle + 1;
	}

	/* The only time this error message should occur is if you've passed
	   a garbage number OR if the CatCompArray is not sorted. */
	if(CatCompArray[bottom].cca_ID == id)
		builtin_string = (STRPTR)CatCompArray[bottom].cca_Str;
	else
		builtin_string = "";

	return(builtin_string);
}

STRPTR
get_str(struct CommandContext * cc, LONG id)
{
	STRPTR builtin_string;
	STRPTR result;

	builtin_string = get_builtin_str(id);

	if(cc->cc_Catalog != NULL)
	{
		DECLARE_LOCALEBASE(cc);

		result = (STRPTR)GetCatalogStr(cc->cc_Catalog,id,builtin_string);
	}
	else
	{
		result = builtin_string;
	}

	return(result);
}

/****************************************************************************/

STATIC VARARGS68K LONG
Local_QueryInterfaceTags(struct CommandContext * cc,STRPTR interface_name,...)
{
	DECLARE_SOCKETBASE(cc);

	va_list args;
	LONG result;

	#if defined(__amigaos4__)
	{
		va_startlinear(args,interface_name);
		result = QueryInterfaceTagList(interface_name,va_getlinearva(args,struct TagItem *));
		va_end(args);
	}
	#else
	{
		va_start(args,interface_name);
		result = QueryInterfaceTagList(interface_name,(struct TagItem *)args);
		va_end(args);
	}
	#endif /* __amigaos4__ */

	return(result);
}

STATIC VARARGS68K LONG
Local_SocketBaseTags(struct CommandContext * cc,...)
{
	DECLARE_SOCKETBASE(cc);

	va_list args;
	LONG result;

	#if defined(__amigaos4__)
	{
		va_startlinear(args,cc);
		result = SocketBaseTagList(va_getlinearva(args,struct TagItem *));
		va_end(args);
	}
	#else
	{
		va_start(args,cc);
		result = SocketBaseTagList((struct TagItem *)args);
		va_end(args);
	}
	#endif /* __amigaos4__ */

	return(result);
}
