/*
 *	$Id: ConfigureNetInterface.c,v 1.17 2007-08-26 12:30:15 obarthel Exp $
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

#include <libraries/locale.h>

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

#if defined(__amigaos4__) && !defined(Flush)
#define Flush(fh) FFlush(fh)
#endif /* __amigaos4__ */

/****************************************************************************/

#include <string.h>
#include <stdarg.h>

/****************************************************************************/

#include "macros.h"

/****************************************************************************/

#include "ConfigureNetInterface_rev.h"

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
	struct Library *		cc_LocaleBase;
	struct Library *		cc_SocketBase;

	#if defined(__amigaos4__)

	/************************************************************************/

	struct ExecIFace *		cc_IExec;
	struct DOSIFace *		cc_IDOS;
	struct UtilityIFace *	cc_IUtility;
	struct LocaleIFace *	cc_ILocale;
	struct SocketIFace *	cc_ISocket;

	/************************************************************************/

	#endif /* __amigaos4__ */

	struct Catalog *		cc_Catalog;
	UBYTE					cc_ProgramName[256];
	struct TagItem *		cc_Tags;
	LONG					cc_NumTags;
	LONG					cc_MaxTags;
	struct MsgPort *		cc_ReplyPort;
	struct AddressAllocationMessage
							cc_AllocationMessage;
	ULONG					cc_RouterTable[16];
	ULONG					cc_DNSTable[16];
	struct DateStamp		cc_LeaseExpires;
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
STATIC LONG cmd(struct CommandContext *cc);
STATIC BOOL validate_ip_address(struct CommandContext *cc, BOOL quiet, STRPTR key, STRPTR address);
STATIC LONG add_tag(struct CommandContext *cc, Tag tag, ULONG data);
STATIC VOID get_errno_and_code(struct CommandContext *cc, LONG *errno_ptr, STRPTR *code_ptr);
STATIC BOOL substring_matches(struct CommandContext *cc, STRPTR pattern, STRPTR string);
STATIC BOOL is_blank_space(UBYTE c);
STATIC VOID strip_extra_blank_spaces(STRPTR s);
STATIC LONG VARARGS68K Local_Printf(struct CommandContext *cc, STRPTR format, ...);
STATIC VOID Local_PrintFault(struct CommandContext *cc, LONG error, STRPTR prefix);
STATIC LONG VARARGS68K Local_ErrorPrintf(struct CommandContext *cc, STRPTR format, ...);
STATIC STRPTR get_builtin_str(LONG id);
STATIC STRPTR get_str(struct CommandContext *cc, LONG id);
STATIC LONG VARARGS68K Local_AddRouteTags(struct CommandContext *cc, ...);
STATIC LONG VARARGS68K Local_ConfigureInterfaceTags(struct CommandContext *cc, STRPTR interface_name, ...);
STATIC LONG VARARGS68K Local_SocketBaseTags(struct CommandContext *cc, ...);

/****************************************************************************/

/****** ROADSHOW/CONFIGURENETINTERFACE ****************************************
*
*   NAME
*	ConfigureNetInterface - Configure network interface parameters.
*
*   FORMAT
*	ConfigureNetInterface [QUIET] [TIMEOUT=<n>] INTERFACE
*
*   TEMPLATE
*	INTERFACE/A,QUIET/S,ADDRESS/K,NETMASK/K,BROADCASTADDR/K,
*	DESTINATION=DESTINATIONADDR/K,METRIC/K/N,MTU/K/N,ALIASADDR/K,
*	DELETEADDR/K,ONLINE/S,OFFLINE/S,UP/S,DOWN/S,DEBUG/K,COMPLETE/K,
*	CONFIGURE/K,LEASE/K,RELEASE=RELEASEADDRESS/S,ID/K,TIMEOUT/K/N
*
*   PATH
*	C:CONFIGURENETINTERFACE
*
*   FUNCTION
*	CONFIGURENETINTERFACE is used to define how a network interface will
*	react and how it will interact with your network.
*
*   OPTIONS
*	INTERFACE/A
*	    The name of the interface to be configured. This is a required
*	    parameter.
*
*	QUIET/S
*	    This option causes the program not to emit any error messages
*	    or progress reports. Also, if the program encounters an error
*	    it will flag this as failure code 5 which can be looked at
*	    using the 'if warn' shell script command. If this option is
*	    not in effect, failure codes will be more severe and all sorts
*	    of progress information will be displayed.
*
*	ADDRESS/K
*	    The IP address to assign to this interface. This should be
*	    specified in dotted-decimal notation ("192.168.0.1") and not as
*	    symbolic name since the system may not be in a state to perform a
*	    name resolution.
*
*	    In place of the IP address you can also specify "DHCP". As the
*	    name suggests, this will start a configuration process involving
*	    the DHCP protocol which should eventually yield the right IP
*	    address for this host. Note that this configuration procedure only
*	    works for Ethernet hardware.
*
*	NETMASK/K
*	    The subnet mask to assign to this interface. This must be
*	    specified in dotted-decimal notation ("192.168.0.1").
*
*	    In place of the subnet mask you can also specify "DHCP". As the
*	    name suggests, this will start a configuration process involving
*	    the DHCP protocol which should eventually yield the right
*	    subnet mask for this host. Note that this configuration procedure
*	    only works for Ethernet hardware.
*
*	BROADCASTADDR/K
*	    The broadcast address to be used by this interface; must be
*	    specified in dotted-decimal notation ("192.168.0.1") and only
*	    works with interfaces that support broadcasts in the first place
*	    (i.e. Ethernet hardware).
*
*	DESTINATION=DESTINATIONADDR/K
*	    The address of the point-to-point partner for this interface; must
*	    be specified in dotted-decimal notation ("192.168.0.1"). Only
*	    works for point-to-point connections, such as PPP.
*
*	METRIC/K/N
*	    Route metric value for this interface.
*
*	MTU/K/N
*	    You can limit the maximum transmission size used by the TCP/IP
*	    stack to push data through the interface. The interface driver
*	    will have its own ideas about the maximum transmission size.
*	    You can therefore only suggest a smaller value than the
*	    driver's preferred hardware MTU size.
*
*	ALIASADDR/K
*	    This adds another address to this interface to respond to. You
*	    can add as many aliases as you like, provided you don't run out
*	    of memory.
*
*	DELETEADDR/K
*	    This removes an alias address from the list the interface is to
*	    respond to.
*
*	UP
*	DOWN
*	ONLINE
*	OFFLINE
*	    This configures the 'line state' of the interface; four states
*	    are supported:
*
*	    UP
*	        The protocol stack will attempt to transmit messages
*	        through this interface (even though it might not be
*	        online yet).
*
*	    DOWN
*	        The protocol stack will no longer attempt to transmit
*	        messages through this interface (even though it might
*	        still be online).
*
*	    OFFLINE
*	        The underlying networking device driver is put offline
*	        and the protocol stack will no longer try to send
*	        messages through the interface either.
*
*	    ONLINE
*	        An attempt is made to put the underlying networking
*	        driver online. If that works, then the protocol stack
*	        will attempt to transmit messages through this
*	        interface.
*
*	DEBUG/K  (possible parameters: YES or NO)
*	    You can enable debug output for this interface to help in tracking
*	    down configuration problems. At this time of writing, the debug
*	    mode will, if enabled, produce information on the progress of the
*	    DHCP configuration process.
*
*	COMPLETE/K  (possible parameters: YES or NO)
*	    If you configure an interface in several steps, use this parameter
*	    in the final invocation of the program. It will tell the TCP/IP
*	    stack that the configuration for this interface is complete. This
*	    has the effect of causing the static route definition file to be
*	    reread, if necessary.
*
*	RELEASEADDRESS
*	    If an IP address was dynamically assigned to an interface, this
*	    switch will tell ConfigureNetInterface to release it. Note that
*	    you can only release what was previously allocated.
*
*	CONFIGURE/K  (possible parameters: DHCP, AUTO or FASTAUTO)
*	    You can use DHCP configuration for this interface and protocol
*	    stack internals, namely the list of routers (and the default
*	    gateway) to use and the domain name servers. This option allows
*	    you to bring up the complete network configuration in one
*	    single step.
*
*	    You can request that a particular IP address is assigned to this
*	    interface by the DHCP process by specifying CONFIGURE=DHCP and
*	    your choice of ADDRESS=xxx.xxx.xxx.xxx.
*
*	    If your network has no DHCP server, you may choose
*	    CONFIGURE=AUTO to use automatic IPv4 address selection,
*	    based upon a protocol called ZeroConf. This protocol will
*	    select a currently unused address from a specially
*	    designated address range.
*
*	    If you choose automatic configuration in a wireless network,
*	    you might want to use CONFIGURE=FASTAUTO instead of
*	    CONFIGURE=AUTO.
*
*	    Note that only the CONFIGURE=DHCP option will attempt to
*	    set up a default route and a set of DNS servers for you to
*	    use. The alternatives of CONFIGURE=FASTAUTO and
*	    CONFIGURE=AUTO are restricted to selecting the network
*	    interface IPv4 addresses.
*
*	TIMEOUT/K/N
*	    If you're going to use DHCP configuration for any of the
*	    interfaces, a default timeout value of 60 seconds will
*	    limit the time an interface can take to be configured.
*	    This parameter allows you to use a different timeout value.
*	    Note that due to how the configuration protocol works,
*	    the timeout cannot be shorter than ten seconds.
*
*	LEASE/K
*	    This is a complex option which can be used to request how long an
*	    IP address should be bound to an interface. Several combinations
*	    of options are possible. Here is a short list:
*
*	        LEASE=300
*	        LEASE=300seconds
*
*	            This requests a lease of exactly 300 seconds, or
*	            five minutes.
*
*	        LEASE=30min
*
*	            This requests a lease of 30 minutes.
*
*	        LEASE=2hours
*
*	            This requests a lease of 2 hours.
*
*	        LEASE=1day
*
*	            This requests a lease of 1 day.
*
*	        LEASE=4weeks
*
*	            This requests a lease of 4 weeks.
*
*	        LEASE=infinite
*
*	            This requests that the IP address should be
*	            permanently bound.
*
*	    Blank spaces between the numbers and the qualifiers are supported.
*	    The qualifiers are tested using substring matching, which means
*	    for example that "30 minutes" is the same as "30 min" and "30 m".
*
*	    Note that the requested lease time may be ignored by the DHCP
*	    server. After all, it is just a suggestion and not an order.
*
*	ID/K
*	    This option works along with the CONFIGURE=DHCP process. It can be
*	    used to tell the DHCP server by which name the local host should be
*	    referred to. Some DHCP servers are on good terms with their local name
*	    resolution services and will add the name and the associated IP
*	    address to the local host database. The name you can supply here
*	    cannot be longer than 255 characters and must be at least 2 characters
*	    long. Keep it brief: not all DHCP servers have room for the whole 255
*	    characters.
*
*   NOTES
*	The command is similar to the Unix "ifconfig" command.
*
*	If you tell an interface to go online then the program's return
*	code will tell you if the command succeeded: a return value of 0
*	indicates success (the interface is now online), and a value
*	of 5 indicates that it didn't quite work.
*
*	Configuring the address of an interface has two effects: first,
*	the interface will be marked as 'up', meaning that the protocol
*	stack will attempt to send messages through it when appropriate.
*	Second, a direct route to the interface will be established.
*
*   SEE ALSO
*	AddNetInterface
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
			cc->cc_Catalog = OpenCatalogA(NULL,"roadshow.catalog",NULL);

		result = cmd(cc);

		if(cc->cc_Tags != NULL)
			FreeVec(cc->cc_Tags);

		if(cc->cc_Catalog != NULL)
			CloseCatalog(cc->cc_Catalog);
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
#define CONFIGURENETINTERFACE_CATALOG_STRINGS
#include "roadshow.h"

/****************************************************************************/

STATIC LONG
cmd(struct CommandContext * cc)
{
	struct
	{
		KEY		Interface;
		SWITCH	Quiet;
		KEY		Address;
		KEY		NetMask;
		KEY		BroadcastAddress;
		KEY		DestinationAddress;
		NUMBER	Metric;
		NUMBER	MTU;
		KEY		AddAddress;
		KEY		DeleteAddress;
		SWITCH	Online;
		SWITCH	Offline;
		SWITCH	Up;
		SWITCH	Down;
		KEY		Debug;
		KEY		Complete;
		KEY		Configure;
		KEY		Lease;
		SWITCH	ReleaseAddress;
		KEY		ID;
		NUMBER	Timeout;
	} args;

	STRPTR args_template =
		"INTERFACE/A,"
		"QUIET/S,"
		"ADDRESS/K,"
		"NETMASK/K,"
		"BROADCASTADDR/K,"
		"DESTINATION=DESTINATIONADDR/K,"
		"METRIC/K/N,"
		"MTU/K/N,"
		"ALIASADDR/K,"
		"DELETEADDR/K,"
		"ONLINE/S,"
		"OFFLINE/S,"
		"UP/S,"
		"DOWN/S,"
		"DEBUG/K,"
		"COMPLETE/K,"
		"CONFIGURE/K,"
		"LEASE/K,"
		"RELEASE=RELEASEADDRESS/S,"
		"ID/K,"
		"TIMEOUT/K/N"
		VERSTAG;

	DECLARE_SYSBASE(cc);
	DECLARE_DOSBASE(cc);
	DECLARE_UTILITYBASE(cc);
	DECLARE_SOCKETBASE(cc);

	struct AddressAllocationMessage * aam = &cc->cc_AllocationMessage;
	LONG result = RETURN_FAIL;
	LONG have_interface_api = FALSE;
	LONG have_routing_api = FALSE;
	LONG have_address_conversion_api = FALSE;
	struct RDArgs * rda;
	STRPTR state_name = NULL;
	int state = -1;
	LONG error;
	BOOL quiet = FALSE;
	STRPTR address = NULL;
	STRPTR net_mask = NULL;
	BOOL configure_dynamic = FALSE;
	BOOL configure_auto = FALSE;
	BOOL configure_slow_auto = FALSE;
	LONG timeout;
	UBYTE default_domain_name[256];
	ULONG lease_time = DHCP_DEFAULT_LEASE_TIME;
	STRPTR client_id = NULL;

	GetProgramName(cc->cc_ProgramName,sizeof(cc->cc_ProgramName));

	memset(&args,0,sizeof(args));

	rda = ReadArgs(args_template,(LONG *)&args,NULL);
	if(rda == NULL)
	{
		PrintFault(IoErr(),cc->cc_ProgramName);
		goto out;
	}

	quiet = (BOOL)(args.Quiet != 0);

	if(UtilityBase == NULL)
	{
		if(NOT quiet)
		{
			Local_ErrorPrintf(cc,get_str(cc,MSG_CONFIGURENETINTERFACE_NO_UTILITY_TXT),
				cc->cc_ProgramName);
		}

		goto out;
	}

	if(SocketBase == NULL)
	{
		if(NOT quiet)
		{
			Local_ErrorPrintf(cc,get_str(cc,MSG_CONFIGURENETINTERFACE_CANNOT_OPEN_BSDSOCKET_TXT),
				cc->cc_ProgramName);
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
		if(NOT quiet)
		{
			Local_ErrorPrintf(cc,get_str(cc,MSG_CONFIGURENETINTERFACE_BSDSOCKET_HAS_NO_INTERFACE_API_TXT),
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
		if(NOT quiet)
		{
			Local_ErrorPrintf(cc,get_str(cc,MSG_CONFIGURENETINTERFACE_BSDSOCKET_HAS_NO_ROUTE_API_TXT),
				cc->cc_ProgramName, SocketBase->lib_Node.ln_Name, SocketBase->lib_Version, SocketBase->lib_Revision);
		}

		goto out;
	}

	if(Local_SocketBaseTags(cc,
	    SBTM_GETREF(SBTC_HAVE_ADDRESS_CONVERSION_API),&have_address_conversion_api,
	TAG_END) != 0)
	{
		have_address_conversion_api = FALSE;
	}

	if(NOT have_address_conversion_api)
	{
		if(NOT quiet)
		{
			Local_ErrorPrintf(cc,get_str(cc,MSG_CONFIGURENETINTERFACE_HAS_NO_ADDRESS_CONVERSION_API_TXT),
				cc->cc_ProgramName, SocketBase->lib_Node.ln_Name, SocketBase->lib_Version, SocketBase->lib_Revision);
		}

		goto out;
	}

	if(args.Address != NULL)
	{
		struct in_addr in;

		if(Stricmp(args.Address,"DHCP") != SAME)
		{
			if(CANNOT validate_ip_address(cc,quiet,"ADDRESS",args.Address))
				goto out;
		}

		if(inet_aton(args.Address,&in))
		{
			if(in.s_addr == 0 || in.s_addr == ~0UL)
			{
				if(NOT quiet)
				{
					Local_ErrorPrintf(cc,get_str(cc,MSG_CONFIGURENETINTERFACE_INVALID_ADDRESS_TXT),
						cc->cc_ProgramName,args.Address);
				}

				goto out;
			}

			if(((in.s_addr >> 24) & 0xFF) == 169 &&
			   ((in.s_addr >> 16) & 0xFF) == 254)
			{
				Local_ErrorPrintf(cc,get_str(cc,MSG_CONFIGURENETINTERFACE_DYNAMIC_ADDRESS_SHOULD_NOT_BE_BOUND_TXT),
					cc->cc_ProgramName,args.Address);
			}
		}

		FreeVec(address);

		address = AllocVec(strlen(args.Address) + 1,MEMF_ANY|MEMF_PUBLIC);
		if(address != NULL)
		{
			strcpy(address,args.Address);
		}
		else
		{
			if(NOT quiet)
			{
				Local_ErrorPrintf(cc,get_str(cc,MSG_CONFIGURENETINTERFACE_NOT_ENOUGH_MEMORY_TO_CONFIGURE_INTERFACE_ADDRESS_TXT),
					cc->cc_ProgramName,args.Interface);
			}

			goto out;
		}
	}

	if(args.NetMask != NULL)
	{
		if(Stricmp(args.NetMask,"DHCP") != SAME)
		{
			if(CANNOT validate_ip_address(cc,quiet,"NETMASK",args.NetMask))
				goto out;
		}

		FreeVec(net_mask);

		net_mask = AllocVec(strlen(args.NetMask) + 1,MEMF_ANY|MEMF_PUBLIC);
		if(net_mask != NULL)
		{
			strcpy(net_mask,args.NetMask);
		}
		else
		{
			if(NOT quiet)
			{
				Local_ErrorPrintf(cc,get_str(cc,MSG_CONFIGURENETINTERFACE_NOT_ENOUGH_MEMORY_TO_CONFIGURE_INTERFACE_MASK_TXT),
					cc->cc_ProgramName,args.Interface);
			}

			goto out;
		}
	}

	if(args.DestinationAddress != NULL)
	{
		if(CANNOT validate_ip_address(cc,quiet,"DESTINATIONADDR",args.DestinationAddress))
			goto out;

		error = add_tag(cc,IFC_DestinationAddress,(ULONG)args.DestinationAddress);
		if(error != OK)
		{
			if(NOT quiet)
			{
				Local_ErrorPrintf(cc,get_str(cc,MSG_CONFIGURENETINTERFACE_NOT_ENOUGH_MEMORY_TO_CONFIGURE_INTERFACE_DESTINATION_ADDRESS_TXT),
					cc->cc_ProgramName,args.Interface,args.DestinationAddress);
			}

			goto out;
		}
	}

	if(args.BroadcastAddress != NULL)
	{
		if(CANNOT validate_ip_address(cc,quiet,"BROADCASTADDR",args.BroadcastAddress))
			goto out;

		error = add_tag(cc,IFC_BroadcastAddress,(ULONG)args.BroadcastAddress);
		if(error != OK)
		{
			if(NOT quiet)
			{
				Local_ErrorPrintf(cc,get_str(cc,MSG_CONFIGURENETINTERFACE_NOT_ENOUGH_MEMORY_TO_CONFIGURE_INTERFACE_BROADCAST_ADDRESS_TXT),
					cc->cc_ProgramName,args.Interface,args.BroadcastAddress);
			}

			goto out;
		}
	}

	if(args.Metric != NULL)
	{
		error = add_tag(cc,IFC_Metric,(*args.Metric));
		if(error != OK)
		{
			if(NOT quiet)
			{
				Local_ErrorPrintf(cc,get_str(cc,MSG_CONFIGURENETINTERFACE_NOT_ENOUGH_MEMORY_TO_CONFIGURE_INTERFACE_METRIC_TXT),
					cc->cc_ProgramName,args.Interface,(*args.Metric));
			}

			goto out;
		}
	}

	if(args.MTU != NULL && (*args.MTU) > 0)
	{
		error = add_tag(cc,IFC_LimitMTU,(*args.MTU));
		if(error != OK)
		{
			if(NOT quiet)
			{
				Local_ErrorPrintf(cc,get_str(cc,MSG_CONFIGURENETINTERFACE_NOT_ENOUGH_MEMORY_TO_CONFIGURE_INTERFACE_MTU_TXT),
					cc->cc_ProgramName,args.Interface,(*args.MTU));
			}

			goto out;
		}
	}

	if(args.Timeout != NULL && (*args.Timeout) > 0)
	{
		timeout = (*args.Timeout);
		if(timeout < AAM_TIMEOUT_MIN)
		{
			if(NOT quiet)
			{
				Local_ErrorPrintf(cc,get_str(cc,MSG_CONFIGURENETINTERFACE_TIMEOUT_TOO_SHORT_TXT),
					cc->cc_ProgramName,timeout);
			}

			timeout = AAM_TIMEOUT_MIN;
		}
	}
	else
	{
		timeout = 60;
	}

	if(args.AddAddress != NULL)
	{
		if(CANNOT validate_ip_address(cc,quiet,"ALIASADDR",args.AddAddress))
			goto out;

		error = add_tag(cc,IFC_AddAliasAddress,(ULONG)args.AddAddress);
		if(error != OK)
		{
			if(NOT quiet)
			{
				Local_ErrorPrintf(cc,get_str(cc,MSG_CONFIGURENETINTERFACE_NOT_ENOUGH_MEMORY_TO_ADD_ALIAS_ADDRESS_TXT),
					cc->cc_ProgramName,args.Interface,args.AddAddress);
			}

			goto out;
		}
	}

	if(args.DeleteAddress != NULL)
	{
		if(CANNOT validate_ip_address(cc,quiet,"DELETEADDR",args.DeleteAddress))
			goto out;

		error = add_tag(cc,IFC_DeleteAliasAddress,(ULONG)args.DeleteAddress);
		if(error != OK)
		{
			if(NOT quiet)
			{
				Local_ErrorPrintf(cc,get_str(cc,MSG_CONFIGURENETINTERFACE_NOT_ENOUGH_MEMORY_TO_DELETE_ALIAS_ADDRESS_TXT),
					cc->cc_ProgramName,args.Interface,args.DeleteAddress);
			}

			goto out;
		}
	}

	if(args.Debug != NULL)
	{
		BOOL mode = FALSE;

		if(Stricmp(args.Debug,"ON") == SAME || Stricmp(args.Debug,"YES") == SAME)
		{
			mode = TRUE;
		}
		else if (Stricmp(args.Debug,"OFF") != SAME && Stricmp(args.Debug,"NO") != SAME)
		{
			if(NOT quiet)
			{
				Local_ErrorPrintf(cc,get_str(cc,MSG_CONFIGURENETINTERFACE_UNKNOWN_DEBUG_PARAMETER_TXT),
					cc->cc_ProgramName,args.Debug);
			}

			goto out;
		}

		error = add_tag(cc,IFC_SetDebugMode,(ULONG)mode);
		if(error != OK)
		{
			if(NOT quiet)
			{
				Local_ErrorPrintf(cc,get_str(cc,MSG_CONFIGURENETINTERFACE_NOT_ENOUGH_MEMORY_TO_CONFIGURE_TXT),
					cc->cc_ProgramName,args.Interface);
			}

			goto out;
		}
	}

	if(args.Complete != NULL)
	{
		BOOL complete = FALSE;

		if(Stricmp(args.Complete,"ON") == SAME || Stricmp(args.Complete,"YES") == SAME)
		{
			complete = TRUE;
		}
		else if (Stricmp(args.Complete,"OFF") != SAME && Stricmp(args.Complete,"NO") != SAME)
		{
			if(NOT quiet)
			{
				Local_ErrorPrintf(cc,get_str(cc,MSG_CONFIGURENETINTERFACE_UNKNOWN_COMPLETE_PARAMETER_TXT),
					cc->cc_ProgramName,args.Complete);
			}

			goto out;
		}

		error = add_tag(cc,IFC_Complete,(ULONG)complete);
		if(error != OK)
		{
			if(NOT quiet)
			{
				Local_ErrorPrintf(cc,get_str(cc,MSG_CONFIGURENETINTERFACE_NOT_ENOUGH_MEMORY_TO_CONFIGURE_TXT),
					cc->cc_ProgramName,args.Interface);
			}

			goto out;
		}
	}

	if(args.Configure != NULL)
	{
		if(Stricmp(args.Configure,"DHCP") == SAME)
		{
			configure_dynamic = TRUE;
			configure_auto = FALSE;
		}
		else if (Stricmp(args.Configure,"SLOWAUTO") == SAME || Stricmp(args.Configure,"AUTO") == SAME)
		{
			configure_dynamic = configure_auto = configure_slow_auto = TRUE;
		}
		else if (Stricmp(args.Configure,"FASTAUTO") == SAME)
		{
			configure_dynamic = configure_auto = TRUE;
			configure_slow_auto = FALSE;
		}
		else
		{
			if(NOT quiet)
			{
				Local_ErrorPrintf(cc,get_str(cc,MSG_CONFIGURENETINTERFACE_UNKNOWN_CONFIGURE_PARAMETER_TXT),
					cc->cc_ProgramName,args.Configure);
			}

			goto out;
		}
	}

	if(args.Online || args.Offline || args.Up || args.Down)
	{
		STRPTR switches[4];
		LONG count;

		memset(switches,0,sizeof(switches));

		count = 0;

		if(args.Online)
		{
			state_name = "ONLINE";
			state = SM_Online;

			switches[count++] = state_name;
		}

		if(args.Offline)
		{
			state_name = "OFFLINE";
			state = SM_Offline;

			switches[count++] = state_name;
		}

		if(args.Up)
		{
			state_name = "UP";
			state = SM_Up;

			switches[count++] = state_name;
		}

		if(args.Down)
		{
			state_name = "DOWN";
			state = SM_Down;

			switches[count++] = state_name;
		}

		if(count > 1)
		{
			if(NOT quiet)
			{
				LONG i;

				Local_ErrorPrintf(cc,get_str(cc,MSG_CONFIGURENETINTERFACE_PICK_ONLY_ONE_STATE_TXT),cc->cc_ProgramName);

				for(i = 0 ; i < count ; i++)
				{
					if(i > 0)
						Local_ErrorPrintf(cc,",");

					Local_ErrorPrintf(cc," %s",switches[i]);
				}

				Local_ErrorPrintf(cc,".\n");
			}

			goto out;
		}
	}

	if(args.Lease != NULL)
	{
		STRPTR s = args.Lease;
		STRPTR key = NULL;
		LONG number;
		LONG len;

		while(is_blank_space(*s))
			s++;

		len = 0;
		number = 0;
		while('0' <= (*s) && (*s) <= '9')
		{
			number = (10 * number) + (*s) - '0';
			len++;
			s++;
		}

		while(is_blank_space(*s))
			s++;

		if(s[0] != '\0')
			key = s;

		if(len == 0 && key != NULL)
		{
			if(substring_matches(cc,"INF",key) == SAME)
			{
				lease_time = DHCP_INFINITE_LEASE_TIME;
			}
			else
			{
				if(NOT quiet)
				{
					Local_ErrorPrintf(cc,get_str(cc,MSG_CONFIGURENETINTERFACE_UNKNOWN_LEASE_PARAMETER_TXT),
						cc->cc_ProgramName,key);
				}

				goto out;
			}
		}
		else if (len > 0)
		{
			lease_time = number;

			if(key != NULL)
			{
				if(substring_matches(cc,"SECOND",key) == SAME)
				{
					/* This is the default unit */
				}
				else if (substring_matches(cc,"MINUTE",key) == SAME)
				{
					lease_time *= 60;
				}
				else if (substring_matches(cc,"HOUR",key) == SAME)
				{
					lease_time *= 60 * 60;
				}
				else if (substring_matches(cc,"DAY",key) == SAME)
				{
					lease_time *= 24 * 60 * 60;
				}
				else if (substring_matches(cc,"WEEK",key) == SAME)
				{
					lease_time *= 7 * 24 * 60 * 60;
				}
				else
				{
					if(NOT quiet)
					{
						Local_ErrorPrintf(cc,get_str(cc,MSG_CONFIGURENETINTERFACE_UNKNOWN_LEASE_PARAMETER_TXT),
							cc->cc_ProgramName,key);
					}

					goto out;
				}
			}
		}
		else
		{
			if(NOT quiet)
			{
				Local_ErrorPrintf(cc,get_str(cc,MSG_CONFIGURENETINTERFACE_INVALID_LEASE_PARAMETER_TXT),
					cc->cc_ProgramName,args.Lease);
			}

			goto out;
		}
	}

	if(args.ID != NULL)
	{
		if(strlen(args.ID) > 255 && NOT quiet)
		{
			Local_ErrorPrintf(cc,get_str(cc,MSG_CONFIGURENETINTERFACE_ID_TOO_LONG_TXT),
				args.ID);
		}

		if(strlen(args.ID) < 2)
		{
			if(NOT quiet)
			{
				Local_ErrorPrintf(cc,get_str(cc,MSG_CONFIGURENETINTERFACE_ID_TOO_SHORT_TXT),
					args.ID);
			}

			goto out;
		}

		client_id = args.ID;
	}

	if(args.ReleaseAddress)
	{
		if(Local_ConfigureInterfaceTags(cc,args.Interface,
			IFC_ReleaseAddress,TRUE,
		TAG_DONE) != 0)
		{
			if(NOT quiet)
			{
				LONG errno;
				STRPTR code;

				get_errno_and_code(cc,&errno,&code);

				Local_ErrorPrintf(cc,get_str(cc,MSG_CONFIGURENETINTERFACE_COULD_NOT_RELEASE_INTERFACE_ADDRESS_TXT),
					cc->cc_ProgramName,args.Interface);

				if(code != NULL && errno > 0)
					Local_ErrorPrintf(cc," (%s)",code);

				Local_ErrorPrintf(cc,".\n");
			}

			goto out;
		}

		if(NOT quiet)
		{
			Local_Printf(cc,get_str(cc,MSG_CONFIGURENETINTERFACE_INTERFACE_ADDRESS_WAS_RELEASED_TXT),
				args.Interface);
		}
	}

	if(address != NULL || configure_dynamic)
	{
		BOOL want_address = (BOOL)(address != NULL && Stricmp(address,"DHCP") == SAME);
		BOOL want_netmask = (BOOL)(net_mask != NULL && Stricmp(net_mask,"DHCP") == SAME);

		if(configure_dynamic || want_address || want_netmask)
		{
			LONG signals;

			/* Remember the client identifier. */
			aam->aam_ClientIdentifier = client_id;

			/* We have to have the reply port ready. */
			if(cc->cc_ReplyPort == NULL)
			{
				cc->cc_ReplyPort = CreateMsgPort();
				if(cc->cc_ReplyPort == NULL)
				{
					if(NOT quiet)
						Local_ErrorPrintf(cc,get_str(cc,MSG_CONFIGURENETINTERFACE_NOT_ENOUGH_MEMORY_TO_CONFIGURE_TXT),cc->cc_ProgramName,args.Interface);

					goto out;
				}
			}

			/* Before we begin, we will need to mark this
			   interface 'up' so that the protocol stack will
			   send messages through it. */
			if(Local_ConfigureInterfaceTags(cc,args.Interface,
				IFC_State,SM_Up,
			TAG_DONE) != 0)
			{
				if(NOT quiet)
				{
					LONG errno;
					STRPTR code;

					get_errno_and_code(cc,&errno,&code);

					Local_ErrorPrintf(cc,get_str(cc,MSG_CONFIGURENETINTERFACE_COULD_NOT_MARK_INTERFACE_UP_TXT),
						cc->cc_ProgramName,args.Interface);

					if(code != NULL && errno > 0)
						Local_ErrorPrintf(cc," (%s)",code);

					Local_ErrorPrintf(cc,".\n");
				}

				goto out;
			}

			if(NOT quiet)
			{
				Local_Printf(cc,get_str(cc,MSG_CONFIGURENETINTERFACE_TRYING_INTERFACE_CONFIGURATION_TXT),
					cc->cc_ProgramName,args.Interface);

				Flush(Output());
			}

			/* Initialize the message, then send it. */
			aam->aam_Message.mn_Node.ln_Type	= NT_REPLYMSG;
			aam->aam_Message.mn_ReplyPort		= cc->cc_ReplyPort;
			aam->aam_Message.mn_Length			= sizeof(*aam);

			memcpy(aam->aam_InterfaceName,args.Interface,sizeof(aam->aam_InterfaceName)-1);
			aam->aam_InterfaceName[sizeof(aam->aam_InterfaceName)-1] = '\0';

			if(configure_auto)
			{
				if(configure_slow_auto)
					aam->aam_Protocol = AAMP_SLOWAUTO;
				else
					aam->aam_Protocol = AAMP_FASTAUTO;
			}
			else
			{
				aam->aam_Protocol = AAMP_DHCP;
			}

			aam->aam_Version			= AAM_VERSION;
			aam->aam_Timeout			= timeout;
			aam->aam_LeaseTime			= lease_time;
			aam->aam_RouterTable		= cc->cc_RouterTable;
			aam->aam_RouterTableSize	= NUM_ENTRIES(cc->cc_RouterTable);
			aam->aam_DNSTable			= cc->cc_DNSTable;
			aam->aam_DNSTableSize		= NUM_ENTRIES(cc->cc_DNSTable);
			aam->aam_LeaseExpires		= &cc->cc_LeaseExpires;
			aam->aam_DomainName			= default_domain_name;
			aam->aam_DomainNameSize		= sizeof(default_domain_name);

			strcpy(default_domain_name,"");

			if(address != NULL && Stricmp(address,"DHCP") != SAME)
				aam->aam_RequestedAddress = inet_addr(address);

			BeginInterfaceConfig(aam);

			/* Wait for something to happen. */
			signals = Wait(SIGBREAKF_CTRL_C | SIGBREAKF_CTRL_F | (1UL << cc->cc_ReplyPort->mp_SigBit));

			if(signals & SIGBREAKF_CTRL_C)
			{
				AbortInterfaceConfig(aam);
				WaitPort(cc->cc_ReplyPort);
				GetMsg(cc->cc_ReplyPort);

				if(NOT quiet)
				{
					Local_Printf(cc,"\n");
					Local_PrintFault(cc,ERROR_BREAK,cc->cc_ProgramName);
				}

				goto out;
			}
			else if (signals & SIGBREAKF_CTRL_F)
			{
				AbortInterfaceConfig(aam);
				WaitPort(cc->cc_ReplyPort);
				GetMsg(cc->cc_ReplyPort);

				if(NOT quiet)
				{
					Local_Printf(cc,"\n");
					Local_Printf(cc,get_str(cc,MSG_CONFIGURENETINTERFACE_INTERFACE_CONFIGURATION_ABORTED_TXT),
						cc->cc_ProgramName,args.Interface);
				}

				result = RETURN_OK;
				goto out;
			}
			else
			{
				if(NOT quiet)
					Local_Printf(cc,"\n");

				WaitPort(cc->cc_ReplyPort);
				GetMsg(cc->cc_ReplyPort);

				if(aam->aam_Result != AAMR_Success && NOT quiet)
				{
					STATIC CONST error_map_table[][2] =
					{
						{ AAMR_Aborted,				MSG_CONFIGURENETINTERFACE_INTERFACE_CONFIGURATION_ABORTED_TXT },
						{ AAMR_Timeout,				MSG_CONFIGURENETINTERFACE_INTERFACE_CONFIGURATION_TIMED_OUT_TXT },
						{ AAMR_InterfaceNotKnown,	MSG_CONFIGURENETINTERFACE_INTERFACE_CONFIGURATION_NAME_UNKNOWN_TXT },
						{ AAMR_InterfaceWrongType,	MSG_CONFIGURENETINTERFACE_INTERFACE_CONFIGURATION_WRONG_TYPE_TXT },
						{ AAMR_AddressKnown,		MSG_CONFIGURENETINTERFACE_INTERFACE_CONFIGURATION_ALREADY_CONFIGURED_TXT },
						{ AAMR_VersionUnknown,		MSG_CONFIGURENETINTERFACE_INTERFACE_CONFIGURATION_NOT_COMPATIBLE_TXT },
						{ AAMR_NoMemory,			MSG_CONFIGURENETINTERFACE_INTERFACE_CONFIGURATION_NOT_ENOUGH_MEMORY_TXT },
						{ AAMR_AddressInUse,		MSG_CONFIGURENETINTERFACE_INTERFACE_CONFIGURATION_ADDRESS_IN_USE_TXT },
						{ AAMR_AddrChangeFailed,	MSG_CONFIGURENETINTERFACE_INTERFACE_CONFIGURATION_ADDRESS_NOT_CHANGED_TXT },
						{ AAMR_MaskChangeFailed,	MSG_CONFIGURENETINTERFACE_INTERFACE_CONFIGURATION_MASK_NOT_CHANGED_TXT },
						{ AAMR_Busy,				MSG_CONFIGURENETINTERFACE_INTERFACE_IS_BUSY_TXT },
						{ -1,						-1}
					};

					LONG message_code;
					size_t i;

					message_code = MSG_CONFIGURENETINTERFACE_INTERFACE_CONFIGURATION_UNKNOWN_ERROR_TXT ;

					for(i = 0 ; error_map_table[i][0] != -1 ; i++)
					{
						if(aam->aam_Result == error_map_table[i][0])
						{
							message_code = error_map_table[i][1];
							break;
						}
					}

					Local_ErrorPrintf(cc,get_str(cc,message_code),
						cc->cc_ProgramName,args.Interface,aam->aam_Result);
				}

				if(aam->aam_Result != AAMR_Success)
				{
					result = RETURN_OK;
					goto out;
				}

				if(configure_dynamic || want_address)
				{
					UBYTE str[20];

					if(aam->aam_Address == 0)
					{
						if(NOT quiet)
						{
							Local_ErrorPrintf(cc,get_str(cc,MSG_CONFIGURENETINTERFACE_INTERFACE_NOT_CONFIGURED_ADDRESS_INVALID_TXT),
								cc->cc_ProgramName,args.Interface);
						}

						goto out;
					}

					strcpy(str,Inet_NtoA(aam->aam_Address));

					FreeVec(address);

					address = AllocVec(strlen(str)+1,MEMF_ANY|MEMF_PUBLIC);
					if(address == NULL)
					{
						if(NOT quiet)
						{
							Local_ErrorPrintf(cc,get_str(cc,MSG_CONFIGURENETINTERFACE_INTERFACE_CONFIGURATION_NOT_ENOUGH_MEMORY_TXT),
								cc->cc_ProgramName,args.Interface);
						}

						goto out;
					}

					strcpy(address,str);
				}

				if(configure_dynamic || want_netmask)
				{
					if(aam->aam_SubnetMask == 0x00000000 ||
					   aam->aam_SubnetMask == 0xFFFFFFFF)
					{
						FreeVec(net_mask);
						net_mask = NULL;
					}
					else
					{
						UBYTE str[20];

						strcpy(str,Inet_NtoA(aam->aam_SubnetMask));

						FreeVec(net_mask);

						net_mask = AllocVec(strlen(str)+1,MEMF_ANY|MEMF_PUBLIC);
						if(net_mask == NULL)
						{
							if(NOT quiet)
							{
								Local_ErrorPrintf(cc,get_str(cc,MSG_CONFIGURENETINTERFACE_INTERFACE_CONFIGURATION_NOT_ENOUGH_MEMORY_TXT),
									cc->cc_ProgramName,args.Interface);
							}

							goto out;
						}

						strcpy(net_mask,str);
					}
				}

				if(configure_dynamic)
				{
					LONG i,n;

					n = 0;
					for(i = 0 ; i < aam->aam_RouterTableSize ; i++)
					{
						if(aam->aam_RouterTable[i] != 0)
							n++;
					}

					if(n == 0)
						aam->aam_RouterTable = NULL;

					n = 0;
					for(i = 0 ; i < aam->aam_DNSTableSize ; i++)
					{
						if(aam->aam_DNSTable[i] != 0)
							n++;
					}

					if(n == 0)
						aam->aam_DNSTable = NULL;
				}

				/* If we are about to set the interface address, we will also
				   have to take care of the interface state, which defaults to
				   'up'. */
				if(state == -1)
					state = SM_Up;
			}
		}
	}

	if(address != NULL)
	{
		error = add_tag(cc,IFC_Address,(ULONG)address);
		if(error != OK)
		{
			if(NOT quiet)
			{
				Local_ErrorPrintf(cc,get_str(cc,MSG_CONFIGURENETINTERFACE_NOT_ENOUGH_MEMORY_TO_CONFIGURE_ADDRESS_TXT),
					cc->cc_ProgramName,args.Interface,address);
			}

			goto out;
		}
	}

	if(net_mask != NULL)
	{
		error = add_tag(cc,IFC_NetMask,(ULONG)net_mask);
		if(error != OK)
		{
			if(NOT quiet)
			{
				Local_ErrorPrintf(cc,get_str(cc,MSG_CONFIGURENETINTERFACE_NOT_ENOUGH_MEMORY_TO_CONFIGURE_NETMASK_TXT),
					cc->cc_ProgramName,args.Interface,net_mask);
			}

			goto out;
		}
	}

	if(cc->cc_NumTags > 0)
	{
		if(state == SM_Online || state == SM_Up)
		{
			error = add_tag(cc,IFC_State,state);
			if(error != OK)
			{
				if(NOT quiet)
				{
					Local_ErrorPrintf(cc,get_str(cc,MSG_CONFIGURENETINTERFACE_NOT_ENOUGH_MEMORY_TO_CONFIGURE_STATE_TXT),
						cc->cc_ProgramName,args.Interface);
				}

				goto out;
			}

			state = -1;
		}

		error = add_tag(cc,TAG_END,0);
		if(error != OK)
		{
			if(NOT quiet)
			{
				Local_ErrorPrintf(cc,get_str(cc,MSG_CONFIGURENETINTERFACE_NOT_ENOUGH_MEMORY_TO_CONFIGURE_INTERFACE_TXT),
					cc->cc_ProgramName,args.Interface);
			}

			goto out;
		}

		if(ConfigureInterfaceTagList(args.Interface,cc->cc_Tags) != OK)
		{
			if(NOT quiet)
			{
				LONG error = OK;
				STRPTR code = NULL;

				get_errno_and_code(cc,&error,&code);

				Local_ErrorPrintf(cc,get_str(cc,MSG_CONFIGURENETINTERFACE_COULD_NOT_CONFIGURE_INTERFACE_TXT),
					cc->cc_ProgramName,args.Interface);

				if(code != NULL && error > 0)
					Local_ErrorPrintf(cc," (%s)",code);

				Local_ErrorPrintf(cc,".\n");
			}

			goto out;
		}

		if(NOT quiet)
		{
			STRPTR prefix = "";

			Local_Printf(cc,get_str(cc,MSG_CONFIGURENETINTERFACE_INTERFACE_CONFIGURED_TXT),
				cc->cc_ProgramName,args.Interface);

			if(args.Address != NULL)
			{
				Local_Printf(cc,get_str(cc,MSG_CONFIGURENETINTERFACE_INTERFACE_ADDRESS_IS_TXT),prefix,args.Address);
				prefix = ", ";
			}

			if(args.NetMask != NULL)
			{
				Local_Printf(cc,get_str(cc,MSG_CONFIGURENETINTERFACE_INTERFACE_MASK_TXT),prefix,args.NetMask);
				prefix = ", ";
			}

			if(args.DestinationAddress != NULL)
			{
				Local_Printf(cc,get_str(cc,MSG_CONFIGURENETINTERFACE_INTERFACE_DESTINATION_ADDRESS_TXT),prefix,args.DestinationAddress);
				prefix = ", ";
			}

			if(args.BroadcastAddress != NULL)
			{
				Local_Printf(cc,get_str(cc,MSG_CONFIGURENETINTERFACE_INTERFACE_BROADCAST_ADDRESS_TXT),prefix,args.BroadcastAddress);
				prefix = ", ";
			}

			if(args.Metric != NULL)
			{
				Local_Printf(cc,get_str(cc,MSG_CONFIGURENETINTERFACE_INTERFACE_METRIC_TXT),prefix,(*args.Metric));
				prefix = ", ";
			}

			if(args.AddAddress != NULL)
			{
				Local_Printf(cc,get_str(cc,MSG_CONFIGURENETINTERFACE_INTERFACE_ADD_ADDRESS_TXT),prefix,args.AddAddress);
				prefix = ", ";
			}

			if(args.DeleteAddress != NULL)
			{
				Local_Printf(cc,get_str(cc,MSG_CONFIGURENETINTERFACE_INTERFACE_DELETE_ADDRESS_TXT),prefix,args.DeleteAddress);
			}

			Local_Printf(cc,").\n");
		}
	}

	if(configure_dynamic)
	{
		STRPTR domain_name;

		if(aam->aam_DomainName != NULL && aam->aam_DomainName[0] != '\0')
			domain_name = aam->aam_DomainName;
		else
			domain_name = "";

		SetDefaultDomainName(domain_name);

		if(domain_name[0] != '\0' && NOT quiet)
		{
			Local_Printf(cc,get_str(cc,MSG_CONFIGURENETINTERFACE_DEFAULT_DOMAIN_NAME_TXT),
				cc->cc_ProgramName,domain_name);
		}
	}

	if(configure_dynamic && aam->aam_RouterTable != NULL)
	{
		LONG i,n;

		n = 0;
		for(i = 0 ; i < aam->aam_RouterTableSize ; i++)
		{
			if(aam->aam_RouterTable[i] != 0)
			{
				UBYTE str[20];
				LONG error = OK;
				STRPTR code = NULL;

				strcpy(str,Inet_NtoA(aam->aam_RouterTable[i]));

				if(n == 0)
				{
					if(Local_AddRouteTags(cc,
						RTA_DefaultGateway,str,
					TAG_DONE) != OK)
					{
						get_errno_and_code(cc,&error,&code);
					}

					if(error == OK && NOT quiet)
					{
						Local_Printf(cc,get_str(cc,MSG_CONFIGURENETINTERFACE_ADDED_DEFAULT_ROUTE_TXT),
							cc->cc_ProgramName,str);
					}
				}
				else
				{
					if(Local_AddRouteTags(cc,
						RTA_Destination,str,
					TAG_DONE) != OK)
					{
						get_errno_and_code(cc,&error,&code);
					}

					if(error == OK && NOT quiet)
					{
						Local_Printf(cc,get_str(cc,MSG_CONFIGURENETINTERFACE_ADDED_ROUTE_TXT),
							cc->cc_ProgramName,str);
					}
				}

				if(error != OK)
				{
					if(NOT quiet)
					{
						Local_ErrorPrintf(cc,get_str(cc,MSG_CONFIGURENETINTERFACE_COULD_NOT_ADD_ROUTE_TO_TXT),
							cc->cc_ProgramName,str);

						if(code != NULL && error > 0)
							Local_ErrorPrintf(cc," (%s)",code);

						Local_ErrorPrintf(cc,".\n");
					}

					goto out;
				}

				n++;
			}
		}
	}

	if(configure_dynamic && aam->aam_DNSTable != NULL)
	{
		LONG i;

		for(i = 0 ; i < aam->aam_DNSTableSize ; i++)
		{
			if(aam->aam_DNSTable[i] != 0)
			{
				UBYTE str[20];
				LONG error = OK;
				STRPTR code = NULL;

				strcpy(str,Inet_NtoA(aam->aam_DNSTable[i]));

				if(AddDomainNameServer(str) != OK)
					get_errno_and_code(cc,&error,&code);

				if(error != OK)
				{
					if(NOT quiet)
					{
						Local_ErrorPrintf(cc,get_str(cc,MSG_CONFIGURENETINTERFACE_COULD_NOT_ADD_DNS_TXT),
							cc->cc_ProgramName,str);

						if(code != NULL && error > 0)
							Local_ErrorPrintf(cc," (%s)",code);

						Local_ErrorPrintf(cc,".\n");
					}

					goto out;
				}

				if(NOT quiet)
				{
					Local_Printf(cc,get_str(cc,MSG_CONFIGURENETINTERFACE_ADDED_DNS_TXT),
						cc->cc_ProgramName,str);
				}
			}
		}
	}

	if(configure_dynamic && aam->aam_Protocol == AAMP_DHCP && NOT quiet)
	{
		Local_Printf(cc,get_str(cc,MSG_CONFIGURENETINTERFACE_INTERFACE_ADDRESS_TXT),
			cc->cc_ProgramName,args.Interface,address);

		if(cc->cc_LeaseExpires.ds_Days		== 0 &&
		   cc->cc_LeaseExpires.ds_Minute	== 0 &&
		   cc->cc_LeaseExpires.ds_Tick		== 0)
		{
			Local_Printf(cc,get_str(cc,MSG_CONFIGURENETINTERFACE_LEASED_PERMANENTLY_TXT));
		}
		else
		{
			struct DateTime dat;
			UBYTE date[LEN_DATSTRING+1];
			UBYTE time[LEN_DATSTRING+1];

			memset(&dat,0,sizeof(dat));

			dat.dat_Stamp	= cc->cc_LeaseExpires;
			dat.dat_Format	= FORMAT_DEF;
			dat.dat_StrDate	= date;
			dat.dat_StrTime	= time;

			DateToStr(&dat);

			strip_extra_blank_spaces(date);
			strip_extra_blank_spaces(time);

			Local_Printf(cc,get_str(cc,MSG_CONFIGURENETINTERFACE_LEASED_UNTIL_TXT),
				date,time);
		}
	}

	if(state != -1)
	{
		if(Local_ConfigureInterfaceTags(cc,args.Interface,
			IFC_State,state,
		TAG_END) != OK)
		{
			if(NOT quiet)
			{
				LONG error = OK;
				STRPTR code = NULL;

				get_errno_and_code(cc,&error,&code);

				Local_ErrorPrintf(cc,get_str(cc,MSG_CONFIGURENETINTERFACE_COULD_NOT_SET_STATE_TXT),
					cc->cc_ProgramName,args.Interface,state_name);

				if(code != NULL && error > 0)
					Local_ErrorPrintf(cc," (%s)",code);

				Local_ErrorPrintf(cc,".\n");
			}

			if(state == SM_Online)
				result = RETURN_WARN;

			goto out;
		}

		if(NOT quiet)
		{
			Local_Printf(cc,get_str(cc,MSG_CONFIGURENETINTERFACE_STATE_CHANGED_TXT),
					cc->cc_ProgramName,args.Interface,state_name);
		}
	}

	result = RETURN_OK;

 out:

	DeleteMsgPort(cc->cc_ReplyPort);

	FreeVec(address);
	FreeVec(net_mask);

	if(quiet && result != RETURN_OK)
		result = RETURN_WARN;

	if(rda != NULL)
		FreeArgs(rda);

	return(result);
}

/****************************************************************************/

STATIC BOOL
validate_ip_address(struct CommandContext * cc,BOOL quiet,STRPTR key,STRPTR address)
{
	DECLARE_SOCKETBASE(cc);

	struct in_addr in;
	BOOL result;

	if(inet_aton(address,&in))
	{
		result = TRUE;
	}
	else
	{
		if(NOT quiet)
			Local_ErrorPrintf(cc,get_str(cc,MSG_CONFIGURENETINTERFACE_INVALID_IP_ADDRESS_AT_TXT),cc->cc_ProgramName,key,address);

		result = FALSE;
	}

	return(result);
}

/****************************************************************************/

STATIC LONG
add_tag(struct CommandContext * cc,Tag tag,ULONG data)
{
	DECLARE_SYSBASE(cc);

	LONG error = OK;
	LONG which = -1;
	LONG i;

	for(i = 0 ; i < cc->cc_NumTags ; i++)
	{
		if(cc->cc_Tags[i].ti_Tag == tag)
		{
			which = i;
			break;
		}
	}

	if(which == -1 && cc->cc_NumTags + 1 >= cc->cc_MaxTags)
	{
		struct TagItem * new_tags;

		new_tags = AllocVec(sizeof(*new_tags) * (cc->cc_MaxTags+10),MEMF_ANY|MEMF_PUBLIC);
		if(new_tags == NULL)
		{
			error = ERROR_NO_FREE_STORE;
			goto out;
		}

		if(cc->cc_Tags != NULL)
			CopyMem(cc->cc_Tags,new_tags,sizeof(*new_tags) * cc->cc_NumTags);

		FreeVec(cc->cc_Tags);
		cc->cc_Tags = new_tags;
		cc->cc_MaxTags += 10;
	}

	if(which == -1)
		which = cc->cc_NumTags++;

	cc->cc_Tags[which].ti_Tag	= tag;
	cc->cc_Tags[which].ti_Data	= data;

 out:

	return(error);
}

/****************************************************************************/

STATIC VOID
get_errno_and_code(struct CommandContext * cc,LONG * errno_ptr,STRPTR * code_ptr)
{
	LONG errno = 0;
	LONG code;

	Local_SocketBaseTags(cc,
	    SBTM_GETREF(SBTC_ERRNO),&errno,
	TAG_END);

	code = errno;

	if(Local_SocketBaseTags(cc,
	    SBTM_GETREF(SBTC_ERRNOSTRPTR),&code,
	TAG_END) != 0)
	{
		code = 0;
	}

	(*errno_ptr)	= errno;
	(*code_ptr)		= (STRPTR)code;
}

/****************************************************************************/

STATIC BOOL
substring_matches(struct CommandContext * cc,STRPTR pattern,STRPTR string)
{
	DECLARE_UTILITYBASE(cc);

	BOOL result = FALSE;
	LONG i,len,pattern_len;

	len = strlen(string);
	pattern_len = strlen(pattern);

	for(i = 0 ; i <= len - pattern_len ; i++)
	{
		if(Strnicmp(&string[len],pattern,pattern_len) == SAME)
		{
			result = TRUE;
			break;
		}
	}

	return(result);
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

STATIC LONG VARARGS68K
Local_Printf(struct CommandContext * cc,STRPTR format,...)
{
	DECLARE_DOSBASE(cc);

	va_list args;
	LONG result;

	#if defined(__amigaos4__)
	{
		va_startlinear(args,format);
		result = VPrintf(format,va_getlinearva(args,APTR));
		va_end(args);
	}
	#else
	{
		va_start(args,format);
		result = VPrintf(format,args);
		va_end(args);
	}
	#endif /* __amigaos4__ */

	return(result);
}

/****************************************************************************/

STATIC VOID
Local_PrintFault(struct CommandContext * cc,LONG error,STRPTR prefix)
{
	DECLARE_DOSBASE(cc);

	UBYTE str[100];

	Fault(error,NULL,str,sizeof(str));

	Local_Printf(cc,"%s: %s\n",prefix,str);
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

STATIC STRPTR
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

STATIC LONG VARARGS68K
Local_AddRouteTags(struct CommandContext * cc,...)
{
	DECLARE_SOCKETBASE(cc);

	va_list args;
	LONG result;

	#if defined(__amigaos4__)
	{
		va_startlinear(args,cc);
		result = AddRouteTagList(va_getlinearva(args,struct TagItem *));
		va_end(args);
	}
	#else
	{
		va_start(args,cc);
		result = AddRouteTagList((struct TagItem *)args);
		va_end(args);
	}
	#endif /* __amigaos4__ */

	return(result);
}

STATIC LONG VARARGS68K
Local_ConfigureInterfaceTags(struct CommandContext * cc,STRPTR interface_name,...)
{
	DECLARE_SOCKETBASE(cc);

	va_list args;
	LONG result;

	#if defined(__amigaos4__)
	{
		va_startlinear(args,interface_name);
		result = ConfigureInterfaceTagList(interface_name,va_getlinearva(args,struct TagItem *));
		va_end(args);
	}
	#else
	{
		va_start(args,interface_name);
		result = ConfigureInterfaceTagList(interface_name,(struct TagItem *)args);
		va_end(args);
	}
	#endif /* __amigaos4__ */

	return(result);
}

STATIC LONG VARARGS68K
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
