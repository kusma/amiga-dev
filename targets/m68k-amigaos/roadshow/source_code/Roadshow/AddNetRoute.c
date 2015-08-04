/*
 *	$Id: AddNetRoute.c,v 1.11 2007-08-26 12:30:26 obarthel Exp $
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
#include <proto/locale.h>
#include <proto/exec.h>
#include <proto/dos.h>

/****************************************************************************/

#include <string.h>
#include <stdarg.h>

/****************************************************************************/

#include "AddNetRoute_rev.h"

/****************************************************************************/

#include "macros.h"

/****************************************************************************/

#ifndef AbsExecBase
#define AbsExecBase (*(struct ExecBase **)4)
#endif /* AbsExecBase */

/****************************************************************************/

typedef LONG *	NUMBER;
typedef LONG	SWITCH;
typedef STRPTR	KEY;

/****************************************************************************/

struct CommandContext
{
	struct Library *		cc_SysBase;
	struct Library *		cc_DOSBase;
	struct Library *		cc_LocaleBase;
	struct Library *		cc_SocketBase;

	#if defined(__amigaos4__)

	/************************************************************************/

	struct ExecIFace *		cc_IExec;
	struct DOSIFace *		cc_IDOS;
	struct LocaleIFace *	cc_ILocale;
	struct SocketIFace *	cc_ISocket;

	/************************************************************************/

	#endif /* __amigaos4__ */

	struct Catalog *		cc_Catalog;
	UBYTE					cc_ProgramName[256];
	struct TagItem *		cc_Tags;
	LONG					cc_NumTags;
	LONG					cc_MaxTags;
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
STATIC LONG add_tag(struct CommandContext *cc, Tag tag, ULONG data);
STATIC LONG Local_Printf(struct CommandContext *cc, STRPTR format, ...);
STATIC LONG VARARGS68K Local_ErrorPrintf(struct CommandContext *cc, STRPTR format, ...);
STATIC STRPTR get_builtin_str(LONG id);
STATIC STRPTR get_str(struct CommandContext *cc, LONG id);
STATIC LONG VARARGS68K Local_SocketBaseTags(struct CommandContext *cc, ...);

/****************************************************************************/

/****** ROADSHOW/ADDNETROUTE **************************************************
*
*   NAME
*	AddNetRoute - Add message routing paths.
*
*   FORMAT
*	AddNetRoute [QUIET] [DESTINATION=<IP>] [HOSTDESTINATION=<IP>]
*	            [NETDESTINATION=<IP>] [GATEWAY=<IP>] [DEFAULTGATEWAY=<IP>]
*
*   TEMPLATE
*	QUIET/S,DST=DESTINATION/K,HOSTDST=HOSTDESTINATION/K,
*	NETDST=NETDESTINATION/K,VIA=GATEWAY/K,DEFAULT=DEFAULTGATEWAY/K
*
*   PATH
*	C:ADDNETROUTE
*
*   FUNCTION
*	ADDNETROUTE allows to define routes to hosts or networks via an
*	interface.
*
*   OPTIONS
*	QUIET/S
*	    This option causes the program not to emit any error messages
*	    or progress reports. Also, if the program encounters an error
*	    it will flag this as failure code 5 which can be looked at
*	    using the "if warn" shell script command. If this option is
*	    not in effect, failure codes will be more severe and all sorts
*	    of progress information will be displayed.
*
*	DST=DESTINATION/K
*	    The destination address of a route (or in other words, where
*	    the route to be added leads to). This must be an IP address
*	    or a symbolic name. Some routes may require you to specify
*	    a gateway address through which the route has to pass.
*	    Depending upon the address you specify, the protocol stack
*	    will attempt to figure out whether the destination is
*	    supposed to be a host or a network.
*
*	HOSTDST=HOSTDESTINATION/K
*	    Same as the "DST=DESTINATION/K" parameter, except that the
*	    destination is assumed to be a host (rather than a network).
*
*	NETDST=NETDESTINATION/K
*	    Same as the "DST=DESTINATION/K" parameter, except that the
*	    destination is assumed to be a network (rather than a host).
*
*	VIA=GATEWAY/K
*	    This parameter complements the route destination address;
*	    it indicates the address to which a message should be sent
*	    for it to be passed to the destination. This must be an IP
*	    address or a symbolic name.
*
*	DEFAULT=DEFAULTGATEWAY/K
*	    This parameter selects the default gateway address (which
*	    must be specified as an IP address or a symbolic host name)
*	    all messages are sent to which don't have any particular
*	    other routes associated with them.
*	    Another, perhaps less misleading name for "default gateway
*	    address" is "default route".
*
*   NOTES
*	The command is similar to the Unix "route" command.
*
*	If you use the "DEFAULT=DEFAULTGATEWAY/K" parameter, all
*	other destination addresses you may have specified will be
*	ignored. Only one of "DESTINATION", "HOSTDESTINATION" or
*	"NETDESTINATION" will be used; choose only one. Before you add
*	a new default gateway you should delete the old one or you'll
*	get an error message instead.
*
*   EXAMPLES
*	Define a route to the host 192.168.10.12 through a
*	gateway at 192.168.1.1
*
*	    1> ADDNETROUTE HOSTDESTINATION 192.168.10.12 VIA 192.168.1.1
*
*   SEE ALSO
*	DeleteNetRoute
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
	}
	#endif /* __amigaos4__ */

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
#define ADDNETROUTE_CATALOG_STRINGS
#include "roadshow.h"

/****************************************************************************/

STATIC LONG
cmd(struct CommandContext * cc)
{
	struct
	{
		SWITCH	Quiet;
		KEY		Destination;
		KEY		DestinationHost;
		KEY		DestinationNet;
		KEY		Gateway;
		KEY		DefaultGateway;
	} args;

	STRPTR args_template =
		"QUIET/S,"
		"DST=DESTINATION/K,"
		"HOSTDST=HOSTDESTINATION/K,"
		"NETDST=NETDESTINATION/K,"
		"VIA=GATEWAY/K,"
		"DEFAULT=DEFAULTGATEWAY/K"
		VERSTAG;

	DECLARE_DOSBASE(cc);
	DECLARE_SOCKETBASE(cc);

	LONG result = RETURN_FAIL;
	LONG have_routing_api = FALSE;
	struct RDArgs * rda;
	LONG error;
	BOOL quiet = FALSE;

	GetProgramName(cc->cc_ProgramName,sizeof(cc->cc_ProgramName));

	memset(&args,0,sizeof(args));

	rda = ReadArgs(args_template,(LONG *)&args,NULL);
	if(rda == NULL)
	{
		PrintFault(IoErr(),cc->cc_ProgramName);
		goto out;
	}

	quiet = (BOOL)(args.Quiet != 0);

	if(SocketBase == NULL)
	{
		if(NOT quiet)
		{
			Local_ErrorPrintf(cc,get_str(cc,MSG_ADDNETROUTE_CANNOT_OPEN_BSDSOCKET_TXT),
				cc->cc_ProgramName);
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
			Local_ErrorPrintf(cc,get_str(cc,MSG_ADDNETROUTE_BSDSOCKET_HAS_NO_ROUTE_API_TXT),
				cc->cc_ProgramName, SocketBase->lib_Node.ln_Name, SocketBase->lib_Version, SocketBase->lib_Revision);
		}

		goto out;
	}

	if(args.DefaultGateway != NULL)
	{
		error = add_tag(cc,RTA_DefaultGateway,(ULONG)args.DefaultGateway);
		if(error != OK)
		{
			if(NOT quiet)
			{
				Local_ErrorPrintf(cc,get_str(cc,MSG_ADDNETROUTE_NO_MEMORY_FOR_DEFAULT_GATEWAY_TXT),
					cc->cc_ProgramName,args.DefaultGateway);
			}

			goto out;
		}

		if(NOT quiet)
		{
			if(args.Destination != NULL)
			{
				Local_ErrorPrintf(cc,get_str(cc,MSG_ADDNETROUTE_DESTINATION_IGNORED_TXT),
					cc->cc_ProgramName,args.Destination);
			}

			if(args.DestinationHost != NULL)
			{
				Local_ErrorPrintf(cc,get_str(cc,MSG_ADDNETROUTE_HOSTDESTINATION_IGNORED_TXT),
					cc->cc_ProgramName,args.DestinationHost);
			}

			if(args.DestinationNet != NULL)
			{
				Local_ErrorPrintf(cc,get_str(cc,MSG_ADDNETROUTE_NETDESTINATION_IGNORED_TXT),
					cc->cc_ProgramName,args.DestinationNet);
			}

			if(args.Gateway != NULL)
			{
				Local_ErrorPrintf(cc,get_str(cc,MSG_ADDNETROUTE_GATEWAY_IGNORED_TXT),
					cc->cc_ProgramName,args.Gateway);
			}
		}
	}
	else if (args.Gateway != NULL)
	{
		STRPTR dst;

		if(args.Destination != NULL)
			dst = args.Destination;
		else if (args.DestinationHost != NULL)
			dst = args.DestinationHost;
		else if (args.DestinationNet != NULL)
			dst = args.DestinationNet;
		else
			dst = NULL;

		if(dst == NULL)
		{
			if(NOT quiet)
			{
				Local_ErrorPrintf(cc,get_str(cc,MSG_ADDNETROUTE_GATEWAY_NEEDS_DESTINATION_TXT),
					cc->cc_ProgramName,args.Gateway);
			}

			goto out;
		}

		error = add_tag(cc,RTA_Gateway,(ULONG)args.Gateway);
		if(error != OK)
		{
			if(NOT quiet)
			{
				Local_ErrorPrintf(cc,get_str(cc,MSG_ADDNETROUTE_NOT_ENOUGH_MEMORY_FOR_GATEWAY_TXT),
					cc->cc_ProgramName,args.Gateway);
			}

			goto out;
		}
	}
	else
	{
		PrintFault(ERROR_REQUIRED_ARG_MISSING,cc->cc_ProgramName);
		goto out;
	}

	if(args.Destination != NULL)
	{
		error = add_tag(cc,RTA_Destination,(ULONG)args.Destination);
		if(error != OK)
		{
			if(NOT quiet)
			{
				Local_ErrorPrintf(cc,get_str(cc,MSG_ADDNETROUTE_NOT_ENOUGH_MEMORY_FOR_ROUTE_TXT),
					cc->cc_ProgramName,args.Destination);
			}

			goto out;
		}

		if(NOT quiet && args.DestinationHost != NULL)
		{
			Local_ErrorPrintf(cc,get_str(cc,MSG_ADDNETROUTE_HOSTDESTINATION_IGNORED_TXT),
				cc->cc_ProgramName,args.DestinationHost);
		}

		if(NOT quiet && args.DestinationNet != NULL)
		{
			Local_ErrorPrintf(cc,get_str(cc,MSG_ADDNETROUTE_NETDESTINATION_IGNORED_TXT),
				cc->cc_ProgramName,args.DestinationNet);
		}
	}
	else if (args.DestinationHost != NULL)
	{
		error = add_tag(cc,RTA_DestinationHost,(ULONG)args.DestinationHost);
		if(error != OK)
		{
			if(NOT quiet)
			{
				Local_ErrorPrintf(cc,get_str(cc,MSG_ADDNETROUTE_NOT_ENOUGH_MEMORY_FOR_ROUTE_TO_HOST_TXT),
					cc->cc_ProgramName,args.Destination);
			}

			goto out;
		}

		if(NOT quiet && args.DestinationNet != NULL)
		{
			Local_ErrorPrintf(cc,get_str(cc,MSG_ADDNETROUTE_NETDESTINATION_IGNORED_TXT),
				cc->cc_ProgramName,args.DestinationNet);
		}
	}
	else if (args.DestinationNet != NULL)
	{
		error = add_tag(cc,RTA_DestinationNet,(ULONG)args.DestinationNet);
		if(error != OK)
		{
			if(NOT quiet)
			{
				Local_ErrorPrintf(cc,get_str(cc,MSG_ADDNETROUTE_NOT_ENOUGH_MEMORY_FOR_ROUTE_TO_NET_TXT),
					cc->cc_ProgramName,args.Destination);
			}

			goto out;
		}
	}

	if(cc->cc_NumTags > 0)
	{
		error = add_tag(cc,TAG_END,0);
		if(error != OK)
		{
			if(NOT quiet)
			{
				Local_ErrorPrintf(cc,get_str(cc,MSG_ADDNETROUTE_NOT_ENOUGH_MEMORY_TO_ADD_ROUTE_TXT),
					cc->cc_ProgramName);
			}

			goto out;
		}

		if(AddRouteTagList(cc->cc_Tags) != OK)
		{
			if(NOT quiet)
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

				Local_ErrorPrintf(cc,get_str(cc,MSG_ADDNETROUTE_COULD_NOT_ADD_ROUTE_TXT),
					cc->cc_ProgramName);

				if(code > 0 && errno > 0)
					Local_ErrorPrintf(cc," (%s)",code);

				Local_ErrorPrintf(cc,".\n");
			}

			goto out;
		}

		if(NOT quiet)
		{
			STRPTR prefix = "";

			Local_Printf(cc,get_str(cc,MSG_ADDNETROUTE_ROUTE_ADDED_TXT),
				cc->cc_ProgramName);

			if(args.DefaultGateway != NULL)
			{
				Local_Printf(cc,get_str(cc,MSG_ADDNETROUTE_DEFAULT_GATEWAY_TXT),prefix,args.DefaultGateway);
				prefix = ", ";
			}

			if(args.Destination != NULL)
			{
				Local_Printf(cc,get_str(cc,MSG_ADDNETROUTE_DESTINATION_TXT),prefix,args.Destination);
				prefix = ", ";
			}

			if(args.DestinationHost != NULL)
			{
				Local_Printf(cc,get_str(cc,MSG_ADDNETROUTE_DESTINATION_HOST_TXT),prefix,args.DestinationHost);
				prefix = ", ";
			}

			if(args.DestinationNet != NULL)
				Local_Printf(cc,get_str(cc,MSG_ADDNETROUTE_DESTINATION_NET_TXT),prefix,args.DestinationNet);

			if(args.Gateway != NULL)
				Local_Printf(cc,get_str(cc,MSG_ADDNETROUTE_VIA_GATEWAY_TXT),prefix,args.Gateway);

			Local_Printf(cc,").\n");
		}
	}

	result = RETURN_OK;

 out:

	if(quiet && result != RETURN_OK)
		result = RETURN_WARN;

	if(rda != NULL)
		FreeArgs(rda);

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

STATIC LONG
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
