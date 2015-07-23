/*
 *	$Id: GetNetStatus.c,v 1.11 2007-08-26 12:30:26 obarthel Exp $
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

#include <string.h>
#include <stdarg.h>

/****************************************************************************/

#include "macros.h"

/****************************************************************************/

#include "GetNetStatus_rev.h"

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
STATIC STRPTR begin_bold(struct CommandContext *cc);
STATIC STRPTR end_bold(struct CommandContext *cc);
STATIC STRPTR get_next_token(STRPTR input, STRPTR string, LONG string_len);
STATIC LONG VARARGS68K Local_Printf(struct CommandContext *cc, STRPTR format, ...);
STATIC LONG VARARGS68K Local_ErrorPrintf(struct CommandContext *cc, STRPTR format, ...);
STATIC STRPTR get_builtin_str(LONG id);
STATIC STRPTR get_str(struct CommandContext *cc, LONG id);
STATIC LONG VARARGS68K Local_SocketBaseTags(struct CommandContext *cc, ...);

/****************************************************************************/

/****** ROADSHOW/GETNETSTATUS **************************************************
*
*   NAME
*	GetNetStatus - Query whether the network is operational.
*
*   FORMAT
*	GetNetStatus [CHECK=condition[,condition...]] [QUIET]
*
*   TEMPLATE
*	CHECK/K,QUIET/S
*
*   FUNCTION
*	The command is used to check/display which interfaces are currently
*	running and which settings are being configured. It can be used in
*	script files or for quick diagnostic purposes.
*
*   OPTIONS
*	CHECK/K
*	    A list of conditions to check, which must be separated by commas.
*	    the following conditions can be checked:
*
*	        INTERFACES
*	            Are any networking interfaces configured and operational?
*
*	        PTPINTERFACES
*	            Are any point-to-point interfaces, e.g. SLIP and PPP,
*	            configured and operational?
*
*	        BCASTINTERFACES
*	            Are any broadcast interfaces, e.g. Ethernet, configured
*	            and operational?
*
*	        RESOLVER
*	            Are any name resolution servers configured?
*
*	        ROUTES
*	            Is any routing information configured?
*
*	        DEFAULTROUTE
*	            Is the default route configured?
*
*	    If any of the conditions to test for is not satisfied, a message
*	    to this effect will be printed and the command will exit with
*	    status 5, which can be tested in script files using the
*	    'IF WARN' command.
*
*	QUIET/S
*	    Whatever happens, except for errors no output will be produced.
*
*   NOTES
*	If no conditions are to be checked for, then this command will print
*	version information and the list of conditions that can be tested
*	for, indicating which ones are satisfied and which are not.
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
#define GETNETSTATUS_CATALOG_STRINGS
#include "roadshow.h"

/****************************************************************************/

STATIC LONG
cmd(struct CommandContext * cc)
{
	struct
	{
		KEY		Check;
		SWITCH	Quiet;
	} args;

	STRPTR args_template =
		"CHECK/K,"
		"QUIET/S"
		VERSTAG;

	DECLARE_DOSBASE(cc);
	DECLARE_UTILITYBASE(cc);
	DECLARE_SOCKETBASE(cc);

	BOOL check_interfaces = FALSE;
	BOOL check_ptp_interfaces = FALSE;
	BOOL check_broadcast_interfaces = FALSE;
	BOOL check_resolver = FALSE;
	BOOL check_routes = FALSE;
	BOOL check_default_route = FALSE;
	LONG result = RETURN_FAIL;
	struct RDArgs * rda;
	ULONG system_status;

	GetProgramName(cc->cc_ProgramName,sizeof(cc->cc_ProgramName));

	memset(&args,0,sizeof(args));

	rda = ReadArgs(args_template,(LONG *)&args,NULL);
	if(rda == NULL)
	{
		PrintFault(IoErr(),cc->cc_ProgramName);
		goto out;
	}

	if(UtilityBase == NULL)
	{
		Local_ErrorPrintf(cc,get_str(cc,MSG_GETNETSTATUS_NO_UTILITY_TXT),
			cc->cc_ProgramName);

		goto out;
	}

	if(SocketBase == NULL)
	{
		Local_ErrorPrintf(cc,get_str(cc,MSG_GETNETSTATUS_CANNOT_OPEN_BSDSOCKET_TXT),
			cc->cc_ProgramName);

		goto out;
	}

	if(Local_SocketBaseTags(cc,
	    SBTM_GETREF(SBTC_SYSTEM_STATUS),&system_status,
	TAG_END) != 0)
	{
		Local_ErrorPrintf(cc,get_str(cc,MSG_GETNETSTATUS_BSDSOCKET_HAS_NO_STATUS_API_TXT),
			cc->cc_ProgramName, SocketBase->lib_Node.ln_Name, SocketBase->lib_Version, SocketBase->lib_Revision);

		goto out;
	}

	if(args.Check != NULL)
	{
		STRPTR arg;

		arg = args.Check;

		if(strcmp(arg,"?") == SAME)
		{
			Local_Printf(cc,
				"INTERFACES/S,"
				"PTPINTERFACES=PTP/S,"
				"BCASTINTERFACES=BCAST=BROADCAST/S,"
				"RESOLVER=NAMERESOLUTION=DNS/S,"
				"ROUTES/S,"
				"DEFAULTROUTE=DEFAULTGATEWAY/S"
				"\n");
		}
		else
		{
			UBYTE token[40];

			while((arg = get_next_token(arg,token,sizeof(token))) != NULL)
			{
				if(Stricmp(token,"INTERFACES") == SAME)
					check_interfaces = TRUE;
				else if (Stricmp(token,"PTPINTERFACES") == SAME || Stricmp(token,"PTP") == SAME)
					check_ptp_interfaces = TRUE;
				else if (Stricmp(token,"BCASTINTERFACES") == SAME || Stricmp(token,"BCAST") == SAME || Stricmp(token,"BROADCAST") == SAME)
					check_broadcast_interfaces = TRUE;
				else if (Stricmp(token,"RESOLVER") == SAME || Stricmp(token,"NAMERESOLUTION") == SAME || Stricmp(token,"DNS") == SAME)
					check_resolver = TRUE;
				else if (Stricmp(token,"ROUTES") == SAME)
					check_routes = TRUE;
				else if (Stricmp(token,"DEFAULTROUTE") == SAME || Stricmp(token,"DEFAULTGATEWAY") == SAME)
					check_default_route = TRUE;
				else
					Local_ErrorPrintf(cc,get_str(cc,MSG_GETNETSTATUS_UNKNOWN_CONDITION_TXT),cc->cc_ProgramName,token);
			}
		}
	}
	else if (NOT args.Quiet)
	{
		UBYTE string[256];
		LONG len;

		if(SocketBase->lib_IdString != NULL)
		{
			strncpy(string,SocketBase->lib_IdString,sizeof(string)-1);
			string[sizeof(string)-1] = '\0';
		}
		else
		{
			strcpy(string,"");
		}

		len = strlen(string);
		while(len > 0 && (string[len-1] == '\r' || string[len-1] == '\n'))
			string[--len] = '\0';

		if(len > 0)
		{
			STRPTR release_string = NULL;
			BOOL need_line_feed = FALSE;
			LONG i;

			for(i = 0 ; i < len ; i++)
			{
				if(string[i] == '\n')
				{
					need_line_feed = TRUE;
					break;
				}
			}

			Local_Printf(cc,"%s",string);

			if(Local_SocketBaseTags(cc,
			    SBTM_GETREF(SBTC_RELEASESTRPTR),&release_string,
			TAG_END) != 0)
			{
				release_string = NULL;
			}

			if(release_string != NULL)
			{
				strncpy(string,release_string,sizeof(string)-1);
				string[sizeof(string)-1] = '\0';
			}
			else
			{
				strcpy(string,"");
			}

			len = strlen(string);
			while(len > 0 && (string[len-1] == '\r' || string[len-1] == '\n'))
				string[--len] = '\0';

			if(len > 0)
			{
				if(need_line_feed)
					Local_Printf(cc,"\n");
				else
					Local_Printf(cc," ");

				Local_Printf(cc,"[%s]",string);
			}

			Local_Printf(cc,"\n");
		}
	}

	result = RETURN_OK;

	if(FLAG_IS_SET(system_status,SBSYSSTAT_Interfaces))
	{
		if(NOT args.Quiet && args.Check == NULL)
			Local_Printf(cc,get_str(cc,MSG_GETNETSTATUS_INTERFACES_AVAILABLE_AND_CONFIGURED_TXT));
	}
	else
	{
		if(NOT args.Quiet && (args.Check == NULL || check_interfaces))
			Local_Printf(cc,get_str(cc,MSG_GETNETSTATUS_NO_INTERFACES_AVAILABLE_AND_CONFIGURED_TXT),begin_bold(cc),end_bold(cc));

		if(check_interfaces)
			result = RETURN_WARN;
	}

	if(FLAG_IS_SET(system_status,SBSYSSTAT_PTP_Interfaces))
	{
		if(NOT args.Quiet && args.Check == NULL)
			Local_Printf(cc,get_str(cc,MSG_GETNETSTATUS_POINT_TO_POINT_INTERFACES_AVAILABLE_AND_CONFIGURED_TXT));
	}
	else
	{
		if(NOT args.Quiet && (args.Check == NULL || check_ptp_interfaces))
			Local_Printf(cc,get_str(cc,MSG_GETNETSTATUS_NO_POINT_TO_POINT_INTERFACES_AVAILABLE_AND_CONFIGURED_TXT),begin_bold(cc),end_bold(cc));

		if(check_ptp_interfaces)
			result = RETURN_WARN;
	}

	if(FLAG_IS_SET(system_status,SBSYSSTAT_BCast_Interfaces))
	{
		if(NOT args.Quiet && args.Check == NULL)
			Local_Printf(cc,get_str(cc,MSG_GETNETSTATUS_BROADCAST_INTERFACES_AVAILABLE_AND_CONFIGURED_TXT));
	}
	else
	{
		if(NOT args.Quiet && (args.Check == NULL || check_broadcast_interfaces))
			Local_Printf(cc,get_str(cc,MSG_GETNETSTATUS_NO_BROADCAST_INTERFACES_AVAILABLE_AND_CONFIGURED_TXT),begin_bold(cc),end_bold(cc));

		if(check_broadcast_interfaces)
			result = RETURN_WARN;
	}

	if(FLAG_IS_SET(system_status,SBSYSSTAT_Resolver))
	{
		if(NOT args.Quiet && args.Check == NULL)
			Local_Printf(cc,get_str(cc,MSG_GETNETSTATUS_DNS_CONFIGURED_TXT));
	}
	else
	{
		if(NOT args.Quiet && (args.Check == NULL || check_resolver))
			Local_Printf(cc,get_str(cc,MSG_GETNETSTATUS_NO_DNS_CONFIGURED_TXT),begin_bold(cc),end_bold(cc));

		if(check_resolver)
			result = RETURN_WARN;
	}

	if(FLAG_IS_SET(system_status,SBSYSSTAT_Routes))
	{
		if(NOT args.Quiet && args.Check == NULL)
			Local_Printf(cc,get_str(cc,MSG_GETNETSTATUS_ROUTE_CONFIGURED_TXT));
	}
	else
	{
		if(NOT args.Quiet && (args.Check == NULL || check_routes))
			Local_Printf(cc,get_str(cc,MSG_GETNETSTATUS_NO_ROUTE_CONFIGURED_TXT),begin_bold(cc),end_bold(cc));

		if(check_routes)
			result = RETURN_WARN;
	}

	if(FLAG_IS_SET(system_status,SBSYSSTAT_DefaultRoute))
	{
		if(NOT args.Quiet && args.Check == NULL)
			Local_Printf(cc,get_str(cc,MSG_GETNETSTATUS_DEFAULT_ROUTE_CONFIGURED_TXT));
	}
	else
	{
		if(NOT args.Quiet && (args.Check == NULL || check_default_route))
			Local_Printf(cc,get_str(cc,MSG_GETNETSTATUS_NO_DEFAULT_ROUTE_CONFIGURED_TXT),begin_bold(cc),end_bold(cc));

		if(check_default_route)
			result = RETURN_WARN;
	}

 out:

	if(rda != NULL)
		FreeArgs(rda);

	return(result);
}

/****************************************************************************/

STATIC STRPTR
begin_bold(struct CommandContext * cc)
{
	DECLARE_DOSBASE(cc);

	STRPTR result;

	if(IsInteractive(Output()))
		result = "\33[1m";
	else
		result = "";

	return(result);
}

STATIC STRPTR
end_bold(struct CommandContext * cc)
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

STATIC STRPTR
get_next_token(
	STRPTR	input,
	STRPTR	string,
	LONG	string_len)
{
	if((*input) == '\0')
	{
		input = NULL;
	}
	else
	{
		BOOL is_escape = FALSE;
		BOOL is_quote = FALSE;
		BOOL done;
		UBYTE c;

		while((*input) == ' ' || (*input) == (UBYTE)'\240' || (*input) == '\t' || (*input) == ',')
			input++;

		done = FALSE;

		do
		{
			c = (*input++);
			if(c == '\0')
			{
				input--;
				break;
			}

			if(is_escape)
			{
				if(string_len > 1)
				{
					(*string++) = c;
					string_len--;
				}

				is_escape = FALSE;
				continue;
			}

			switch(c)
			{
				case '\\':

					is_escape = TRUE;
					break;

				case ' ':
				case '\240':
				case '\t':
				case ',':

					if(is_quote)
					{
						if(string_len > 1)
						{
							(*string++) = c;
							string_len--;
						}
					}
					else
					{
						done = TRUE;
					}

					break;

				case '\"':

					if(is_quote)
					{
						done = TRUE;
					}
					else
					{
						is_quote ^= TRUE;
					}

					break;

				default:

					if(string_len > 1)
					{
						(*string++) = c;
						string_len--;
					}

					break;
			}
		}
		while(NOT done);

		if(string_len > 0)
			(*string) = '\0';
	}

	return(input);
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
