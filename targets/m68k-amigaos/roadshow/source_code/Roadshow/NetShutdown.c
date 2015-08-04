/*
 *	$Id: NetShutdown.c,v 1.11 2007-08-26 12:30:26 obarthel Exp $
 *
 *	:ts=4
 *
 *	Copyright © 2001-2007 by Olaf Barthel. All Rights Reserved.
 */

#include <libraries/bsdsocket.h>

#include <exec/execbase.h>
#include <exec/memory.h>

#include <devices/timer.h>

#include <dos/dosextens.h>
#include <dos/dostags.h>
#include <dos/dosasl.h>
#include <dos/rdargs.h>
#include <dos/stdio.h>

#include <libraries/locale.h>

/****************************************************************************/

#if defined(__amigaos4__) && !defined(Flush)
#define Flush(fh) FFlush(fh)
#endif /* __amigaos4__ */

/****************************************************************************/

#if !defined(__amigaos4__)
#define NO_INLINE_STDARG
#endif /* __amigaos4__ */

#define __NOLIBBASE__
#define __NOGLOBALIFACE__
#define __USE_INLINE__

/****************************************************************************/

#include <proto/locale.h>
#include <proto/exec.h>
#include <proto/dos.h>

/****************************************************************************/

#include <string.h>
#include <stdarg.h>

/****************************************************************************/

#include "macros.h"

/****************************************************************************/

#include "NetShutdown_rev.h"

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

	#if defined(__amigaos4__)

	/************************************************************************/

	struct ExecIFace *		cc_IExec;
	struct DOSIFace *		cc_IDOS;
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

/****************************************************************************/

#else

/****************************************************************************/

#define DECLARE_SYSBASE(cc) \
	struct Library * SysBase = cc->cc_SysBase

#define DECLARE_DOSBASE(cc) \
	struct Library * DOSBase = cc->cc_DOSBase

#define DECLARE_LOCALEBASE(cc) \
	struct Library * LocaleBase = cc->cc_LocaleBase

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

/****************************************************************************/

/****** ROADSHOW/NETSHUTDOWN **************************************************
*
*   NAME
*	NetShutdown - Attempt to shut down the network in an orderly fashion.
*
*   FORMAT
*	NetShutdown [TIMEOUT=<secs>] [QUIET]
*
*   TEMPLATE
*	TIMEOUT/N,QUIET/S
*
*   FUNCTION
*	The command will stop all running interfaces.
*
*   OPTIONS
*	TIMEOUT/N
*	    How many seconds this command should wait until it gives up. By
*	    default, it will wait up to 5 seconds for the network to shut
*	    down once it has triggered the shutdown process.
*
*	QUIET/S
*	    Use this parameter to stop the command from reporting what it
*	    is currently doing.
*
*   NOTES
*	The "NetShutdown" command will trigger the shutdown process of the
*	network. This process cannot be stopped once it has started. However,
*	this command can make an attempt to wait until the shutdown has
*	completed. Normally, the shutdown should be finished in a fraction of
*	a second, but at times when other clients still hang onto the network
*	resources, the shutdown can fail to complete quite so quickly. In that
*	case, the "NetShutdown" command will tell you that it could not
*	complete its task within the allocated time frame (within five
*	seconds, or whatever timeout you specified). The shutdown, however,
*	will proceed and may conclude at a later time.
*
*	When this command starts up it begins by checking if the network is
*	currently operational. If this is not the case, it will exit
*	immediately, printing a message to this effect.
*
*   SEE ALSO
*	AddNetInterface
*	ShowNetStatus
*******************************************************************************
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
	}
	#endif /* __amigaos4__ */

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

	error = OK;

 out:

	return(error);
}

/****************************************************************************/

#define CATCOMP_ARRAY
#define NETSHUTDOWN_CATALOG_STRINGS
#include "roadshow.h"

/****************************************************************************/

STATIC LONG
cmd(struct CommandContext * cc)
{
	struct
	{
		NUMBER	Timeout;
		SWITCH	Quiet;
	} args;

	STRPTR args_template =
		"TIMEOUT/N,"
		"QUIET/S"
		VERSTAG;

	DECLARE_SYSBASE(cc);
	DECLARE_DOSBASE(cc);

	struct MsgPort * timer_port = NULL;
	struct timerequest * timer_request = NULL;
	BOOL timer_ticking = FALSE;
	LONG result = RETURN_FAIL;
	struct RDArgs * rda;
	struct MsgPort * reply_port = NULL;
	LONG timeout;
	ULONG reply_mask;
	ULONG timer_mask;
	ULONG signals;
	struct NetControlPort * control_port;
	struct NetShutdownMessage * shutdown_message = NULL;
	struct NetShutdownMessage * recall_message = NULL;
	LONG client_count = 0;
	BOOL network_in_use;

	GetProgramName(cc->cc_ProgramName,sizeof(cc->cc_ProgramName));

	memset(&args,0,sizeof(args));

	rda = ReadArgs(args_template,(LONG *)&args,NULL);
	if(rda == NULL)
	{
		PrintFault(IoErr(),cc->cc_ProgramName);
		goto out;
	}

	/* We wait for up to 5 seconds for the network to shut down. */
	timeout = 5;

	if(args.Timeout != NULL)
	{
		/* The user can override the timeout, but the minimum wait time
		   is exactly one second. */
		timeout = (*args.Timeout);
		if(timeout < 1)
		{
			if(NOT args.Quiet)
				PrintFault(ERROR_BAD_NUMBER,cc->cc_ProgramName);

			goto out;
		}
	}

	shutdown_message	= AllocVec(sizeof(*shutdown_message),MEMF_ANY|MEMF_PUBLIC|MEMF_CLEAR);
	recall_message		= AllocVec(sizeof(*shutdown_message),MEMF_ANY|MEMF_PUBLIC|MEMF_CLEAR);

	if(shutdown_message == NULL || recall_message == NULL)
	{
		if(NOT args.Quiet)
		{
			Local_ErrorPrintf(cc,get_str(cc,MSG_NETSHUTDOWN_NO_MEMORY_TXT),
				cc->cc_ProgramName);
		}

		goto out;
	}

	reply_port = CreateMsgPort();
	if(reply_port == NULL)
	{
		if(NOT args.Quiet)
		{
			Local_ErrorPrintf(cc,get_str(cc,MSG_NETSHUTDOWN_NO_MSGPORT_TXT),
				cc->cc_ProgramName);
		}

		goto out;
	}

	shutdown_message->nsm_Message.mn_ReplyPort		= reply_port;
	shutdown_message->nsm_Message.mn_Length			= sizeof(*shutdown_message);
	shutdown_message->nsm_Command					= NSMC_Shutdown;
	shutdown_message->nsm_Data						= &client_count;
	shutdown_message->nsm_Length					= sizeof(client_count);

	recall_message->nsm_Message.mn_ReplyPort		= reply_port;
	recall_message->nsm_Message.mn_Length			= sizeof(*recall_message);
	recall_message->nsm_Command						= NSMC_Cancel;
	recall_message->nsm_Data						= shutdown_message;
	recall_message->nsm_Length						= sizeof(shutdown_message);

	timer_port = CreateMsgPort();
	if(timer_port == NULL)
	{
		if(NOT args.Quiet)
		{
			Local_ErrorPrintf(cc,get_str(cc,MSG_NETSHUTDOWN_NO_MSGPORT_TXT),
				cc->cc_ProgramName);
		}

		goto out;
	}

	timer_request = (struct timerequest *)CreateIORequest(timer_port,sizeof(*timer_request));
	if(timer_request == NULL)
	{
		if(NOT args.Quiet)
		{
			Local_ErrorPrintf(cc,get_str(cc,MSG_NETSHUTDOWN_NO_TIMER_REQUEST_TXT),
				cc->cc_ProgramName);
		}

		goto out;
	}

	if(OpenDevice(TIMERNAME,UNIT_VBLANK,(struct IORequest *)timer_request,0) != OK)
	{
		if(NOT args.Quiet)
		{
			Local_ErrorPrintf(cc,get_str(cc,MSG_NETSHUTDOWN_NO_TIMER_DEVICE_TXT),
				cc->cc_ProgramName,TIMERNAME);
		}

		goto out;
	}

	/* Try to send the shutdown message. */
	Forbid();

	control_port = (struct NetControlPort *)FindPort(NETWORK_CONTROLLER_PORT_NAME);

	if(control_port != NULL && control_port->ncp_Magic == NCPM_Cookie)
	{
		network_in_use = TRUE;

		PutMsg(&control_port->ncp_Port,(struct Message *)shutdown_message);
	}
	else
	{
		network_in_use = FALSE;
	}

	Permit();

	if(NOT network_in_use)
	{
		if(NOT args.Quiet)
			Local_ErrorPrintf(cc,get_str(cc,MSG_NETSHUTDOWN_NETWORK_NOT_IN_USE_TXT),cc->cc_ProgramName);

		result = RETURN_WARN;
		goto out;
	}

	reply_mask	= (1UL << reply_port->mp_SigBit);
	timer_mask	= (1UL << timer_port->mp_SigBit);

	/* Get the timeout rolling... */
	timer_request->tr_node.io_Command	= TR_ADDREQUEST;
	timer_request->tr_time.tv_secs		= timeout;
	timer_request->tr_time.tv_micro		= 0;

	SendIO((struct IORequest *)timer_request);
	timer_ticking = TRUE;

	if(NOT args.Quiet)
	{
		Local_Printf(cc,get_str(cc,MSG_NETSHUTDOWN_TRYING_TO_SHUT_DOWN_TXT));
		Flush(Output());
	}

	/* Wait for one of three events: the 'all clear' signal from the network
	   controller, the user not wanting to wait any longer and the timer to
	   elapse. */
	signals = Wait(SIGBREAKF_CTRL_C | reply_mask | timer_mask);

	/* Did we get the 'all clear' signal? */
	if(signals & reply_mask)
	{
		struct ExecBase * ExecBase = (struct ExecBase *)SysBase;
		struct Library * lib;

		/* Pick up the message that is waiting. */
		GetMsg(reply_port);

		/* Flush the library from memory. */
		Forbid();

		lib = (struct Library *)FindName(&ExecBase->LibList,"bsdsocket.library");
		if(lib != NULL)
			RemLibrary(lib);

		Permit();

		if(NOT args.Quiet)
			Local_Printf(cc,get_str(cc,MSG_NETSHUTDOWN_SHUTDOWN_DONE_TXT));

		result = RETURN_OK;
	}
	else
	{
		/* OK, so we need to recall the shutdown message before we bow out. */

		Forbid();

		/* Look for the controller port again since it may have gone away
		   while we were printing messages to the console, etc. above. */
		control_port = (struct NetControlPort *)FindPort(NETWORK_CONTROLLER_PORT_NAME);
		if(control_port != NULL && control_port->ncp_Magic != NCPM_Cookie)
			PutMsg(&control_port->ncp_Port,(struct Message *)recall_message);

		Permit();

		/* Wait for the two messages to return. This is indicated by the
		   message node types changing from NT_MESSAGE to NT_REPLYMSG, or
		   the message type never even changing to NT_MESSAGE (if the
		   message is never sent). */
		while(shutdown_message->nsm_Message.mn_Node.ln_Type	== NT_MESSAGE ||
		      recall_message->nsm_Message.mn_Node.ln_Type	== NT_MESSAGE)
		{
			WaitPort(reply_port);

			while(TRUE)
			{
				if(GetMsg(reply_port) == NULL)
					break;
			}
		}

		result = RETURN_WARN;

		if(NOT args.Quiet)
		{
			LONG message_code;

			if(signals & SIGBREAKF_CTRL_C)
				message_code = MSG_NETSHUTDOWN_SHUTDOWN_ABORTED_TXT;
			else
				message_code = MSG_NETSHUTDOWN_SHUTDOWN_TIMEOUT_TXT;

			Local_Printf(cc,get_str(cc,message_code));
		}
	}

 out:

	if(timer_request != NULL)
	{
		if(timer_ticking)
		{
			if(CheckIO((struct IORequest *)timer_request) == NULL)
				AbortIO((struct IORequest *)timer_request);

			WaitIO((struct IORequest *)timer_request);
		}

		if(timer_request->tr_node.io_Device != NULL)
			CloseDevice((struct IORequest *)timer_request);

		DeleteIORequest((struct IORequest *)timer_request);
	}

	DeleteMsgPort(timer_port);

	DeleteMsgPort(reply_port);

	FreeVec(shutdown_message);
	FreeVec(recall_message);

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
