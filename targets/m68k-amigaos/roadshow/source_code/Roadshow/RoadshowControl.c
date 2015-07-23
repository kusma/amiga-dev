/*
 *	$Id: RoadshowControl.c,v 1.10 2010-03-14 10:00:09 obarthel Exp $
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

#include "RoadshowControl_rev.h"

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
STATIC BOOL is_blank_space(UBYTE c);
STATIC BOOL save_settings(struct CommandContext *cc, BOOL quiet, STRPTR name, LONG value);
STATIC VOID get_errno_and_code(struct CommandContext *cc, LONG *errno_ptr, STRPTR *code_ptr);
STATIC LONG VARARGS68K Local_Printf(struct CommandContext *cc, STRPTR format, ...);
STATIC LONG VARARGS68K Local_ErrorPrintf(struct CommandContext *cc, STRPTR format, ...);
STATIC VARARGS68K VOID lsprintf(struct CommandContext *cc, STRPTR buffer, STRPTR formatString, ...);
STATIC BOOL create_path(struct CommandContext *cc, CONST_STRPTR name);
STATIC STRPTR get_builtin_str(LONG id);
STATIC STRPTR get_str(struct CommandContext *cc, LONG id);
STATIC LONG VARARGS68K Local_SocketBaseTags(struct CommandContext *cc, ...);

/****************************************************************************/

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
#define ROADSHOWCONTROL_CATALOG_STRINGS
#include "roadshow.h"

/****************************************************************************/

/****** ROADSHOW/ROADSHOWCONTROL ***********************************************
*
*   NAME
*	RoadshowControl - Display and change internal configuration options
*
*   FORMAT
*	RoadshowControl [SAVE] [QUIET] [GET option] [SET option=value]
*
*   TEMPLATE
*	SAVE/S,QUIET/S,GET/K,SET/K/F
*
*   FUNCTION
*	Several internal configuration options which determine the behaviour
*	of the TCP/IP stack can be changed at run time. This command will
*	display/query option values and can be used to change them, too.
*
*   OPTIONS
*	GET/K
*	    Determine if a named option is supported and print its current
*	    value. If the option does not exist, the command will print
*	    an error message and return with a warning. You can test for
*	    the warning with the "IF" shell command.
*
*	SET/K
*	    Change the value of an option.
*
*	SAVE/S
*	    When combined with the SET option, also save these settings
*	    permanently so that they will be used the next time you start
*	    the TCP/IP stack.
*
*	QUIET/S
*	    If this option is in effect, neither the SET nor the GET options
*	    will print the current value of an option.
*
*	If you do no specify and option, all known options and their values
*	will be printed.
*
*   EXAMPLES
*	Check if the udp.cksum option exists:
*
*	1> RoadshowControl get udp.cksum
*	udp.cksum = 1
*
*	Check if the bpf.bufsize option exists:
*	1> RoadshowControl get bpf.bufsize
*	bpf.bufsize: Object not found
*
*	Change the tcp.use_mssdflt_for_remote option:
*	1> RoadshowControl set tcp.use_mssdflt_for_remote = 1
*	tcp.use_mssdflt_for_remote = 1
*
*   NOTES
*	You really should know what you are doing when you are changing
*	internal configuration options to values which are not the default
*	settings. The wrong choices may render the TCP/IP inoperable!
*
*   OPTIONS
*	Here is a brief list of options that may be supported by your
*	Roadshow installation.
*
*	    bpf.bufsize
*	        The size of the Berkeley Packet Filter buffer.
*
*	    icmp.maskrepl
*	        Controls if the ICMP layer responds to mask requests.
*	        This can be 1 (accept) or 0 (ignore).
*
*	    icmp.processecho
*	        Controls if the ICMP layer responds to echo requests.
*	        This can be 0 (accept), 1 (ignore) or 2 (drop).
*
*	    icmp.procesststamp
*	        Controls if the ICMP layer responds to time stamp requests.
*	        This can be 0 (accept), 1 (ignore) or 2 (drop).
*
*	    ip.defttl
*	        Controls the default time-to-live value of IP packets
*	        generated.
*
*	    ip.forwarding
*	        Controls if IP packets may be forwarded or not.
*	        This can be 1 (forward) or 0 (drop).
*
*	    ip.sendredirects
*	        Controls if ICMP redirect messages may be generated.
*	        This can be 1 (yes) or 0 (no).
*
*	    ip.subnetsarelocal
*	        Controls if the Internet addresses of directly
*	        connected hosts should be considered local, or if
*	        this also applies to hosts on the same subnet.
*	        This can be 1 (subnets are local) or 0 (they are not).
*
*		task.priority.controller
*		    Selects the priority at which the network I/O
*		    controller Task runs. The priority can affect overall
*	        network performance.
*		    This must be in the range -128..127. Default is 0.
*
*	    tcp.do_rfc1323
*	        Controls whether or not the TCP extensions for high
*	        performance (RFC1323) should be enabled or not.
*	        Specifically, this covers round trip time measurement
*	        and the TCP window scale option.
*	        This can be 1 (enable) or 0 (disable).
*
*	    tcp.do_timestamps
*	        Controls whether or not the round trip time measurement
*	        feature should be enabled if the tcp.do_rfc1323 option
*	        is enabled.
*	        This can be 1 (enable) or 0 (disable).
*
*	    tcp.do_win_scale
*	        Controls whether or not the TCP window scale option
*	        should be enabled if the tcp.do_rfc1323 option
*	        is enabled.
*	        This can be 1 (enable) or 0 (disable).
*
*	    tcp.mssdflt
*	        Controls the default TCP maximum segment size value.
*
*	    tcp.recvspace
*	        Controls the size of the default TCP receive buffer.
*
*	    tcp.rttdflt
*	        Controls the default TCP retransmit time value.
*
*	    tcp.sendspace
*	        Controls the size of the default TCP transmit buffer.
*
*	    tcp.use_mssdflt_for_remote
*	        Controls if the TCP protocol should use a smaller
*	        maximum segment size value for packets sent to
*	        hosts which are not in the local network.
*	        This can be 1 (yes) or 0 (no).
*
*	    udp.cksum
*	        Controls if checksums should be calculated over
*	        UDP datagrams to be sent and verified for UDP
*	        datagrams received.
*	        This can be 1 (yes) or 0 (no).
*
*	    udp.recvspace
*	        Controls the size of the default UDP receive buffer.
*
*	    udp.sendspace
*	        Controls the size of the default UDP transmit buffer.
*
*   ENVIRONMENT VARIABLES
*	It is not necessary to change the TCP/IP stack options every time you
*	start it. You can also set up global environment variables with the
*	SetEnv command which the TCP/IP stack will check when it is started.
*
*	The environment variable names correspond to the options listed above.
*	For example, Roadshow/tcp/do_win_scale corresponds to the
*	tcp.do_win_scale option, Roadshow/ip/forwarding corresponds to the
*	ip.forwarding option, etc.
*
*	To change an environment variable, you would enter the following
*	in the shell:
*
*	    SetEnv SAVE Roadshow/ip/forwarding 1
*
*	Note that your shell may not support the SAVE switch, or might have
*	problems with environment variable names longer than 31 letters. In
*	such a case you would have to get by with the MakeDir, Echo and
*	Copy commands, like this:
*
*	    MakeDir ENV:Roadshow
*	    MakeDir ENV:Roadshow/ip
*	    Echo >ENV:Roadshow/ip/forwarding 1
*	    Copy ENV:Roadshow ENVARC:Roadshow ALL QUIET
*
******************************************************************************
*/

STATIC LONG
cmd(struct CommandContext * cc)
{
	struct
	{
		SWITCH	Save;
		SWITCH	Quiet;
		KEY		Get;
		KEY		Set;
	} args;

	STRPTR args_template =
		"SAVE/S,"
		"QUIET/S,"
		"GET/K,"
		"SET/K/F"
		VERSTAG;

	DECLARE_DOSBASE(cc);
	DECLARE_UTILITYBASE(cc);
	DECLARE_SOCKETBASE(cc);

	LONG result = RETURN_FAIL;
	struct RDArgs * rda;
	ULONG have_api = 0;
	struct List * list = NULL;
	struct RoadshowDataNode * rdn;

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
		if(NOT args.Quiet)
		{
			Local_ErrorPrintf(cc,get_str(cc,MSG_ROADSHOWCONTROL_NO_UTILITY_TXT),
				cc->cc_ProgramName);
		}

		goto out;
	}

	if(SocketBase == NULL)
	{
		if(NOT args.Quiet)
		{
			Local_ErrorPrintf(cc,get_str(cc,MSG_ROADSHOWCONTROL_CANNOT_OPEN_BSDSOCKET_TXT),
				cc->cc_ProgramName);
		}

		goto out;
	}

	if(Local_SocketBaseTags(cc,
	    SBTM_GETREF(SBTC_HAVE_ROADSHOWDATA_API),&have_api,
	TAG_END) != 0 || have_api == 0)
	{
		if(NOT args.Quiet)
		{
			Local_ErrorPrintf(cc,get_str(cc,MSG_ROADSHOWCONTROL_BSDSOCKET_HAS_NO_ROADSHOWDATA_API_TXT),
				cc->cc_ProgramName, SocketBase->lib_Node.ln_Name, SocketBase->lib_Version, SocketBase->lib_Revision);
		}

		goto out;
	}

	if(args.Set)
	{
		size_t len,i;
		STRPTR key,value;
		LONG number;

		list = ObtainRoadshowData(ORD_WriteAccess);
		if(list == NULL)
		{
			if(NOT args.Quiet)
			{
				STRPTR error_message;
				LONG errno;

				get_errno_and_code(cc,&errno,&error_message);

				Local_ErrorPrintf(cc,get_str(cc,MSG_ROADSHOWCONTROL_NO_ROADSHOW_DATA_TXT),
					cc->cc_ProgramName, errno, error_message);
			}

			goto out;
		}

		key = args.Set;
		while(is_blank_space(*key))
			key++;

		value = NULL;

		len = strlen(key);

		for(i = 0 ; i < len ; i++)
		{
			if(is_blank_space(key[i]) || key[i] == '=')
			{
				if(value == NULL)
				{
					value = &key[i+1];
					key[i] = '\0';
					break;
				}
			}
		}

		if(value != NULL)
		{
			while(is_blank_space(*value) || (*value) == '=')
				value++;
		}

		if((*key) == '\0' || value == NULL || (*value) == '\0')
		{
			if(NOT args.Quiet)
				PrintFault(ERROR_REQUIRED_ARG_MISSING,cc->cc_ProgramName);

			goto out;
		}

		if(StrToLong(value,&number) < 1)
		{
			if(NOT args.Quiet)
				PrintFault(ERROR_BAD_NUMBER,value);

			goto out;
		}

		if(CANNOT ChangeRoadshowData(list,key,sizeof(number),&number))
		{
			if(NOT args.Quiet)
			{
				STRPTR error_message;
				LONG errno;

				get_errno_and_code(cc,&errno,&error_message);

				Local_ErrorPrintf(cc,get_str(cc,MSG_ROADSHOWCONTROL_COULD_NOT_SET_OPTION_TXT),
					cc->cc_ProgramName, key, number, errno, error_message);
			}

			goto out;
		}

		if(args.Save)
		{
			if(CANNOT save_settings(cc,args.Quiet,key,number))
				goto out;
		}

		if(NOT args.Quiet)
		{
			struct RoadshowDataNode * found = NULL;

			for(rdn = (struct RoadshowDataNode *)list->lh_Head ;
			    rdn->rdn_MinNode.mln_Succ != NULL ;
			    rdn = (struct RoadshowDataNode *)rdn->rdn_MinNode.mln_Succ)
			{
				if(Stricmp(rdn->rdn_Name,key) == SAME)
				{
					found = rdn;
					break;
				}
			}

			if(found != NULL && found->rdn_Type == RDNT_Integer)
				Local_Printf(cc,"%s = %ld\n",found->rdn_Name,*(int *)found->rdn_Data);
		}
	}
	else
	{
		list = ObtainRoadshowData(ORD_ReadAccess);
		if(list == NULL)
		{
			if(NOT args.Quiet)
			{
				STRPTR error_message;
				LONG errno;

				get_errno_and_code(cc,&errno,&error_message);

				Local_ErrorPrintf(cc,get_str(cc,MSG_ROADSHOWCONTROL_NO_ROADSHOW_DATA_TXT),
					cc->cc_ProgramName, errno, error_message);
			}

			goto out;
		}

		if(args.Get)
		{
			struct RoadshowDataNode * found = NULL;

			for(rdn = (struct RoadshowDataNode *)list->lh_Head ;
			    rdn->rdn_MinNode.mln_Succ != NULL ;
			    rdn = (struct RoadshowDataNode *)rdn->rdn_MinNode.mln_Succ)
			{
				if(Stricmp(rdn->rdn_Name,args.Get) == SAME)
				{
					found = rdn;
					break;
				}
			}

			if(found == NULL || found->rdn_Type != RDNT_Integer)
			{
				if(NOT args.Quiet)
					PrintFault(ERROR_OBJECT_NOT_FOUND,args.Get);

				result = RETURN_WARN;
				goto out;
			}

			if(NOT args.Quiet)
				Local_Printf(cc,"%s = %ld\n",found->rdn_Name,*(int *)found->rdn_Data);
		}
		else
		{
			for(rdn = (struct RoadshowDataNode *)list->lh_Head ;
			    rdn->rdn_MinNode.mln_Succ != NULL ;
			    rdn = (struct RoadshowDataNode *)rdn->rdn_MinNode.mln_Succ)
			{
				if(CheckSignal(SIGBREAKF_CTRL_C))
				{
					PrintFault(ERROR_BREAK,cc->cc_ProgramName);

					result = RETURN_WARN;
					goto out;
				}

				if(rdn->rdn_Type != RDNT_Integer)
					continue;

				Local_Printf(cc,"%s = %ld\n",rdn->rdn_Name,*(int *)rdn->rdn_Data);
			}
		}
	}

	result = RETURN_OK;

 out:

	if(list != NULL)
		ReleaseRoadshowData(list);

	if(rda != NULL)
		FreeArgs(rda);

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

STATIC BOOL
save_settings(struct CommandContext * cc,BOOL quiet,STRPTR name,LONG value)
{
	DECLARE_DOSBASE(cc);

	BOOL success = FALSE;
	TEXT variable_name[512];
	TEXT value_str[30];
	BPTR file = ZERO;
	size_t i;

	strcpy(variable_name,"ENVARC:Roadshow/");
	strcat(variable_name,name);

	for(i = 0 ; i < strlen(variable_name) ; i++)
	{
		if(variable_name[i] == '.')
			variable_name[i] = '/';
	}

	lsprintf(cc,(STRPTR)value_str,"%ld\n",value);

	if(CANNOT SetVar(&variable_name[7],value_str,-1,GVF_GLOBAL_ONLY))
	{
		if(NOT quiet)
		{
			TEXT error_string[100];

			Fault(IoErr(),NULL,error_string,sizeof(error_string));

			Local_ErrorPrintf(cc,get_str(cc,MSG_ROADSHOWCONTROL_COULD_NOT_SET_ENV_VARIABLE_TXT),
				cc->cc_ProgramName, &variable_name[7], IoErr(), error_string);
		}

		goto out;
	}

	if(CANNOT create_path(cc,variable_name) ||
	   (file = Open(variable_name,MODE_NEWFILE)) == ZERO ||
	   Write(file,value_str,strlen(value_str)) == -1)
	{
		if(NOT quiet)
		{
			TEXT error_string[100];

			Fault(IoErr(),NULL,error_string,sizeof(error_string));

			Local_ErrorPrintf(cc,get_str(cc,MSG_ROADSHOWCONTROL_COULD_NOT_SET_ENV_VARIABLE_TXT),
				cc->cc_ProgramName, variable_name, IoErr(), error_string);
		}

		goto out;
	}

	success = TRUE;

 out:

	if(file != ZERO)
		Close(file);

	return(success);
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
create_path(struct CommandContext * cc,CONST_STRPTR name)
{
	DECLARE_SYSBASE(cc);
	DECLARE_DOSBASE(cc);

	STRPTR name_copy = NULL;
	BPTR lock = ZERO;
	BOOL result = FALSE;

	/* Don't try to do anything if the name is just a name and does not
	   include any path component. */
	if(FilePart((STRPTR)name) != (STRPTR)name)
	{
		size_t len,i;
		STRPTR tail;

		len = strlen(name);

		name_copy = AllocVec((ULONG)len+1,MEMF_ANY|MEMF_PUBLIC);
		if(name_copy == NULL)
			goto out;

		memcpy(name_copy,name,len+1);

		/* Isolate the path name component. */
		tail = PathPart(name_copy);
		(*tail) = '\0';

		/* Now figure out if there is any path to be created. */
		len = strlen(name_copy);
		if(len == 0 || name_copy[len-1] == ':')
		{
			result = TRUE;
			goto out;
		}

		/* Now rattle down the path, checking if all the components are
		   present. If one is missing, create it and move on. */
		for(i = 0 ; i <= len ; i++)
		{
			if(name_copy[i] == '/' || name_copy[i] == '\0')
			{
				TEXT c;

				/* Cut off anything to follow this part of the
				   directory path. */
				c = name_copy[i];
				name_copy[i] = '\0';

				/* Does the path exist? */
				lock = Lock(name_copy,SHARED_LOCK);
				if(lock == ZERO)
				{
					/* It doesn't. But is this because a directory is missing? */
					if(IoErr() != ERROR_OBJECT_NOT_FOUND)
						goto out;

					/* Try to create the missing path part. */
					lock = CreateDir(name_copy);
					if(lock == ZERO)
						goto out;
				}

				name_copy[i] = c;

				UnLock(lock);
				lock = ZERO;
			}
		}
	}

	result = TRUE;

 out:

	UnLock(lock);

	FreeVec(name_copy);

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
