/*
 *	$Id: ppp_connector.c,v 1.9 2005/06/30 07:44:50 obarthel Exp $
 *
 *	:ts=4
 *
 *	Copyright © 2001-2006 by Olaf Barthel. All Rights Reserved.
 */

/****************************************************************************/

#include <dos/dosextens.h>
#include <dos/dosasl.h>
#include <dos/dostags.h>
#include <dos/rdargs.h>
#include <dos/stdio.h>

#include <exec/memory.h>

#include <devices/sana2.h>

#include <libraries/bsdsocket.h>

/*****************************************************************************/

#include <clib/alib_protos.h>

/*****************************************************************************/

#define __NOLIBBASE__
#define __NOGLOBALIFACE__
#define __USE_INLINE__

/*****************************************************************************/

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/bsdsocket.h>
#include <proto/utility.h>

/*****************************************************************************/

#include <string.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>

/****************************************************************************/

#include "ppp_connector_rev.h"
STRPTR Version = VERSTAG;

/****************************************************************************/

#include "macros.h"

/****************************************************************************/

/*#define DEBUG*/
#include "assert.h"

/****************************************************************************/

#define BUSY ((struct IORequest *)NULL)

/****************************************************************************/

#define PPP_DEFAULT_MTU 1500

/****************************************************************************/

#ifdef __GNUC__
 #ifdef __PPC__
  #pragma pack(2)
 #endif
#elif defined(__VBCC__)
 #pragma amiga-align
#endif

/****************************************************************************/

/* From the NewStyleDevices specification. */
#define NSCMD_DEVICEQUERY 0x4000

struct NSDeviceQueryResult
{
	/* Standard information */
	ULONG	DevQueryFormat;			/* this is type 0 */
	ULONG	SizeAvailable;			/* bytes available */

	/* Common information (READ ONLY!) */
	UWORD	DeviceType;				/* what the device does */
	UWORD	DeviceSubType;			/* depends on the main type */
	UWORD *	SupportedCommands;		/* 0 terminated list of cmd's */

	/* May be extended in the future! Check SizeAvailable! */
};

#define NSDEVTYPE_SANA2 7 /* A >=SANA2R2 networking device */
#define NSDEVTYPE_SERIAL 11 /* like serial.device */

/****************************************************************************/

#ifdef __GNUC__
 #ifdef __PPC__
  #pragma pack()
 #endif
#elif defined(__VBCC__)
 #pragma default-align
#endif

/****************************************************************************/

extern struct Library *	SysBase;
extern struct Library *	DOSBase;

/****************************************************************************/

struct Library *	UtilityBase;
struct Library * 	SocketBase;

/****************************************************************************/

#if defined(__amigaos4__)

/****************************************************************************/

extern struct ExecIFace *	IExec;
extern struct DOSIFace *	IDOS;

/****************************************************************************/

struct UtilityIFace *	IUtility;
struct SocketIFace *	ISocket;

/****************************************************************************/

#endif /* __amigaos4__ */

/****************************************************************************/

struct MsgPort *	NetPort;
struct IOSana2Req *	NetRequest;
struct MsgPort *	NetEventPort;
struct IOSana2Req *	NetEventRequest;
BOOL				NetEventRequestPending;

/****************************************************************************/

struct MsgPort * 	HookPort;

/****************************************************************************/

/* Connection state information to be passed to the device driver. */
struct Sana2Connection Sana2Connection;

/****************************************************************************/

/* Global login and password; these two are not part of the
 * option lists passed to the device driver.
 */
STRPTR Login;
STRPTR Password;

/****************************************************************************/

#if defined(__GNUC__) && !defined(__amigaos4__)

#if defined(Printf)
#undef Printf
#endif /* Printf */

#define Printf Local_Printf

STATIC LONG
Local_Printf(STRPTR format,...)
{
	va_list args;
	LONG result;

	va_start(args,format);
	result = VPrintf(format,args);
	va_end(args);

	return(result);
}

#endif /* __GNUC__ */

/****************************************************************************/

/* Return a descriptive text for an I/O error code returned by a
 * SANA-II device.
 */
STRPTR
get_io_error_string(LONG error,UBYTE * buffer)
{
	STATIC struct { LONG val; STRPTR str; } tab[] =
	{
		{ IOERR_OPENFAIL,		"Device/unit failed to open" },
		{ IOERR_ABORTED,		"Request terminated early" },
		{ IOERR_NOCMD,			"Command not supported by device" },
		{ IOERR_BADLENGTH,		"Not a valid length" },
		{ IOERR_BADADDRESS,		"Invalid address" },
		{ IOERR_UNITBUSY,		"Device opens OK, but requested unit is busy" },
		{ IOERR_SELFTEST,		"Hardware failed self-test" },

		{ S2ERR_NO_RESOURCES,	"Resource allocation failure" },
		{ S2ERR_BAD_ARGUMENT,	"Bad argument" },
		{ S2ERR_BAD_STATE,		"Inappropriate state" },
		{ S2ERR_BAD_ADDRESS,	"Bad address" },
		{ S2ERR_MTU_EXCEEDED,	"Maximum transmission unit exceeded" },
		{ S2ERR_NOT_SUPPORTED,	"Command not supported by hardware" },
		{ S2ERR_SOFTWARE,		"Software error detected" },
		{ S2ERR_OUTOFSERVICE,	"Driver is offline" },
		{ S2ERR_TX_FAILURE,		"Transmission attempt failed" }
	};

	LONG i,n;

	n = -1;
	for(i = 0 ; i < (LONG)NUM_ENTRIES(tab) ; i++)
	{
		if(tab[i].val == error)
		{
			n = i;
			break;
		}
	}

	if(n != -1)
	{
		strcpy(buffer,tab[n].str);
	}
	else
	{
		if(error < 0)
			sprintf(buffer,"Unknown I/O error %ld",error);
		else
			sprintf(buffer,"Unknown SANA-II error %ld",error);
	}

	return(buffer);
}

/* Return a descriptive text for an wire error code returned
 * by a SANA-II device.
 */
STRPTR
get_wire_error_string(LONG error,UBYTE * buffer)
{
	STATIC struct { LONG val; STRPTR str; } tab[] =
	{
		{ S2WERR_GENERIC_ERROR,			"No specific information available" },
		{ S2WERR_NOT_CONFIGURED,		"Unit is not configured" },
		{ S2WERR_UNIT_ONLINE,			"Unit is currently online" },
		{ S2WERR_UNIT_OFFLINE,			"Unit is currently offline" },
		{ S2WERR_ALREADY_TRACKED,		"Protocol is already tracked" },
		{ S2WERR_NOT_TRACKED,			"Protocol is not tracked" },
		{ S2WERR_BUFF_ERROR,			"Buffer management function returned an error" },
		{ S2WERR_SRC_ADDRESS,			"Source address problem" },
		{ S2WERR_DST_ADDRESS,			"Destination address problem" },
		{ S2WERR_BAD_BROADCAST,			"Broadcast address problem" },
		{ S2WERR_BAD_MULTICAST,			"Multicast address problem" },
		{ S2WERR_MULTICAST_FULL,		"Multicast address list is full" },
		{ S2WERR_BAD_EVENT,				"Unsupported event class" },
		{ S2WERR_BAD_STATDATA,			"StatData failed sanity check" },
		{ S2WERR_IS_CONFIGURED,			"Attempted to configure twice" },
		{ S2WERR_NULL_POINTER,			"NULL pointer detected" },
		{ S2WERR_TOO_MANY_RETRIES,		"Transmission failed - too many retries" },
		{ S2WERR_RCVREL_HDW_ERR,		"Driver fixable hardware error" },
		{ S2WERR_UNIT_DISCONNECTED,		"Unit currently not connected" },
		{ S2WERR_UNIT_CONNECTED,		"Unit currently connected" },
		{ S2WERR_INVALID_OPTION,		"Invalid option rejected" },
		{ S2WERR_MISSING_OPTION,		"Mandatory option missing" },
		{ S2WERR_AUTHENTICATION_FAILED,	"Could not log in" }
	};

	LONG i,n;

	n = -1;
	for(i = 0 ; i < (LONG)NUM_ENTRIES(tab) ; i++)
	{
		if(tab[i].val == error)
		{
			n = i;
			break;
		}
	}

	if(n != -1)
		strcpy(buffer,tab[n].str);
	else
		sprintf(buffer,"Unknown wire error %ld",error);

	return(buffer);
}

/****************************************************************************/

/* Try to find an option value by name, then extract it. This must be a
 * string.
 */
BOOL
find_s2c_option_str(struct Sana2Connection * s2c,STRPTR name,STRPTR * value_ptr)
{
	struct Sana2ConnectionOption * s2co;
	BOOL result = FALSE;

	ASSERT( s2c != NULL && name != NULL && value_ptr != NULL );

	for(s2co = (struct Sana2ConnectionOption *)s2c->s2c_Options.mlh_Head ;
	    s2co->s2co_MinNode.mln_Succ != NULL ;
	    s2co = (struct Sana2ConnectionOption *)s2co->s2co_MinNode.mln_Succ)
	{
		if(s2co->s2co_Value != NULL && s2co->s2co_Name != NULL && Stricmp(name,s2co->s2co_Name) == SAME)
		{
			(*value_ptr) = s2co->s2co_Value;

			result = TRUE;

			break;
		}
	}

	return(result);
}

/* Try to find an option value by name, then extract it. This must be
 * a number.
 */
BOOL
find_s2c_option_num(struct Sana2Connection * s2c,STRPTR name,LONG * value_ptr)
{
	struct Sana2ConnectionOption * s2co;
	BOOL result = FALSE;

	ASSERT( s2c != NULL && name != NULL && value_ptr != NULL );

	for(s2co = (struct Sana2ConnectionOption *)s2c->s2c_Options.mlh_Head ;
	    s2co->s2co_MinNode.mln_Succ != NULL ;
	    s2co = (struct Sana2ConnectionOption *)s2co->s2co_MinNode.mln_Succ)
	{
		if(s2co->s2co_Value != NULL && s2co->s2co_Name != NULL && Stricmp(name,s2co->s2co_Name) == SAME)
		{
			LONG n;

			if(StrToLong(s2co->s2co_Value,&n) > 0)
			{
				(*value_ptr) = n;
				result = TRUE;
			}

			break;
		}
	}

	return(result);
}

/****************************************************************************/

void
cleanup(void)
{
	struct Node * node;

	if(Login != NULL)
	{
		FreeVec(Login);
		Login = NULL;
	}

	if(Password != NULL)
	{
		FreeVec(Password);
		Password = NULL;
	}

	while((node = RemHead((struct List *)&Sana2Connection.s2c_Options)) != NULL)
		FreeVec(node);

	#if defined(__amigaos4__)
	{
		if(IUtility != NULL)
		{
			DropInterface((struct Interface *)IUtility);
			IUtility = NULL;
		}
	}
	#endif /* __amigaos4__ */

	if(UtilityBase != NULL)
	{
		CloseLibrary(UtilityBase);
		UtilityBase = NULL;
	}

	#if defined(__amigaos4__)
	{
		if(ISocket != NULL)
		{
			DropInterface((struct Interface *)ISocket);
			ISocket = NULL;
		}
	}
	#endif /* __amigaos4__ */

	if(SocketBase != NULL)
	{
		CloseLibrary(SocketBase);
		SocketBase = NULL;
	}

	if(NetEventRequest != NULL)
	{
		if(NetEventRequestPending)
		{
			if(CheckIO((struct IORequest *)NetEventRequest) == BUSY)
				AbortIO((struct IORequest *)NetEventRequest);

			WaitIO((struct IORequest *)NetEventRequest);

			NetEventRequestPending = FALSE;
		}

		DeleteIORequest((struct IORequest *)NetEventRequest);
		NetEventRequest = NULL;
	}

	if(NetRequest != NULL)
	{
		if(NetRequest->ios2_Req.io_Device != NULL)
			CloseDevice((struct IORequest *)NetRequest);

		DeleteIORequest((struct IORequest *)NetRequest);
		NetRequest = NULL;
	}

	if(NetPort != NULL)
	{
		DeleteMsgPort(NetPort);
		NetPort = NULL;
	}

	if(NetEventPort != NULL)
	{
		DeleteMsgPort(NetEventPort);
		NetEventPort = NULL;
	}

	if(HookPort != NULL)
	{
		DeleteMsgPort(HookPort);
		HookPort = NULL;
	}
}

/****************************************************************************/

/* This opens and initializes all the basic resources this program needs.
 * The cleanup() routine is eventually invoked to release the resources
 * again.
 */
BOOL
setup(void)
{
	LONG have_interface_api = FALSE;
	LONG have_routing_api = FALSE;
	BOOL success = FALSE;

	NewList((struct List *)&Sana2Connection.s2c_Options);

	UtilityBase = OpenLibrary("utility.library",37);
	if(UtilityBase == NULL)
	{
		Printf("ERROR: Could not open 'utility.library' V37.\n");
		goto out;
	}

	#if defined(__amigaos4__)
	{
		IUtility = (struct UtilityIFace *)GetInterface(UtilityBase, "main", 1, NULL);
		if(IUtility == NULL)
		{
			Printf("ERROR: Could not obtain 'utility.library' interface.\n");
			goto out;
		}
	}
	#endif /* __amigaos4__ */

	SocketBase = OpenLibrary("bsdsocket.library",4);
	if(SocketBase == NULL)
	{
		Printf("ERROR: Could not open 'bsdsocket.library' V4.\n");
		goto out;
	}

	#if defined(__amigaos4__)
	{
		ISocket = (struct SocketIFace *)GetInterface(SocketBase, "main", 1, NULL);
		if(ISocket == NULL)
		{
			Printf("ERROR: Could not obtain 'bsdsocket.library' interface.\n");
			goto out;
		}
	}
	#endif /* __amigaos4__ */

	if(SocketBaseTags(
	    SBTM_GETREF(SBTC_HAVE_INTERFACE_API),&have_interface_api,
	TAG_END) != 0)
	{
		have_interface_api = FALSE;
	}

	if(NOT have_interface_api)
	{
		Printf("ERROR: '%s' V%ld.%ld does not support the interface query method used by this program.\n",
			SocketBase->lib_Node.ln_Name, SocketBase->lib_Version, SocketBase->lib_Revision);

		goto out;
	}

	if(SocketBaseTags(
	    SBTM_GETREF(SBTC_HAVE_ROUTING_API),&have_routing_api,
	TAG_END) != 0)
	{
		have_routing_api = FALSE;
	}

	if(NOT have_routing_api)
	{
		Printf("ERROR: '%s' V%ld.%ld does not support the route configuration method used by this program.\n",
			SocketBase->lib_Node.ln_Name, SocketBase->lib_Version, SocketBase->lib_Revision);

		goto out;
	}

	NetPort = CreateMsgPort();
	if(NetPort == NULL)
	{
		Printf("ERROR: Could not create net port.\n");
		goto out;
	}

	NetRequest = (struct IOSana2Req *)CreateIORequest(NetPort,sizeof(*NetRequest));
	if(NetRequest == NULL)
	{
		Printf("ERROR: Could not create net request.\n");
		goto out;
	}

	NetEventPort = CreateMsgPort();
	if(NetEventPort == NULL)
	{
		Printf("ERROR: Could not create net event port.\n");
		goto out;
	}

	NetEventRequest = (struct IOSana2Req *)CreateIORequest(NetEventPort,sizeof(*NetEventRequest));
	if(NetEventRequest == NULL)
	{
		Printf("ERROR: Could not create net event request.\n");
		goto out;
	}

	HookPort = CreateMsgPort();
	if(HookPort == NULL)
	{
		Printf("ERROR: Could not create hook port.\n");
		goto out;
	}

	success = TRUE;

 out:

	return(success);
}

/****************************************************************************/

/* Compare a string against two sets of keywords for ON/OFF; returns
 * either TRUE, FALSE or -1 for "not understood".
 */
LONG
match_key(STRPTR what)
{
	LONG result;

	ENTER();

	ASSERT( what != NULL );

	if(Stricmp(what,"YES") == SAME ||
	   Stricmp(what,"ON") == SAME)
	{
		result = TRUE;
	}
	else if (Stricmp(what,"NO") == SAME ||
	         Stricmp(what,"OFF") == SAME)
	{
		result = FALSE;
	}
	else
	{
		result = -1;
	}

	RETURN(result);
	return(result);
}

/****************************************************************************/

/* This is the special message the hook callback functions set up
 * and exchange with the main program.
 */
struct HookMessage
{
	struct Message hm_Message;
};

/****************************************************************************/

/* This function is invoked whenever an error message is to be displayed. */
VOID ASM
ErrorHookFunc(
	REG(a0,struct Hook *	unused_hook),
	REG(a2,APTR				unused_reserved),
	REG(a1,STRPTR			message))
{
	struct HookMessage * hm;

	ENTER();

	ASSERT( message != NULL );

	SHOWSTRING(message);

	/* Allocate memory for the hook message and the error text, then
	 * copy the error text and send it to the main program.
	 */
	hm = AllocVec(sizeof(*hm) + strlen(message) + 1,MEMF_ANY|MEMF_PUBLIC);
	if(hm != NULL)
	{
		memset(hm,0,sizeof(*hm));

		/* The error message goes into the message's node name. */
		hm->hm_Message.mn_Node.ln_Name	= (char *)(hm + 1);
		hm->hm_Message.mn_Length		= sizeof(*hm) + strlen(message) + 1;

		strcpy(hm->hm_Message.mn_Node.ln_Name,message);

		PutMsg(HookPort,(struct Message *)hm);
	}

	LEAVE();
}

/****************************************************************************/

/* This function is invoked when the link layer has been opened, but further
 * initializations are necessary to bring it online. Nothing happens here
 * because the PPPoE driver does these things all by itself.
 */
LONG ASM
ConnectHookFunc(
	REG(a0,struct Hook *					unused_hook),
	REG(a2,APTR								unused_reserved),
	REG(a1,struct Sana2ConnectionMessage *	unused_s2cm))
{
	BOOL success = TRUE;

	ENTER();

	RETURN((LONG)success);
	return((LONG)success);
}

/****************************************************************************/

/* This function is invoked when the link layer is to be shut down and
 * further work may be required to do this. Nothing happens here because
 * the PPPoE driver does these things all by itself.
 */
VOID ASM
DisconnectHookFunc(
	REG(a0,struct Hook *					unused_hook),
	REG(a2,APTR								unused_reserved),
	REG(a1,struct Sana2ConnectionMessage *	unused_s2cm))
{
	ENTER();

	LEAVE();
}

/****************************************************************************/

/* Check whether "cp" is a valid ASCII representation
 * of an Internet address and convert to a binary address.
 * Returns TRUE if the address is valid, FALSE if not.
 *
 * This replaces inet_addr, the return value from which
 * cannot distinguish between failure and a local broadcast address.
 *
 * Derived from BSD code.
 */
BOOL
convert_string_to_ip_address(STRPTR cp)
{
	LONG parts[4];
	LONG *pp = parts;
	LONG val = 0;
	LONG bits = 0;
	LONG base;
	UBYTE c;

	ASSERT( cp != NULL );

	while(TRUE)
	{
		/*
		 * Collect number up to ``.''.
		 * Values are specified as for C:
		 * 0x=hex, 0=octal, other=decimal.
		 */
		val = 0;
		base = 10;

		if((*cp) == '0')
		{
			cp++;

			if((*cp) == 'x' || (*cp) == 'X')
			{
				base = 16;
				cp++;
			}
			else
			{
				base = 8;
			}
		}

		while((c = (*cp)) != '\0')
		{
			if(base == 8)
			{
				if('0' <= c && c <= '7')
				{
					bits += 4;
					if(bits > 32)
						return(FALSE);

					val = (val * base) + (c & 15);
					cp++;
				}
				else
				{
					break;
				}
			}
			else if ('0' <= c && c <= '9')
			{
				bits += 4;
				if(bits > 32)
					return(FALSE);

				val = (val * base) + (c & 15);
				cp++;
			}
			else if (base == 16 && (('a' <= c && c <= 'f') || ('A' <= c && c <= 'F')))
			{
				bits += 4;
				if(bits > 32)
					return(FALSE);

				val = (val * base) + (c & 15) + 9;
				cp++;
			}
			else
			{
				break;
			}
		}

		if((*cp) == '.')
		{
			/*
			 * Internet format:
			 *	a.b.c.d
			 *	a.b.c	(with c treated as 16-bits)
			 *	a.b	(with b treated as 24 bits)
			 */
			if(pp >= parts + 3 || val > 0xFF)
				return(FALSE);

			(*pp++) = val;
			cp++;

			bits = 0;
		}
		else
		{
			break;
		}
	}

	/*
	 * Check for trailing characters.
	 */
	if((*cp) != '\0')
		return(FALSE);

	/*
	 * Concoct the address according to
	 * the number of parts specified.
	 */
	switch(pp - parts + 1)
	{
		case 1:	/* a -- 32 bits */

			break;

		case 2:	/* a.b -- 8.24 bits */

			if(val > 0xFFFFFF)
				return(FALSE);

			break;

		case 3:	/* a.b.c -- 8.8.16 bits */

			if(val > 0xFFFF)
				return(FALSE);

			break;

		case 4:	/* a.b.c.d -- 8.8.8.8 bits */

			if(val > 0xFF)
				return(FALSE);

			break;

		default:

			return(FALSE);
	}

	return(TRUE);
}

/****************************************************************************/

/* Allocate memory for another option node, then attach
 * that node to the option list. The option value must
 * be a string.
 */
BOOL
add_option_str(STRPTR name,STRPTR value)
{
	struct Sana2ConnectionOption * s2co;
	BOOL result = FALSE;

	ASSERT( name != NULL && value != NULL );

	SHOWSTRING(name);
	SHOWSTRING(value);

	s2co = AllocVec(sizeof(*s2co) + strlen(name)+1 + strlen(value)+1,MEMF_ANY|MEMF_PUBLIC);
	if(s2co == NULL)
		goto out;

	memset(s2co,0,sizeof(*s2co));

	s2co->s2co_Name = (STRPTR)(s2co + 1);
	strcpy(s2co->s2co_Name,name);

	s2co->s2co_Value = s2co->s2co_Name + strlen(s2co->s2co_Name) + 1;
	strcpy(s2co->s2co_Value,value);

	AddTail((struct List *)&Sana2Connection.s2c_Options,(struct Node *)s2co);

	result = TRUE;

 out:

	return(result);
}

/* A front-end for add_option_str() that takes a number as its
 * parameter, encodes it as a string and then drops straight
 * into add_option_str().
 */
BOOL
add_option_num(STRPTR name,LONG num)
{
	UBYTE value[20];
	BOOL result;

	ASSERT( name != NULL );

	sprintf(value,"%ld",num);

	result = add_option_str(name,value);

	return(result);
}

/****************************************************************************/

typedef STRPTR	KEY;
typedef LONG	SWITCH;
typedef LONG *	NUMBER;

/****************************************************************************/

/* This routine reads the configuration file and eventually opens the
 * underlying SANA-II device driver.
 */
BOOL
configure_connection(STRPTR file_name,STRPTR interface_name,LONG interface_name_len)
{
	struct
	{
		KEY		Interface;
		KEY		Device;
		NUMBER	Unit;
		KEY		Raw;
		KEY		Bypass;
		NUMBER	NumReadPackets;
		NUMBER	NumWritePackets;
		KEY		Service;
		KEY		AccessConcentrator;

		KEY		LocalAddress;
		KEY		RemoteAddress;
		KEY		DNS1Address;
		KEY		DNS2Address;
		NUMBER	MaxFail;
		NUMBER	MaxTerm;
		NUMBER	MaxConfig;
		NUMBER	Timeout;
		NUMBER	MaxReconfigure;
		NUMBER	MTU;
		NUMBER	IdleTimeout;
		NUMBER	PeerIdleTimeout;
		KEY		SetEnv;
		KEY		SendID;
		KEY		RejectPAP;
		NUMBER	PAPTimeout;
		NUMBER	PAPRetry;
		KEY		DummyRemoteAddress;

		NUMBER	ConnectTimeout;
		KEY		Login;
		KEY		Password;

		KEY		LogFile;
		KEY		Log;
	} args;

	STRPTR args_template =
		"INTERFACE/K,"
		"DEVICE/K,"
		"UNIT/K/N,"
		"RAW/K,"
		"BYPASS/K,"
		"READPACKETS/K/N,"
		"WRITEPACKETS/K/N,"
		"SERVICE/K,"
		"AC=ACCESSCONCENTRATOR/K,"
		"LOCALADDRESS=LOCALIP/K,"
		"REMOTEADDRESS=REMOTEIP/K,"
		"DNS1ADDRESS=DNS1IP/K,"
		"DNS2ADDRESS=DNS2IP/K,"
		"MAXFAIL/K/N,"
		"MAXTERM/K/N,"
		"MAXCONFIG/K/N,"
		"TIMEOUT/K/N,"
		"MAXRECONFIGURE/K/N,"
		"MTU/K/N,"
		"IDLETIMEOUT/K/N,"
		"PEERIDLETIMEOUT/K/N,"
		"SETENV/K,"
		"SENDID/K,"
		"REJECTPAP/K,"
		"PAPTIMEOUT/K/N,"
		"PAPRETRY/K/N,"
		"DUMMYREMOTEADDRESS/K,"
		"CONNECTTIMEOUT/K/N,"
		"LOGIN/K,"
		"PASSWORD/K,"
		"LOGFILE/K,"
		"LOG/K";

	BOOL success = FALSE;
	BPTR file;
	struct RDArgs * rda = NULL;
	UBYTE * line_buffer = NULL;
	const int line_buffer_size = 1024;
	STRPTR device_name = NULL;
	ULONG device_unit = 0;
	BOOL have_device = FALSE;
	LONG line_number;
	LONG len;
	LONG i;
	UBYTE *s;
	UBYTE *t;
	UBYTE *v;
	LONG separator_index;
	UBYTE separator_char;
	LONG error;

	ENTER();

	ASSERT( file_name != NULL );

	/* Things start by opening the configuration file for reading. */
	file = Open(file_name,MODE_OLDFILE);
	if(file == ZERO)
	{
		UBYTE buffer[100];

		error = IoErr();

		Fault(error,NULL,buffer,sizeof(buffer));

		Printf("ERROR: Could not open '%s' (%s).\n",file_name,buffer);

		goto out;
	}

	SetVBuf(file,NULL,BUF_LINE,1024);

	/* This buffer is used for reading and parsing configuration file
	 * contents.
	 */
	line_buffer = AllocVec(line_buffer_size+2,MEMF_ANY|MEMF_PUBLIC);
	if(line_buffer == NULL)
	{
		Printf("ERROR: Not enough memory for read buffer.\n");

		SHOWMSG("not enough memory for read buffer");
		goto out;
	}

	/* Allocate memory for the line parser. */
	rda = AllocDosObject(DOS_RDARGS,NULL);
	if(rda == NULL)
	{
		Printf("ERROR: Not enough memory for option parser.\n");

		SHOWMSG("not enough memory for option parser");
		goto out;
	}

	/* Proceed to read the configuration file line by line. */
	line_number = 0;

	while(TRUE)
	{
		if(FGets(file,line_buffer,line_buffer_size) == NULL)
		{
			SHOWMSG("end of file");
			break;
		}

		line_number++;

		/* Strip leading blanks. */
		s = line_buffer;
		while((*s) == ' ' || (*s) == (UBYTE)'\240' || (*s) == '\t')
			s++;

		/* Skip comments. */
		if((*s) == '#' || (*s) == ';')
			continue;

		/* Strip trailing blanks. */
		len = strlen(s);
		while(len > 0 && (s[len-1] == ' ' || s[len-1] == (UBYTE)'\240' || s[len-1] == '\t' || s[len-1] == '\r' || s[len-1] == '\n'))
			len--;

		if(len == 0)
			continue;

		s[len] = '\0';

		D(("config line |%s|",s));

		/* Find the blank space or '=' that separates keyword
		 * and parameter.
		 */
		t = v = s;

		while((*t) != ' ' && (*t) != '\t' && (*t) != '=' && (*t) != '\0')
		{
			v++;
			t++;
		}

		/* Strip the extra characters that separate keyword and parameter. */
		if((*t) != '\0')
		{
			/* That knocks out the character between the keyword and the parameter. */
			(*v++) = ' ';

			t++;

			/* Skip all separator characters there may be. */
			while((*t) == ' ' || (*t) == '\t' || (*t) == '=')
				t++;

			/* If the next character is not a quote, add one
			 * at the beginning and append one, too.
			 */
			if((*t) != '\"')
			{
				SHOWMSG("adding quotes");

				memmove(t+1,t,strlen(t)+1);
				t[0] = '\"';
				strcat(t,"\"");
			}

			while((*t) != '\0')
				(*v++) = (*t++);
		}

		(*v) = '\0';

		D(("line prior to parsing = |%s|",s));

		len = strlen(s);

		/* Find the separator character. */
		separator_index = -1;
		separator_char = '\0';

		for(i = 0 ; i < len ; i++)
		{
			if(s[i] == ' ' || s[i] == '=')
			{
				separator_index	= i;
				separator_char	= s[i];

				s[i] = '\0';
				break;
			}
		}

		/* Do we know this keyword? */
		if(FindArg(args_template,s) == -1)
		{
			Printf("Warning: unknown keyword '%s' in line %ld of '%s'.\n",
				s,line_number,file_name);

			continue;
		}

		/* Restore the separator character. */
		if(separator_index != -1)
			s[separator_index] = separator_char;

		s[len++]	= '\n';
		s[len]		= '\0';

		/* Finally parse that line. */
		FreeArgs(rda);
		memset(&args,0,sizeof(args));

		rda->RDA_Source.CS_Buffer = s;
		rda->RDA_Source.CS_Length = len;
		rda->RDA_Source.CS_CurChr = 0;
		rda->RDA_Flags |= RDAF_NOPROMPT;

		SHOWMSG("parsing the line...");

		if(CANNOT ReadArgs((STRPTR)args_template,(LONG *)&args,rda))
		{
			error = IoErr();

			Fault(error,NULL,line_buffer,line_buffer_size);

			Printf("ERROR: Parse error in line %ld of '%s' (%s).\n",
				line_number,file_name,line_buffer);

			break;
		}

		SHOWMSG("done; picking up the pieces");

		/* Name of the interface to use. */
		if(args.Interface != NULL)
		{
			if(QueryInterfaceTags(args.Interface,
				IFQ_DeviceName,&device_name,
				IFQ_DeviceUnit,&device_unit,
			TAG_DONE) != 0)
			{
				Printf("ERROR: Could not obtain information on interface '%s'.\n",
					args.Interface);

				goto out;
			}

			len = strlen(args.Interface);
			if(len > interface_name_len-1)
				len = interface_name_len-1;

			strncpy(interface_name,args.Interface,(size_t)len);
			interface_name[len] = '\0';
		}

		/* Choose the device driver to use. */
		if(args.Device != NULL)
		{
			if(CANNOT add_option_str("ppp.ethernet.device",args.Device))
			{
				Printf("ERROR: Not enough memory to add option '%s=%s'.\n",
					"DEVICE",args.Device);

				goto out;
			}

			have_device = TRUE;
		}

		/* The device driver unit to use. */
		if(args.Unit != NULL)
		{
			if(CANNOT add_option_num("ppp.ethernet.unit",(*args.Unit)))
			{
				Printf("ERROR: Not enough memory to add option '%s=%s'.\n",
					"UNIT",(*args.Unit));

				goto out;
			}
		}

		if(args.Raw != NULL)
		{
			LONG value;

			value = match_key(args.Raw);
			if(value != -1)
			{
				if(CANNOT add_option_num("ppp.ethernet.raw",value))
				{
					Printf("ERROR: Not enough memory to add option '%s=%s'.\n",
						"RAW",args.Raw);

					goto out;
				}
			}
			else
			{
				Printf("Warning: unknown key '%s=%s' in line %ld of '%s'.\n",
					"RAW",args.Raw,line_number,file_name);
			}
		}

		if(args.Bypass != NULL)
		{
			LONG value;

			value = match_key(args.Bypass);
			if(value != -1)
			{
				if(CANNOT add_option_num("ppp.ethernet.bypass",value))
				{
					Printf("ERROR: Not enough memory to add option '%s=%s'.\n",
						"BYPASS",args.Bypass);

					goto out;
				}
			}
			else
			{
				Printf("Warning: unknown key '%s=%s' in line %ld of '%s'.\n",
					"BYPASS",args.Bypass,line_number,file_name);
			}
		}

		if(args.NumReadPackets != NULL)
		{
			if((*args.NumReadPackets) > 0)
			{
				if(CANNOT add_option_num("ppp.ethernet.readpackets",(*args.NumReadPackets)))
				{
					Printf("ERROR: Not enough memory to add option '%s=%ld'.\n",
						"READPACKETS",(*args.NumReadPackets));

					goto out;
				}
			}
			else
			{
				Printf("Warning: '%s=%ld' in line %ld of '%s' ignored.\n",
					"READPACKETS",(*args.NumReadPackets),line_number,file_name);
			}
		}

		if(args.NumWritePackets != NULL)
		{
			if((*args.NumWritePackets) > 0)
			{
				if(CANNOT add_option_num("ppp.ethernet.writepackets",(*args.NumWritePackets)))
				{
					Printf("ERROR: Not enough memory to add option '%s=%ld'.\n",
						"WRITEPACKETS",(*args.NumWritePackets));

					goto out;
				}
			}
			else
			{
				Printf("Warning: '%s=%ld' in line %ld of '%s' ignored.\n",
					"WRITEPACKETS",(*args.NumWritePackets),line_number,file_name);
			}
		}

		/* Set the maxfail counter default? */
		if(args.MaxFail != NULL)
		{
			if((*args.MaxFail) > 0)
			{
				if(CANNOT add_option_num("ppp.maxfail",(*args.MaxFail)))
				{
					Printf("ERROR: Not enough memory to add option '%s=%ld'.\n",
						"MAXFAIL",(*args.MaxFail));

					goto out;
				}
			}
			else
			{
				Printf("Warning: '%s=%ld' in line %ld of '%s' ignored.\n",
					"MAXFAIL",(*args.MaxFail),line_number,file_name);
			}
		}

		/* Set the maxterm counter default? */
		if(args.MaxTerm != NULL)
		{
			if((*args.MaxTerm) > 0)
			{
				if(CANNOT add_option_num("ppp.maxterm",(*args.MaxTerm)))
				{
					Printf("ERROR: Not enough memory to add option '%s=%ld'.\n",
						"MAXTERM",(*args.MaxTerm));

					goto out;
				}
			}
			else
			{
				Printf("Warning: '%s=%ld' in line %ld of '%s' ignored.\n",
					"MAXTERM",(*args.MaxTerm),line_number,file_name);
			}
		}

		/* Set the maxconfig counter default? */
		if(args.MaxConfig != NULL)
		{
			if((*args.MaxConfig) > 0)
			{
				if(CANNOT add_option_num("ppp.config",(*args.MaxConfig)))
				{
					Printf("ERROR: Not enough memory to add option '%s=%ld'.\n",
						"MAXCONFIG",(*args.MaxConfig));

					goto out;
				}
			}
			else
			{
				Printf("Warning: '%s=%ld' in line %ld of '%s' ignored.\n",
					"MAXCONFIG",(*args.MaxConfig),line_number,file_name);
			}
		}

		/* Set the default timeout? */
		if(args.Timeout != NULL)
		{
			if((*args.Timeout) > 0)
			{
				if(CANNOT add_option_num("ppp.timeout",(*args.Timeout)))
				{
					Printf("ERROR: Not enough memory to add option '%s=%ld'.\n",
						"TIMEOUT",(*args.Timeout));

					goto out;
				}
			}
			else
			{
				Printf("Warning: '%s=%ld' in line %ld of '%s' ignored.\n",
					"TIMEOUT",(*args.Timeout),line_number,file_name);
			}
		}

		/* Set the maximum number of reconfigurations? */
		if(args.MaxReconfigure != NULL)
		{
			if((*args.MaxReconfigure) > 0)
			{
				if(CANNOT add_option_num("ppp.maxreconfigure",(*args.MaxReconfigure)))
				{
					Printf("ERROR: Not enough memory to add option '%s=%ld'.\n",
						"MAXRECONFIGURE",(*args.MaxReconfigure));

					goto out;
				}
			}
			else
			{
				Printf("Warning: '%s=%ld' in line %ld of '%s' ignored.\n",
					"MAXRECONFIGURE",(*args.MaxReconfigure),line_number,file_name);
			}
		}

		/* Use a static host address? */
		if(args.LocalAddress != NULL)
		{
			if(CANNOT convert_string_to_ip_address(args.LocalAddress))
			{
				Printf("ERROR: Invalid IP address '%s' in line %ld of '%s'.\n",
					args.LocalAddress,line_number,file_name);

				goto out;
			}

			if(CANNOT add_option_str("ppp.localaddress",args.LocalAddress))
			{
				Printf("ERROR: Not enough memory to add option '%s=%s'.\n",
					"LOCALADDRESS",args.LocalAddress);

				goto out;
			}
		}

		/* Use a static peer address? */
		if(args.RemoteAddress != NULL)
		{
			if(CANNOT convert_string_to_ip_address(args.RemoteAddress))
			{
				Printf("ERROR: Invalid IP address '%s' in line %ld of '%s'.\n",
					args.RemoteAddress,line_number,file_name);

				goto out;
			}

			if(CANNOT add_option_str("ppp.remoteaddress",args.RemoteAddress))
			{
				Printf("ERROR: Not enough memory to add option '%s=%s'.\n",
					"REMOTEADDRESS",args.RemoteAddress);

				goto out;
			}
		}

		/* Use a static primary domain name server address? */
		if(args.DNS1Address != NULL)
		{
			if(CANNOT convert_string_to_ip_address(args.DNS1Address))
			{
				Printf("ERROR: Invalid IP address '%s' in line %ld of '%s'.\n",
					args.DNS1Address,line_number,file_name);

				goto out;
			}

			if(CANNOT add_option_str("ppp.dns1address",args.DNS1Address))
			{
				Printf("ERROR: Not enough memory to add option '%s=%s'.\n",
					"DNS1ADDRESS",args.DNS1Address);

				goto out;
			}
		}

		/* Use a static secondary domain name server address? */
		if(args.DNS2Address != NULL)
		{
			if(CANNOT convert_string_to_ip_address(args.DNS2Address))
			{
				Printf("ERROR: Invalid IP address '%s' in line %ld of '%s'.",
					args.DNS2Address,line_number,file_name);

				goto out;
			}

			if(CANNOT add_option_str("ppp.dns2address",args.DNS2Address))
			{
				Printf("ERROR: Not enough memory to add option '%s=%s'.\n",
					"DNS2ADDRESS",args.DNS2Address);

				goto out;
			}
		}

		/* Set the maximum transmission unit? */
		if(args.MTU != NULL)
		{
			LONG mtu;

			mtu = (*args.MTU);
			if(64 <= mtu && mtu <= PPP_DEFAULT_MTU)
			{
				if(CANNOT add_option_num("ppp.mtu",mtu))
				{
					Printf("ERROR: Not enough memory to add option '%s=%ld'.\n",
						"MTU",(*args.MTU));

					goto out;
				}
			}
			else
			{
				Printf("Warning: '%s=%ld' in line %ld of '%s' ignored.\n",
					"MTU",mtu,line_number,file_name);
			}
		}

		/* Set the idle timeout? */
		if(args.IdleTimeout != NULL)
		{
			if(CANNOT add_option_num("ppp.idletimeout",(*args.IdleTimeout)))
			{
				Printf("ERROR: Not enough memory to add option '%s=%ld'.\n",
					"IDLETIMEOUT",(*args.IdleTimeout));

				goto out;
			}
		}

		/* Set the peer idle timeout? */
		if(args.PeerIdleTimeout != NULL)
		{
			if(CANNOT add_option_num("ppp.peeridletimeout",(*args.PeerIdleTimeout)))
			{
				Printf("ERROR: Not enough memory to add option '%s=%ld'.\n",
					"PEERIDLETIMEOUT",(*args.PeerIdleTimeout));

				goto out;
			}
		}

		if(args.Service != NULL)
		{
			if(CANNOT add_option_str("ppp.ethernet.service",args.Service))
			{
				Printf("ERROR: Not enough memory to add option '%s=%s'.\n",
					"SERVICE",args.Service);

				goto out;
			}
		}

		if(args.AccessConcentrator != NULL)
		{
			if(CANNOT add_option_str("ppp.ethernet.ac",args.AccessConcentrator))
			{
				Printf("ERROR: Not enough memory to add option '%s=%s'.\n",
					"ACCESSCONCENTRATOR",args.AccessConcentrator);

				goto out;
			}
		}

		/* Set environment variables when going online? */
		if(args.SetEnv != NULL)
		{
			LONG value;

			value = match_key(args.SetEnv);
			if(value != -1)
			{
				if(CANNOT add_option_num("ppp.x-setenv",value))
				{
					Printf("ERROR: Not enough memory to add option '%s=%s'.\n",
						"SETENV",args.SetEnv);

					goto out;
				}
			}
			else
			{
				Printf("Warning: unknown key '%s=%s' in line %ld of '%s'.\n",
					"SETENV",args.SetEnv,line_number,file_name);
			}
		}

		/* Set environment variables when going online? */
		if(args.SendID != NULL)
		{
			LONG value;

			value = match_key(args.SendID);
			if(value != -1)
			{
				if(CANNOT add_option_num("ppp.sendid",value))
				{
					Printf("ERROR: Not enough memory to add option '%s=%s'.\n",
						"SENDID",args.SendID);

					goto out;
				}
			}
			else
			{
				Printf("Warning: unknown key '%s=%s' in line %ld of '%s'.\n",
					"SENDID",args.SendID,line_number,file_name);
			}
		}

		/* Reject the Password Authentication Protocol? */
		if(args.RejectPAP != NULL)
		{
			LONG value;

			value = match_key(args.RejectPAP);
			if(value != -1)
			{
				if(CANNOT add_option_num("ppp.rejectpap",value))
				{
					Printf("ERROR: Not enough memory to add option '%s=%s'.\n",
						"REJECTPAP",args.RejectPAP);

					goto out;
				}
			}
			else
			{
				Printf("Warning: unknown key '%s=%s' in line %ld of '%s'.\n",
					"REJECTPAP",args.RejectPAP,line_number,file_name);
			}
		}

		/* Set the PAP timeout? */
		if(args.PAPTimeout != NULL)
		{
			if((*args.PAPTimeout) > 0)
			{
				if(CANNOT add_option_num("ppp.pap.timeout",(*args.PAPTimeout)))
				{
					Printf("ERROR: Not enough memory to add option '%s=%ld'.\n",
						"PAPTIMEOUT",(*args.PAPTimeout));

					goto out;
				}
			}
			else
			{
				Printf("Warning: '%s=%ld' in line %ld of '%s' ignored.\n",
					"PAPTIMEOUT",(*args.PAPTimeout),line_number,file_name);
			}
		}

		/* Set the number of PAP retries? */
		if(args.PAPRetry != NULL)
		{
			if((*args.PAPRetry) > 0)
			{
				if(CANNOT add_option_num("ppp.pap.retry",(*args.PAPRetry)))
				{
					Printf("ERROR: Not enough memory to add option '%s=%ld'.\n",
						"PAPRETRY",(*args.PAPRetry));

					goto out;
				}
			}
			else
			{
				Printf("Warning: '%s=%ld' in line %ld of '%s' ignored.\n",
					"PAPRETRY",(*args.PAPRetry),line_number,file_name);
			}
		}

		/* Substitute a dummy remote IP address if the peer does not reveal it? */
		if(args.DummyRemoteAddress != NULL)
		{
			LONG value;

			value = match_key(args.DummyRemoteAddress);
			if(value != -1)
			{
				if(CANNOT add_option_num("ppp.dummyremoteaddress",value))
				{
					Printf("ERROR: Not enough memory to add option '%s=%s'.\n",
						"DUMMYREMOTEADDRESS",args.DummyRemoteAddress);

					goto out;
				}
			}
			else
			{
				Printf("Warning: unknown key '%s=%s' in line %ld of '%s'.\b",
					"DUMMYREMOTEADDRESS",args.DummyRemoteAddress,line_number,file_name);
			}
		}

		if(args.ConnectTimeout != NULL)
		{
			if((*args.ConnectTimeout) > 0)
			{
				if(CANNOT add_option_num("ppp.ethernet.connecttimeout",(*args.ConnectTimeout)))
				{
					Printf("ERROR: Not enough memory to add option '%s=%s'.\n",
						"CONNECTTIMEOUT",(*args.ConnectTimeout));

					goto out;
				}
			}
			else
			{
				Printf("Warning: '%s=%ld' in line %ld of '%s' ignored.\n",
					"CONNECTTIMEOUT",(*args.ConnectTimeout),line_number,file_name);
			}
		}

		/* Specify the login string? */
		if(args.Login != NULL)
		{
			FreeVec(Login);

			Login = AllocVec(strlen(args.Login)+1,MEMF_ANY|MEMF_PUBLIC);
			if(Login == NULL)
			{
				Printf("ERROR: Not enough memory for login text.\n");
				goto out;
			}

			strcpy(Login,args.Login);
		}

		/* Specify the password? */
		if(args.Password != NULL)
		{
			FreeVec(Password);

			Password = AllocVec(strlen(args.Password)+1,MEMF_ANY|MEMF_PUBLIC);
			if(Password == NULL)
			{
				Printf("ERROR: Not enough memory for password text.\n");
				goto out;
			}

			strcpy(Password,args.Password);
		}

		/* Name of the log file. */
		if(args.LogFile != NULL)
		{
			if(CANNOT add_option_str("logfile",args.LogFile))
			{
				Printf("ERROR: Not enough memory to add option '%s=%s'.\n",
					"LOGFILE",args.LogFile);

				goto out;
			}
		}

		/* Which log options should be enabled. */
		if(args.Log != NULL)
		{
			if(CANNOT add_option_str("logoptions",args.Log))
			{
				Printf("ERROR: Not enough memory to add option '%s=%s'.\n",
					"LOGOPTIONS",args.Log);

				goto out;
			}
		}
	}

	if(device_name == NULL)
	{
		Printf("ERROR: No interface name was specified.\n");
		goto out;
	}

	if(NOT have_device)
	{
		Printf("ERROR: No device driver name was specified.\n");
		goto out;
	}

	error = OpenDevice(device_name,device_unit,(struct IORequest *)NetRequest,0);
	if(error != OK)
	{
		UBYTE io_error_str[100];

		Printf("ERROR: Could not open '%s', unit %ld (%s).\n",
			device_name,device_unit,get_io_error_string(error,io_error_str));

		goto out;
	}

	/* Duplicate the I/O request; we need the copy for the S2EVENT_OFFLINE signalling. */
	(*NetEventRequest) = (*NetRequest);
	NetEventRequest->ios2_Req.io_Message.mn_ReplyPort = NetEventPort;

	success = TRUE;

 out:

	if(file != ZERO)
		Close(file);

	FreeVec(line_buffer);

	if(rda != NULL)
	{
		FreeArgs(rda);
		FreeDosObject(DOS_RDARGS,rda);
	}

	RETURN(success);
	return(success);
}

/****************************************************************************/

/* Obtain the error code set by bsdsocket.library and try to find
 * the matching descriptive text as well.
 */
void
get_errno_code(STRPTR * code_ptr)
{
	LONG errno = 0;
	LONG code;

	ASSERT( code_ptr != NULL );

	SocketBaseTags(
	    SBTM_GETREF(SBTC_ERRNO),&errno,
	TAG_END);

	code = errno;

	if(SocketBaseTags(
	    SBTM_GETREF(SBTC_ERRNOSTRPTR),&code,
	TAG_END) != 0)
	{
		code = 0;
	}

	(*code_ptr) = (STRPTR)code;
}

/****************************************************************************/

/* Configure a networking interface by querying the connection information
 * obtained when the associated device went online.
 */
void
configure_interface(STRPTR interface)
{
	struct sockaddr_in addr_sin;
	struct sockaddr_in netmask_sin;
	struct sockaddr_in destination_addr_sin;
	struct sockaddr_in primary_dns_sin;
	struct sockaddr_in secondary_dns_sin;
	UBYTE address_buf[40];
	UBYTE net_mask_buf[40];
	UBYTE destination_address_buf[40];
	struct rt_msghdr * route_table;
	struct sockaddr * default_route = NULL;
	struct rt_msghdr * rtm;
	struct sockaddr * gateway_sa;
	struct sockaddr * destination_sa;
	STRPTR code = NULL;
	ULONG hardware_mtu_size;
	LONG len;

	ENTER();

	ASSERT( interface != NULL );

	/* Make a copy of the current routing table. We need it to find
	 * out which entry refers to the default route, which is the one
	 * we will want ot replace.
	 */
	route_table = GetRouteInfo(0,0);
	if(route_table == NULL)
	{
		Printf("ERROR: Unable to query routing information.\n");
		goto out;
	}

	/* Obtain the peer and domain name server addresses for this
	 * interface.
	 */
	if(ConfigureInterfaceTags(interface,
		IFC_GetPeerAddress,	TRUE,
		IFC_GetDNS,			TRUE,
	TAG_END) != OK)
	{
		get_errno_code(&code);

		Printf("ERROR: Could not configure interface '%s'",interface);

		if(code != NULL)
			Printf(" (%s)",code);

		Printf(".\n");

		goto out;
	}

	/* Transfer the information obtained above into local
	 * variables.
	 */
	if(QueryInterfaceTags(interface,
		IFQ_Address,				&addr_sin,
		IFQ_NetMask,				&netmask_sin,
		IFQ_DestinationAddress,		&destination_addr_sin,
		IFQ_PrimaryDNSAddress,		&primary_dns_sin,
		IFQ_SecondaryDNSAddress,	&secondary_dns_sin,
		IFQ_HardwareMTU,			&hardware_mtu_size,	/* querying this will magically update the TCP/IP stack's MTU settings for this interface */
	TAG_DONE) != OK)
	{
		get_errno_code(&code);

		Printf("ERROR: Could not query interface '%s' configuration",interface);

		if(code != NULL)
			Printf(" (%s)",code);

		Printf(".\n");

		goto out;
	}

	/* Show the current configuration, as far as we know the details. */
	Printf("Interface '%s' configured (",interface);

	strcpy(address_buf,Inet_NtoA(addr_sin.sin_addr.s_addr));
	Printf("address = %s",address_buf);

	strcpy(net_mask_buf,Inet_NtoA(netmask_sin.sin_addr.s_addr));
	Printf(", network mask = %s",net_mask_buf);

	strcpy(destination_address_buf,Inet_NtoA(destination_addr_sin.sin_addr.s_addr));
	Printf(", destination address = %s",destination_address_buf);

	Printf(").\n");

	/* Let's try to find the default route in this table and
	 * remember where it points to.
	 */
	for(rtm = route_table ;
		rtm->rtm_msglen > 0 ;
		rtm = (struct rt_msghdr *)(((ULONG)rtm) + rtm->rtm_msglen))
	{
		if(rtm->rtm_version != 3 || (rtm->rtm_addrs & RTA_DST) == 0 || (rtm->rtm_addrs & RTA_GATEWAY) == 0)
			continue;

		/* The destination address follows the header. */
		destination_sa = (struct sockaddr *)(rtm + 1);
		if(destination_sa->sa_family != AF_INET)
			continue;

		if(destination_sa->sa_len == 0)
			len = sizeof(long);
		else
			len = destination_sa->sa_len;

		/* The gateway address follows the destination address. */
		gateway_sa = (struct sockaddr *)(((ULONG)destination_sa) + len);
		if(gateway_sa->sa_family != AF_INET)
			continue;

		/* If the destination address is zero, then we've found the
		 * default route.
		 */
		if(((struct sockaddr_in *)destination_sa)->sin_addr.s_addr == INADDR_ANY)
		{
			default_route = gateway_sa;
			break;
		}
	}

	/* If we found the route to the default gateway, remove it. */
	if(default_route != NULL)
	{
		UBYTE str[20];

		/* Take out the default route. */
		strcpy(str,Inet_NtoA(((struct sockaddr_in *)default_route)->sin_addr.s_addr));

		if(DeleteRouteTags(
			RTA_DefaultGateway,str,
		TAG_DONE) != OK)
		{
			get_errno_code(&code);

			Printf("Warning: could not remove default route to %s",str);

			if(code != NULL)
				Printf(" (%s)",code);

			Printf(".\n");
		}
	}

	/* Add a new default route. */
	if(AddRouteTags(
		RTA_DefaultGateway,destination_address_buf,
	TAG_DONE) != OK)
	{
		get_errno_code(&code);

		Printf("ERROR: Could not add route to %s",destination_address_buf);

		if(code != NULL)
			Printf(" (%s)",code);

		Printf(".\n");

		goto out;
	}

	Printf("Added default route to %s.\n",destination_address_buf);

	/* Remember that a route is attached to this interface. */
	ConfigureInterfaceTags(interface,
		IFC_AssociatedRoute,TRUE,
	TAG_DONE);

	/* Is a primary domain name server address provided? */
	if(primary_dns_sin.sin_addr.s_addr != INADDR_ANY)
	{
		UBYTE primary_dns_buf[40];

		strcpy(primary_dns_buf,Inet_NtoA(primary_dns_sin.sin_addr.s_addr));

		if(AddDomainNameServer(primary_dns_buf) != OK)
		{
			get_errno_code(&code);

			Printf("ERROR: Could not add domain name server %s",primary_dns_buf);

			if(code != NULL)
				Printf(" (%s)",code);

			Printf(".\n");

			goto out;
		}

		Printf("Added domain name server %s.\n",primary_dns_buf);

		/* Remember that a domain name server is attached to this interface. */
		ConfigureInterfaceTags(interface,
			IFC_AssociatedDNS,TRUE,
		TAG_DONE);
	}

	/* Is a secondary domain name server address provided? */
	if(secondary_dns_sin.sin_addr.s_addr != INADDR_ANY)
	{
		UBYTE secondary_dns_buf[40];

		strcpy(secondary_dns_buf,Inet_NtoA(secondary_dns_sin.sin_addr.s_addr));

		if(AddDomainNameServer(secondary_dns_buf) != OK)
		{
			get_errno_code(&code);

			Printf("ERROR: Could not add domain name server %s",secondary_dns_buf);

			if(code != NULL)
				Printf(" (%s)",code);

			Printf(".\n");

			goto out;
		}

		Printf("Added domain name server %s.\n",secondary_dns_buf);

		/* Remember that a domain name server is attached to this interface. */
		ConfigureInterfaceTags(interface,
			IFC_AssociatedDNS,TRUE,
		TAG_DONE);
	}

 out:

	if(route_table != NULL)
		FreeRouteInfo(route_table);

	LEAVE();
}

/****************************************************************************/

enum connect_result_t
{
	connect_result_no_connection,
	connect_result_passive_offline,
	connect_result_active_offline
};

/****************************************************************************/

/* Tell the interface to establish a connection, wait for it to go
 * online, then wait for the user to indicate that he wants to go
 * offline again, and eventually take the interface offline again.
 */
enum connect_result_t
connect_interface(STRPTR interface)
{
	struct Sana2Connection * s2c = &Sana2Connection;
	ULONG net_event_mask;
	ULONG net_mask;
	ULONG hook_mask;
	ULONG signal_mask;
	ULONG signals;
	BOOL net_command_pending;
	BOOL connected;
	enum connect_result_t result = connect_result_no_connection;

	/* Set up the connection message to be submitted
	 * to the driver.
	 */
	s2c->s2c_Size					= sizeof(*s2c);
	s2c->s2c_ErrorHook.h_Entry		= (HOOKFUNC)ErrorHookFunc;
	s2c->s2c_ConnectHook.h_Entry	= (HOOKFUNC)ConnectHookFunc;
	s2c->s2c_DisconnectHook.h_Entry	= (HOOKFUNC)DisconnectHookFunc;
	s2c->s2c_Login					= Login;
	s2c->s2c_Password				= Password;

	/* Send the message; it will return eventually. */
	NetRequest->ios2_Req.io_Command	= S2_CONNECT;
	NetRequest->ios2_Data			= s2c;

	SendIO((struct IORequest *)NetRequest);
	net_command_pending = TRUE;

	connected = FALSE;

	net_event_mask = PORT_SIG_MASK(NetEventPort);
	net_mask = PORT_SIG_MASK(NetPort);
	hook_mask = PORT_SIG_MASK(HookPort);

	signal_mask = net_event_mask | net_mask | hook_mask | SIGBREAKF_CTRL_C;
	signals = 0;

	while(TRUE)
	{
		/* Pick up the signals that are pending. Either wait for
		 * some to arrive or poll them.
		 */
		if(signals == 0)
			signals = Wait(signal_mask);
		else
			signals |= SetSignal(0,signal_mask) & signal_mask;

		/* A networking I/O request has returned? */
		if(signals & net_mask)
		{
			struct IOSana2Req * ios2;

			ios2 = (struct IOSana2Req *)GetMsg(NetPort);
			if(ios2 != NULL)
			{
				SHOWMSG("net I/O request has returned");

				ASSERT( ios2 == NetRequest );

				net_command_pending = FALSE;

				/* If not connected, then the I/O request must
				 * refer to the S2_CONNECT command.
				 */
				if(NOT connected)
				{
					if(ios2->ios2_Req.io_Error != OK)
					{
						UBYTE io_error_str[100];
						UBYTE wire_error_str[100];

						/* Ouch! */
						Printf("ERROR: Connection attempt failed (%s, %s).\n",
							get_io_error_string(ios2->ios2_Req.io_Error,io_error_str),
							get_wire_error_string(ios2->ios2_WireError,wire_error_str));

						break;
					}

					/* So we're connected now. Configure the interface (addresses,
					 * routes, domain name servers, etc.) and wait for it to
					 * go offline again.
					 */
					connected = TRUE;

					configure_interface(interface);

					/* We want to be notified when the device goes offline again. */
					NetEventRequest->ios2_Req.io_Command	= S2_ONEVENT;
					NetEventRequest->ios2_WireError			= S2EVENT_OFFLINE;

					SendIO((struct IORequest *)NetEventRequest);
					NetEventRequestPending = TRUE;

					Printf("%s has connected; ^C to disconnect_interface.\n",interface);
				}
				else
				{
					/* We used to be connected. */
					Printf("%s has disconnected.\n",interface);

					break;
				}
			}
			else
			{
				signals &= ~net_mask;
			}
		}

		/* The online state of the interface has changed? */
		if(signals & net_event_mask)
		{
			struct IOSana2Req * ios2;

			ios2 = (struct IOSana2Req *)GetMsg(NetEventPort);
			if(ios2 != NULL)
			{
				SHOWMSG("net event I/O request has returned");

				ASSERT( ios2 == NetEventRequest );

				NetEventRequestPending = FALSE;

				/* The interface is now offline. */
				Printf("%s is offline.\n",interface);

				if(result == connect_result_no_connection)
					result = connect_result_passive_offline;

				/* We may have to wait for the other I/O request
				 * to return first, though.
				 */
				if(NOT net_command_pending)
					break;
			}
			else
			{
				signals &= ~net_event_mask;
			}
		}

		/* The device has called into one of the hooks? */
		if(signals & hook_mask)
		{
			struct HookMessage * hm;

			hm = (struct HookMessage *)GetMsg(HookPort);
			if(hm != NULL)
			{
				SHOWMSG("hook message has arrived");

				/* A non-NULL message node name indicates
				 * an error message.
				 */
				if(hm->hm_Message.mn_Node.ln_Name != NULL)
				{
					SHOWMSG("error message");

					/* Display the message, then release it. */
					Printf("ERROR: %s\n",hm->hm_Message.mn_Node.ln_Name);
					FreeVec(hm);
				}
				else
				{
					/* This should never happen. */
					SHOWMSG("weird hook message!");
				}
			}
			else
			{
				signals &= ~hook_mask;
			}
		}

		/* The connection is to be closed? */
		if(signals & SIGBREAKF_CTRL_C)
		{
			/* We have to disconnect. */
			if(connected && NOT net_command_pending)
			{
				Printf("Disconnecting...\n");

				NetRequest->ios2_Req.io_Command	= S2_DISCONNECT;
				NetRequest->ios2_Data			= s2c;

				SendIO((struct IORequest *)NetRequest);
				net_command_pending = TRUE;

				if(result == connect_result_no_connection)
					result = connect_result_active_offline;
			}
			else if (NOT connected && net_command_pending)
			{
				/* Try to stop the connection process. */
				AbortIO((struct IORequest *)NetRequest);
			}

			signals &= ~SIGBREAKF_CTRL_C;
		}
	}

	return(result);
}

/****************************************************************************/

int
main(int argc,char ** argv)
{
	int result = RETURN_FAIL;

	if(setup())
	{
		struct
		{
			KEY		Name;
			SWITCH	Reconnect;
		} args;

		struct RDArgs * rda;

		memset(&args,0,sizeof(args));

		rda = ReadArgs("NAME/A,RECONNECT/S",(LONG *)&args,NULL);
		if(rda != NULL)
		{
			UBYTE interface[40];

			if(configure_connection(args.Name,interface,sizeof(interface)))
			{
				BOOL done;

				do
				{
					done = TRUE;

					switch(connect_interface(interface))
					{
						case connect_result_no_connection:

							result = RETURN_WARN;
							break;

						case connect_result_active_offline:

							result = RETURN_OK;
							break;

						case connect_result_passive_offline:

							if(args.Reconnect)
							{
								Printf("Reconnecting...\n");

								Delay(TICKS_PER_SECOND);

								done = FALSE;
							}
							else
							{
								result = RETURN_OK;
							}

							break;
					}
				}
				while(NOT done);
			}

			FreeArgs(rda);
		}
		else
		{
			PrintFault(IoErr(),"PPP_Connector");
		}
	}

	cleanup();

	return(result);
}
