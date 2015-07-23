/*
 *	$Id: DeleteNetRoute.c,v 1.10 2007-08-26 12:30:26 obarthel Exp $
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

#include "macros.h"

/****************************************************************************/

#include "DeleteNetRoute_rev.h"

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
	struct Library *		cc_SocketBase;
	struct Library *		cc_LocaleBase;

	#if defined(__amigaos4__)

	/************************************************************************/

	struct ExecIFace *		cc_IExec;
	struct DOSIFace *		cc_IDOS;
	struct SocketIFace *	cc_ISocket;
	struct LocaleIFace *	cc_ILocale;

	/************************************************************************/

	#endif /* __amigaos4__ */

	struct Catalog *		cc_Catalog;
	UBYTE					cc_ProgramName[256];
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
STATIC LONG VARARGS68K Local_Printf(struct CommandContext *cc, STRPTR format, ...);
STATIC LONG VARARGS68K Local_ErrorPrintf(struct CommandContext *cc, STRPTR format, ...);
STATIC STRPTR get_builtin_str(LONG id);
STATIC STRPTR get_str(struct CommandContext *cc, LONG id);
STATIC LONG VARARGS68K Local_SocketBaseTags(struct CommandContext *cc, ...);
STATIC LONG VARARGS68K Local_DeleteRouteTags(struct CommandContext *cc, ...);

/****************************************************************************/

/****** ROADSHOW/DELETENETROUTE ************************************************
*
*   NAME
*	DeleteNetRoute - Delete a message routing path currently in use.
*
*   FORMAT
*	DeleteNetRoute [QUIET] [DESTINATION=<ip>] [DEFAULTGATEWAY=<ip>]
*
*   TEMPLATE
*	QUIET/S,DST=DESTINATION/K,DEFAULT=DEFAULTGATEWAY/K
*
*   PATH
*	C:DELETENETROUTE
*
*   FUNCTION
*	The commands removes a route that was defined in your network.
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
*	    the route to be added leads to) that should be deleted. This
*	    must be an IP address or a symbolic name.
*
*	DEFAULT=DEFAULTGATEWAY/K
*	    The default gateway address to be deleted. This must be an
*	    IP address or a symbolic name.
*
*   NOTES
*	This command is similar to the Unix "route" command.
*
*	You can try to delete a route that doesn't exist, but it will
*	get you an error message instead of failing gracefully.
*
*   SEE ALSO
*	AddNetRoute
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
#define DELETENETROUTE_CATALOG_STRINGS
#include "roadshow.h"

/****************************************************************************/

STATIC LONG
cmd(struct CommandContext * cc)
{
	struct
	{
		SWITCH	Quiet;
		KEY		Destination;
		KEY		DefaultGateway;
	} args;

	STRPTR args_template =
		"QUIET/S,"
		"DST=DESTINATION/K,"
		"DEFAULT=DEFAULTGATEWAY/K"
		VERSTAG;

	DECLARE_DOSBASE(cc);
	DECLARE_SOCKETBASE(cc);

	LONG result = RETURN_FAIL;
	LONG have_routing_api = FALSE;
	struct RDArgs * rda;
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
			Local_ErrorPrintf(cc,get_str(cc,MSG_DELETENETROUTE_CANNOT_OPEN_BSDSOCKET_TXT),
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
			Local_ErrorPrintf(cc,get_str(cc,MSG_DELETENETROUTE_BSDSOCKET_HAS_NO_ROUTE_API_TXT),
				cc->cc_ProgramName, SocketBase->lib_Node.ln_Name, SocketBase->lib_Version, SocketBase->lib_Revision);
		}

		goto out;
	}

	if(args.Destination == NULL && args.DefaultGateway == NULL)
	{
		if(NOT quiet)
		{
			Local_ErrorPrintf(cc,get_str(cc,MSG_DELETENETROUTE_NEED_EITHER_DESTINATION_OR_DEFAULT_TXT),
				cc->cc_ProgramName);
		}

		goto out;
	}

	if(args.DefaultGateway != NULL && args.Destination != NULL)
	{
		if(NOT quiet)
		{
			Local_ErrorPrintf(cc,get_str(cc,MSG_DELETENETROUTE_DESTINATION_IGNORED_TXT),
				cc->cc_ProgramName,args.Destination);
		}

		args.Destination = NULL;
	}

	if(Local_DeleteRouteTags(cc,
		(args.Destination != NULL) ?	RTA_Destination :		TAG_IGNORE,		args.Destination,
		(args.DefaultGateway != NULL) ?	RTA_DefaultGateway :	TAG_IGNORE,		args.DefaultGateway,
	TAG_END) != OK)
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

			if(args.DefaultGateway != NULL)
			{
				Local_ErrorPrintf(cc,get_str(cc,MSG_DELETENETROUTE_ROUTE_TO_DEFAULT_GATEWAY_NOT_DELETED_TXT),
					cc->cc_ProgramName,args.DefaultGateway);
			}
			else
			{
				Local_ErrorPrintf(cc,get_str(cc,MSG_DELETENETROUTE_ROUTE_NOT_DELETED_TXT),
					cc->cc_ProgramName,args.Destination);
			}

			if(code > 0 && errno > 0)
				Local_ErrorPrintf(cc," (%s)",code);

			Local_ErrorPrintf(cc,".\n");
		}

		goto out;
	}

	if(NOT quiet)
	{
		LONG message_code;

		if(args.DefaultGateway != NULL)
			message_code = MSG_DELETENETROUTE_ROUTE_TO_DEFAULT_DELETED_TXT;
		else
			message_code = MSG_DELETENETROUTE_ROUTE_DELETED_TXT;

		Local_Printf(cc,get_str(cc,message_code),
			cc->cc_ProgramName,args.DefaultGateway);
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
	   file you specify out of order string numbers this routine will fail.*/
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

STATIC LONG VARARGS68K
Local_DeleteRouteTags(struct CommandContext * cc,...)
{
	DECLARE_SOCKETBASE(cc);

	va_list args;
	LONG result;

	#if defined(__amigaos4__)
	{
		va_startlinear(args,cc);
		result = DeleteRouteTagList(va_getlinearva(args,struct TagItem *));
		va_end(args);
	}
	#else
	{
		va_start(args,cc);
		result = DeleteRouteTagList((struct TagItem *)args);
		va_end(args);
	}
	#endif /* __amigaos4__ */

	return(result);
}
