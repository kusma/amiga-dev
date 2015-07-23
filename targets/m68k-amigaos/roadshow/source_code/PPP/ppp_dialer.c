/*
 *	$Id: ppp_dialer.c,v 1.9 2005/06/30 07:44:50 obarthel Exp $
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
#include <devices/serial.h>

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

#include "ppp_dialer_rev.h"
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
struct Library *	SocketBase;

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

struct MsgPort *	SerReadPort;
struct IOExtSer *	SerReadRequest;
BOOL				SerReadPending;
struct MsgPort *	SerWritePort;
struct IOExtSer *	SerWriteRequest;

UBYTE				SerReadBuffer[1024];
LONG				SerReadOffset;
LONG				SerReadLength;
UBYTE				SerReadChar;

/****************************************************************************/

struct MsgPort * 	HookPort;

/****************************************************************************/

struct MsgPort *		TimePort;
struct timerequest *	TimeRequest;
BOOL					TimeTicking;

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

#endif /* __GNUC__ && !__amigaos4__ */

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

/* This initializes a local MsgPort using the one-shot signalling method. */
void
init_single_port(struct MsgPort * mp)
{
	ENTER();

	ASSERT( mp != NULL );

	memset(mp,0,sizeof(*mp));

	mp->mp_Flags	= PA_SIGNAL;
	mp->mp_SigBit	= SIGB_SINGLE;
	mp->mp_SigTask	= FindTask(NULL);

	NewList(&mp->mp_MsgList);

	LEAVE();
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

	if(TimeTicking)
	{
		if(CheckIO((struct IORequest *)TimeRequest) == BUSY)
			AbortIO((struct IORequest *)TimeRequest);

		WaitIO((struct IORequest *)TimeRequest);

		TimeTicking = FALSE;
	}

	if(TimeRequest != NULL)
	{
		if(TimeRequest->tr_node.io_Device != NULL)
			CloseDevice((struct IORequest *)TimeRequest);

		DeleteIORequest((struct IORequest *)TimeRequest);
		TimeRequest = NULL;
	}

	if(TimePort != NULL)
	{
		DeleteMsgPort(TimePort);
		TimePort = NULL;
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

	if(SerReadRequest != NULL)
	{
		DeleteIORequest((struct IORequest *)SerReadRequest);
		SerReadRequest = NULL;
	}

	if(SerReadPort != NULL)
	{
		DeleteMsgPort(SerReadPort);
		SerReadPort = NULL;
	}

	if(SerWriteRequest != NULL)
	{
		DeleteIORequest((struct IORequest *)SerWriteRequest);
		SerWriteRequest = NULL;
	}

	if(SerWritePort != NULL)
	{
		DeleteMsgPort(SerWritePort);
		SerWritePort = NULL;
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

	SerReadPort = CreateMsgPort();
	if(SerReadPort == NULL)
	{
		Printf("ERROR: Could not create serial read port.\n");
		goto out;
	}

	SerReadRequest = (struct IOExtSer *)CreateIORequest(SerReadPort,sizeof(*SerReadRequest));
	if(SerReadRequest == NULL)
	{
		Printf("ERROR: Could not create serial read request.\n");
		goto out;
	}

	SerWritePort = CreateMsgPort();
	if(SerWritePort == NULL)
	{
		Printf("ERROR: Could not create serial write port.\n");
		goto out;
	}

	SerWriteRequest = (struct IOExtSer *)CreateIORequest(SerWritePort,sizeof(*SerWriteRequest));
	if(SerWriteRequest == NULL)
	{
		Printf("ERROR: Could not create serial write request.\n");
		goto out;
	}

	HookPort = CreateMsgPort();
	if(HookPort == NULL)
	{
		Printf("ERROR: Could not create hook port.\n");
		goto out;
	}

	TimePort = CreateMsgPort();
	if(TimePort == NULL)
	{
		Printf("ERROR: Could not create timer port.\n");
		goto out;
	}

	TimeRequest = (struct timerequest *)CreateIORequest(TimePort,sizeof(*TimeRequest));
	if(TimeRequest == NULL)
	{
		Printf("ERROR: Could not create time request.\n");
		goto out;
	}

	if(OpenDevice(TIMERNAME,UNIT_VBLANK,(struct IORequest *)TimeRequest,0) != OK)
	{
		Printf("ERROR: Could not open 'timer.device'.\n");
		goto out;
	}

	success = TRUE;

 out:

	return(success);
}

/****************************************************************************/

/* This sends a command to the modem. The command string can include
 * special control sequences, which will be decoded and processed
 * along with the rest.
 */
void
send_command(STRPTR cmd)
{
	LONG len;

	ENTER();

	ASSERT( cmd != NULL );

	/* Skip leading blanks. */
	while((*cmd) == ' ' || (*cmd) == '\t')
		cmd++;

	/* Strip trailing blanks. */
	len = strlen(cmd);
	while(len > 0 && (cmd[len-1] == ' ' || cmd[len-1] == '\t'))
		len--;

	/* Anything left at all? */
	if(len > 0)
	{
		UBYTE line[256];
		LONG n = 0;
		UBYTE c;
		LONG i;

		/* Now run down the command string and process the
		 * embedded control strings as they come.
		 */
		for(i = 0 ; i < len ; )
		{
			c = cmd[i];

			if(c == '^')
			{
				i++;

				/* It's a control character. */

				c = ToUpper(cmd[i]);
				if(c != '\0')
				{
					/* If the character to follow the
					 * caret is in the right range,
					 * produce the associated control
					 * character. Otherwise, use the
					 * following character as it is.
					 */
					if('@' <= c && c <= '_')
						c -= '@';
					else
						c = cmd[i];

					i++;
				}
				else
				{
					break; /* for */
				}
			}
			else if (c == '\\')
			{
				i++;

				/* It's a 'C' style escape sequence. */

				c = ToLower(cmd[i]);
				if(c != '\0')
				{
					switch(c)
					{
						/* Alarm */
						case 'a':

							c = '\a';
							i++;

							break;

						/* Backspace */
						case 'b':

							c = '\b';
							i++;

							break;

						/* Form feed */
						case 'f':

							c = '\f';
							i++;

							break;

						/* Line feed */
						case 'n':

							c = '\n';
							i++;

							break;

						/* Carriage return */
						case 'r':

							c = '\r';
							i++;

							break;

						/* Tabulator */
						case 't':

							c = '\t';
							i++;

							break;

						/* Character in hexadecimal encoding */
						case 'x':

							/* If the next following character qualifies as
							 * hexadecimal encoded digit, translate it into
							 * the corresponding number.
							 */
							c = ToUpper(cmd[i+1]);
							if(('0' <= c && c <= '9') || ('A' <= c && c <= 'F'))
							{
								LONG num = 0;
								LONG count = 0;

								i++;

								/* Only two digits are allowed. */
								while(count < 2)
								{
									c = ToUpper(cmd[i]);

									if('0' <= c && c <= '9')
										num = (num * 16) + (c - '0');
									else if ('A' <= c && c <= 'F')
										num = (num * 16) + (c - 'A') + 10;
									else
										break;

									count++;
									i++;
								}

								c = num;
							}
							else
							{
								/* Ignore the \x and the character to follow it. */
								continue; /* for */
							}

							break;

						default:

							/* Is this a character in octal encoding? */
							if('0' <= c && c <= '7')
							{
								LONG num = 0;
								LONG count = 0;

								/* Only three digits are allowed. */
								while(count < 3)
								{
									c = cmd[i];
									if('0' <= c && c <= '7')
									{
										num = (num * 8) + (c - '0');

										count++;
										i++;
									}
									else
									{
										break;
									}
								}

								c = num;
							}
							else
							{
								/* No, it's just a regular character. */
								c = cmd[i++];
							}

							break;
					}
				}
				else
				{
					break;
				}
			}
			else if (c == '~')
			{
				/* It's a short delay. */

				if(n > 0)
				{
					/* Send what's currently in the buffer before
					 * waiting a bit.
					 */
					#if DEBUG
					{
						line[n] = '\0';
						D(("writing line '%s'",line));
					}
					#endif /* DEBUG */

					SerWriteRequest->IOSer.io_Command	= CMD_WRITE;
					SerWriteRequest->IOSer.io_Data		= line;
					SerWriteRequest->IOSer.io_Length	= n;

					DoIO((struct IORequest *)SerWriteRequest);

					n = 0;
				}

				Delay(TICKS_PER_SECOND / 2);

				i++;

				continue; /* for */
			}
			else
			{
				/* One more character eaten. */
				i++;
			}

			line[n++] = c;

			/* If the line buffer is full, write it now. */
			if(n == sizeof(line))
			{
				#if DEBUG
				{
					line[n] = '\0';
					D(("writing line '%s'",line));
				}
				#endif /* DEBUG */

				SerWriteRequest->IOSer.io_Command	= CMD_WRITE;
				SerWriteRequest->IOSer.io_Data		= line;
				SerWriteRequest->IOSer.io_Length	= n;

				DoIO((struct IORequest *)SerWriteRequest);

				n = 0;
			}
		}

		/* If there is still something left in the line buffer,
		 * write it.
		 */
		if(n > 0)
		{
			#if DEBUG
			{
				line[n] = '\0';
				D(("writing line '%s'",line));
			}
			#endif /* DEBUG */

			SerWriteRequest->IOSer.io_Command	= CMD_WRITE;
			SerWriteRequest->IOSer.io_Data		= line;
			SerWriteRequest->IOSer.io_Length	= n;

			DoIO((struct IORequest *)SerWriteRequest);
		}
	}

	LEAVE();
}

/****************************************************************************/

/* Possible return codes for read_line(); return codes greater than zero
 * indicate the length of the line read.
 */
#define READ_LINE_Nothing	0
#define READ_LINE_Abort		-1
#define READ_LINE_Timeout	-2

/****************************************************************************/

/* Read a line of text from the modem, which must be terminated by
 * a carriage return character. Returned is just the text, but none of
 * the control characters, such as line feeds or carriage returns.
 */
LONG
read_line(UBYTE * line,LONG len)
{
	ULONG signal_mask;
	ULONG signals;
	ULONG time_mask;
	ULONG ser_read_mask;
	LONG result = 0;
	LONG n = 0;
	LONG bytes_read;

	ASSERT( line != NULL );

	line[0] = '\0';

	time_mask = PORT_SIG_MASK(TimePort);
	ser_read_mask = PORT_SIG_MASK(SerReadPort);

	signal_mask = SIGBREAKF_CTRL_C | time_mask | ser_read_mask;

	signals = 0;

	/* If we're not reading any data yet and there is none in
	 * the buffer either, start reading now.
	 */
	if(NOT SerReadPending && SerReadLength < (LONG)sizeof(SerReadBuffer))
	{
		SerReadRequest->IOSer.io_Command	= CMD_READ;
		SerReadRequest->IOSer.io_Data		= &SerReadChar;
		SerReadRequest->IOSer.io_Length		= 1;

		SendIO((struct IORequest *)SerReadRequest);

		SerReadPending = TRUE;
	}

	bytes_read = 0;

	while(TRUE)
	{
		/* If there is still data in the read buffer, process it. */
		if(bytes_read < SerReadLength)
		{
			UBYTE c;

			c = SerReadBuffer[bytes_read++];

			/* Room available in the read buffer? Restart the
			 * read request to catch some more data, if necessary.
			 */
			if(bytes_read > 0 && NOT SerReadPending)
			{
				SerReadRequest->IOSer.io_Command	= CMD_READ;
				SerReadRequest->IOSer.io_Data		= &SerReadChar;
				SerReadRequest->IOSer.io_Length		= 1;

				SendIO((struct IORequest *)SerReadRequest);

				SerReadPending = TRUE;
			}

			if(c == '\n')
			{
				n = 0;
			}
			else if (c == '\r')
			{
				/* Skip trailing blanks. */
				while(n > 0 && line[n-1] == ' ')
					n--;

				line[n] = '\0';

				Printf("> %s\n",line);

				/* If there's something in the line buffer,
				 * return it.
				 */
				if(n > 0)
				{
					result = n;
					break;
				}
			}
			else if (n < len-2 && (c != ' ' || n > 0))
			{
				/* If there is still room in the line buffer and
				 * the character just read is printable, keep it.
				 */
				if((' ' <= c && c < 127) || (c >= 160))
					line[n++] = c;
			}

			signals |= SetSignal(0,signal_mask) & signal_mask;
		}
		else
		{
			if(signals == 0)
				signals = Wait(signal_mask);
			else
				signals |= SetSignal(0,signal_mask) & signal_mask;
		}

		if(signals & time_mask)
		{
			struct timerequest * tr;

			tr = (struct timerequest *)GetMsg(TimePort);
			if(tr != NULL)
			{
				ASSERT( tr == TimeRequest );

				TimeTicking = FALSE;

				result = READ_LINE_Timeout;

				break;
			}
			else
			{
				signals &= ~time_mask;
			}
		}

		if(signals & ser_read_mask)
		{
			struct IOExtSer * ior;

			ior = (struct IOExtSer *)GetMsg(SerReadPort);
			if(ior != NULL)
			{
				LONG available;

				ASSERT( ior == SerReadRequest );

				SerReadPending = FALSE;

				/* Now that we've got another byte, dump the data
				 * we already processed before.
				 */
				if(bytes_read > 0)
				{
					memmove(SerReadBuffer,&SerReadBuffer[bytes_read],(size_t)(SerReadLength - bytes_read));
					SerReadLength -= bytes_read;

					bytes_read = 0;
				}

				SerReadBuffer[SerReadLength++] = SerReadChar;

				SerReadRequest->IOSer.io_Command = SDCMD_QUERY;
				DoIO((struct IORequest *)SerReadRequest);

				available = (LONG)SerReadRequest->IOSer.io_Actual;
				if(available > (LONG)sizeof(SerReadBuffer) - SerReadLength)
					available = sizeof(SerReadBuffer) - SerReadLength;

				if(available > 0)
				{
					SerReadRequest->IOSer.io_Command	= CMD_READ;
					SerReadRequest->IOSer.io_Data		= &SerReadBuffer[SerReadLength];
					SerReadRequest->IOSer.io_Length		= available;

					if(DoIO((struct IORequest *)SerReadRequest) == OK)
						SerReadLength += SerReadRequest->IOSer.io_Actual;
				}

				if(SerReadLength < (LONG)sizeof(SerReadBuffer))
				{
					SerReadRequest->IOSer.io_Command	= CMD_READ;
					SerReadRequest->IOSer.io_Data		= &SerReadChar;
					SerReadRequest->IOSer.io_Length		= 1;

					SendIO((struct IORequest *)SerReadRequest);

					SerReadPending = TRUE;
				}
			}
			else
			{
				signals &= ~ser_read_mask;
			}
		}

		if(signals & SIGBREAKF_CTRL_C)
		{
			SHOWMSG("abort!");
			result = READ_LINE_Abort;
			break;
		}
	}

	if(result > 0)
		D(("line = '%s'",line));

	/* Drop the data that was processed last time. */
	if(bytes_read > 0)
	{
		memmove(SerReadBuffer,&SerReadBuffer[bytes_read],(size_t)(SerReadLength - bytes_read));
		SerReadLength -= bytes_read;
	}

	RETURN(result);
	return(result);
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
	struct Message					hm_Message;
	struct Sana2ConnectionMessage *	hm_Connect;
	struct Sana2ConnectionMessage *	hm_Disconnect;
	BOOL							hm_Success;
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
 * initializations, such as dialing out, are necessary to bring it online.
 */
LONG ASM
ConnectHookFunc(
	REG(a0,struct Hook *					unused_hook),
	REG(a2,APTR								unused_reserved),
	REG(a1,struct Sana2ConnectionMessage *	s2cm))
{
	struct HookMessage hm;
	struct MsgPort mp;

	ENTER();

	ASSERT( s2cm != NULL );

	SHOWPOINTER(s2cm);

	/* Set up the local reply port, then compose
	 * the hook message.
	 */
	init_single_port(&mp);

	memset(&hm,0,sizeof(hm));

	hm.hm_Message.mn_ReplyPort	= &mp;
	hm.hm_Message.mn_Length		= sizeof(hm);
	hm.hm_Connect				= s2cm;

	/* Send the hook message to the main program, then wait for
	 * it to be returned.
	 */
	SetSignal(0,SIGF_SINGLE);
	PutMsg(HookPort,(struct Message *)&hm);
	WaitPort(&mp);

	RETURN((LONG)hm.hm_Success);
	return((LONG)hm.hm_Success);
}

/****************************************************************************/

/* This function is invoked when the link layer is to be shut down and
 * further work may be required to do this, such as hanging up the modem
 * line.
 */
VOID ASM
DisconnectHookFunc(
	REG(a0,struct Hook *					unused_hook),
	REG(a2,APTR								unused_reserved),
	REG(a1,struct Sana2ConnectionMessage *	s2cm))
{
	struct HookMessage hm;
	struct MsgPort mp;

	ENTER();

	ASSERT( s2cm != NULL );

	SHOWPOINTER(s2cm);

	/* Set up the local reply port, then compose
	 * the hook message.
	 */
	init_single_port(&mp);

	memset(&hm,0,sizeof(hm));

	hm.hm_Message.mn_ReplyPort	= &mp;
	hm.hm_Message.mn_Length		= sizeof(hm);
	hm.hm_Disconnect			= s2cm;

	/* Send the hook message to the main program, then wait for
	 * it to be returned.
	 */
	SetSignal(0,SIGF_SINGLE);
	PutMsg(HookPort,(struct Message *)&hm);
	WaitPort(&mp);

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
typedef LONG *	NUMBER;
typedef LONG	SWITCH;

/****************************************************************************/

/* This routine reads the configuration file and eventually opens the
 * underlying SANA-II device driver.
 */
BOOL
configure_dialer(STRPTR file_name,STRPTR interface_name,LONG interface_name_len)
{
	struct
	{
		KEY		Interface;
		KEY		Device;
		NUMBER	Unit;
		NUMBER	Speed;
		NUMBER	NumWriteRequests;
		NUMBER	NumReadRequests;
		NUMBER	BufferSize;
		KEY		CheckCarrier;
		KEY		HardwareHandshaking;
		KEY		Shared;
		KEY		NullModem;
		KEY		EOFMode;

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
		KEY		ACCM;
		KEY		PFC;
		KEY		AACFC;
		KEY		VJHC;
		KEY		IgnoreFCS;
		KEY		SetEnv;
		KEY		SendID;
		KEY		RejectPAP;
		NUMBER	PAPTimeout;
		NUMBER	PAPRetry;
		KEY		DummyRemoteAddress;

		KEY		Init;
		KEY		Dial;
		NUMBER	DialTimeout;
		KEY		Login;
		KEY		Password;
		KEY		Hangup;

		KEY		LogFile;
		KEY		Log;
	} args;

	STRPTR args_template =
		"INTERFACE/K,"
		"DEVICE/K,"
		"UNIT/K/N,"
		"BPS=SPEED/K/N,"
		"WRITEREQUESTS/K/N,"
		"READREQUESTS/K/N,"
		"BUFFERSIZE/K/N,"
		"CD=CHECKCARRIER/K,"
		"7WIRE=RTSCTS/K,"
		"SHARED/K,"
		"NULLMODEM/K,"
		"EOF/K,"
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
		"ACCM/K,"
		"PFC/K,"
		"AACFC/K,"
		"VJHC/K,"
		"IGNOREFCS/K,"
		"SETENV/K,"
		"SENDID/K,"
		"REJECTPAP/K,"
		"PAPTIMEOUT/K/N,"
		"PAPRETRY/K/N,"
		"DUMMYREMOTEADDRESS/K,"
		"INIT/K,"
		"DIAL/K,"
		"DIALTIMEOUT/K/N,"
		"LOGIN/K,"
		"PASSWORD/K,"
		"HANGUP/K,"
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
			if(CANNOT add_option_str("ppp.async.device",args.Device))
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
			if(CANNOT add_option_num("ppp.async.unit",(*args.Unit)))
			{
				Printf("ERROR: Not enough memory to add option '%s=%s'.\n",
					"UNIT",(*args.Unit));

				goto out;
			}
		}

		/* Transmission speed. */
		if(args.Speed != NULL)
		{
			if((*args.Speed) > 0)
			{
				if(CANNOT add_option_num("ppp.async.speed",(*args.Speed)))
				{
					Printf("ERROR: Not enough memory to add option '%s=%ld'.\n",
						"SPEED",(*args.Speed));

					goto out;
				}
			}
			else
			{
				Printf("Warning: '%s=%ld' in line %ld of '%s' ignored.\n",
					"SPEED",(*args.Speed),line_number,file_name);
			}
		}

		/* Size of receive buffer */
		if(args.BufferSize != NULL)
		{
			if((*args.BufferSize) > 0)
			{
				if(CANNOT add_option_num("ppp.async.buffersize",(*args.BufferSize)))
				{
					Printf("ERROR: Not enough memory to add option '%s=%ld'.\n",
						"BUFFERSIZE",(*args.BufferSize));

					goto out;
				}
			}
			else
			{
				Printf("Warning: '%s=%ld' in line %ld of '%s' ignored.\n",
					"BUFFERSIZE",(*args.BufferSize),line_number,file_name);
			}
		}

		/* Test for the presence of the carrier signal? */
		if(args.CheckCarrier != NULL)
		{
			LONG value;

			value = match_key(args.CheckCarrier);
			if(value != -1)
			{
				if(CANNOT add_option_num("ppp.async.checkcarrier",value))
				{
					Printf("ERROR: Not enough memory to add option '%s=%s'.\n",
						"CHECKCARRIER",args.CheckCarrier);

					goto out;
				}
			}
			else
			{
				Printf("Warning: unknown key '%s=%s' in line %ld of '%s'.\n",
					"CHECKCARRIER",args.CheckCarrier,line_number,file_name);
			}
		}

		/* Use hardware handshaking? */
		if(args.HardwareHandshaking != NULL)
		{
			LONG value;

			value = match_key(args.HardwareHandshaking);
			if(value != -1)
			{
				if(CANNOT add_option_num("ppp.async.rtscts",value))
				{
					Printf("ERROR: Not enough memory to add option '%s=%s'.\n",
						"RTSCTS",args.HardwareHandshaking);

					goto out;
				}
			}
			else
			{
				Printf("Warning: unknown key '%s=%s' in line %ld of '%s'.\n",
					"RTSCTS",args.HardwareHandshaking,line_number,file_name);
			}
		}

		/* Use shared access to the serial device driver? */
		if(args.Shared != NULL)
		{
			LONG value;

			value = match_key(args.Shared);
			if(value != -1)
			{
				if(CANNOT add_option_num("ppp.async.shared",value))
				{
					Printf("ERROR: Not enough memory to add option '%s=%s'.\n",
						"SHARED",args.Shared);

					goto out;
				}
			}
			else
			{
				Printf("Warning: unknown key '%s=%s' in line %ld of '%s'.\b",
					"SHARED",args.Shared,line_number,file_name);
			}
		}

		/* Assume a null modem connection? */
		if(args.NullModem != NULL)
		{
			LONG value;

			value = match_key(args.NullModem);
			if(value != -1)
			{
				if(CANNOT add_option_num("ppp.async.nullmodem",value))
				{
					Printf("ERROR: Not enough memory to add option '%s=%s'.\n",
						"NULLMODEM",args.NullModem);

					goto out;
				}
			}
			else
			{
				Printf("Warning: unknown key '%s=%s' in line %ld of '%s'.\n",
					"NULLMODEM",args.NullModem,line_number,file_name);
			}
		}

		/* Enable EOF mode and try to read complete frames off the wire? */
		if(args.EOFMode != NULL)
		{
			LONG value;

			value = match_key(args.EOFMode);
			if(value != -1)
			{
				if(CANNOT add_option_num("ppp.async.eof",value))
				{
					Printf("ERROR: Not enough memory to add option '%s=%s'.\n",
						"EOF",args.EOFMode);

					goto out;
				}
			}
			else
			{
				Printf("Warning: unknown key '%s=%s' in line %ld of '%s'.\n",
					"EOF",args.EOFMode,line_number,file_name);
			}
		}

		/* Set the number of write requests? */
		if(args.NumWriteRequests != NULL)
		{
			if((*args.NumWriteRequests) > 0)
			{
				if(CANNOT add_option_num("ppp.async.writerequests",(*args.NumWriteRequests)))
				{
					Printf("ERROR: Not enough memory to add option '%s=%ld'.\n",
						"WRITEREQUESTS",(*args.NumWriteRequests));

					goto out;
				}
			}
			else
			{
				Printf("Warning: '%s=%ld' in line %ld of '%s' ignored.\n",
					"WRITEREQUESTS",(*args.NumWriteRequests),line_number,file_name);
			}
		}

		/* Set the number of read requests? */
		if(args.NumReadRequests != NULL)
		{
			if((*args.NumReadRequests) > 0)
			{
				if(CANNOT add_option_num("ppp.async.readrequests",(*args.NumReadRequests)))
				{
					Printf("ERROR: Not enough memory to add option '%s=%ld'.\n",
						"READREQUESTS",(*args.NumReadRequests));

					goto out;
				}
			}
			else
			{
				Printf("Warning: '%s=%ld' in line %ld of '%s' ignored.\n",
					"READREQUESTS",(*args.NumReadRequests),line_number,file_name);
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

		/* Set the async control character map? */
		if(args.ACCM != NULL)
		{
			if(CANNOT add_option_str("ppp.async.accm",args.ACCM))
			{
				Printf("ERROR: Not enough memory to add option '%s=%s'.\n",
					"ACCM",args.ACCM);

				goto out;
			}
		}

		/* Configure protocol field compression? */
		if(args.PFC != NULL)
		{
			LONG value;

			value = match_key(args.PFC);
			if(value != -1)
			{
				if(CANNOT add_option_num("ppp.async.pfc",value))
				{
					Printf("ERROR: Not enough memory to add option '%s=%s'.\n",
						"PFC",args.PFC);

					goto out;
				}
			}
			else
			{
				Printf("Warning: unknown key '%s=%s' in line %ld of '%s'.\n",
					"PFC",args.PFC,line_number,file_name);
			}
		}

		/* Configure address and control field compression? */
		if(args.AACFC != NULL)
		{
			LONG value;

			value = match_key(args.AACFC);
			if(value != -1)
			{
				if(CANNOT add_option_num("ppp.async.aacfc",value))
				{
					Printf("ERROR: Not enough memory to add option '%s=%s'.\n",
						"AACFC",args.AACFC);

					goto out;
				}
			}
			else
			{
				Printf("Warning: unknown key '%s=%s' in line %ld of '%s'.\n",
					"AACFC",args.AACFC,line_number,file_name);
			}
		}

		/* Configure Van Jacobson header compression? */
		if(args.VJHC != NULL)
		{
			LONG value;

			value = match_key(args.VJHC);
			if(value != -1)
			{
				if(CANNOT add_option_num("ppp.async.vjhc",value))
				{
					Printf("ERROR: Not enough memory to add option '%s=%s'.\n",
						"VJHC",args.VJHC);

					goto out;
				}
			}
			else
			{
				Printf("Warning: unknown key '%s=%s' in line %ld of '%s'.\n",
					"VJHC",args.VJHC,line_number,file_name);
			}
		}

		/* Ignore the frame check sequence? */
		if(args.IgnoreFCS != NULL)
		{
			LONG value;

			value = match_key(args.IgnoreFCS);
			if(value != -1)
			{
				if(CANNOT add_option_num("ppp.async.ignorefcs",value))
				{
					Printf("ERROR: Not enough memory to add option '%s=%s'.\n",
						"IGNOREFCS",args.IgnoreFCS);

					goto out;
				}
			}
			else
			{
				Printf("Warning: unknown key '%s=%s' in line %ld of '%s'.\n",
					"IGNOREFCS",args.IgnoreFCS,line_number,file_name);
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

		/* Choose the modem init command? */
		if(args.Init != NULL)
		{
			if(CANNOT add_option_str("ppp.async.initialize",args.Init))
			{
				Printf("ERROR: Not enough memory to add option '%s=%s'.\n",
					"INIT",args.Init);

				goto out;
			}
		}

		/* Choose the modem dial command? */
		if(args.Dial != NULL)
		{
			if(CANNOT add_option_str("ppp.async.dial",args.Dial))
			{
				Printf("ERROR: Not enough memory to add option '%s=%s'.\n",
					"DIAL",args.Dial);

				goto out;
			}
		}

		/* Choose the modem hangup command? */
		if(args.Hangup != NULL)
		{
			if(CANNOT add_option_str("ppp.async.hangup",args.Hangup))
			{
				Printf("ERROR: Not enough memory to add option '%s=%s'.\n",
					"HANGUP",args.Hangup);

				goto out;
			}
		}

		/* Set the dial timeout? */
		if(args.DialTimeout != NULL)
		{
			if((*args.DialTimeout) > 0)
			{
				if(CANNOT add_option_num("ppp.async.dialtimeout",(*args.DialTimeout)))
				{
					Printf("ERROR: Not enough memory to add option '%s=%s'.\n",
						"DIALTIMEOUT",(*args.DialTimeout));

					goto out;
				}
			}
			else
			{
				Printf("Warning: '%s=%ld' in line %ld of '%s' ignored.\n",
					"DIALTIMEOUT",(*args.DialTimeout),line_number,file_name);
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

/* If the timer is currently running, stop it. */
void
stop_timer(void)
{
	if(TimeTicking)
	{
		if(CheckIO((struct IORequest *)TimeRequest) == BUSY)
			AbortIO((struct IORequest *)TimeRequest);

		WaitIO((struct IORequest *)TimeRequest);

		TimeTicking = FALSE;
	}
}

/* Start the timer so that it may signal a timeout. */
void
start_timer(LONG seconds)
{
	ENTER();

	ASSERT( seconds > 0 );

	stop_timer();

	TimeRequest->tr_node.io_Command	= TR_ADDREQUEST;
	TimeRequest->tr_time.tv_secs	= seconds;
	TimeRequest->tr_time.tv_micro	= 0;

	SendIO((struct IORequest *)TimeRequest);
	TimeTicking = TRUE;

	LEAVE();
}

/****************************************************************************/

/* Take a look at the connection message, then perform the
 * necessary actions to bring the modem online.
 */
BOOL
connect_interface(struct Sana2ConnectionMessage * s2cm)
{
	BOOL success = FALSE;
	LONG nullmodem;
	LONG dial_timeout;
	STRPTR init_command;
	STRPTR dial_command;

	ENTER();

	ASSERT( s2cm != NULL );

	/* Size of the connection message valid? */
	if(s2cm->s2cm_Size < 20)
	{
		D(("size of connection message %ld is too small",s2cm->s2cm_Size));
		goto out;
	}

	/* This has to be a "serial.device" type device driver. */
	if(s2cm->s2cm_RequestType != NSDEVTYPE_SERIAL)
	{
		D(("I/O requests are of type %ld, and not of NSDTYPE_SERIAL",s2cm->s2cm_RequestType));
		goto out;
	}

	/* Don't do anything if this is a nullmodem connection. */
	if(find_s2c_option_num(s2cm->s2cm_Connection,"ppp.async.nullmodem",&nullmodem) && (nullmodem != 0))
	{
		SHOWMSG("no dialing necessary; nullmodem connection");
		success = TRUE;
		goto out;
	}

	/* Pick up the configured dial timeout. */
	if(CANNOT find_s2c_option_num(s2cm->s2cm_Connection,"ppp.async.dialtimeout",&dial_timeout) || dial_timeout <= 0)
		dial_timeout = 60;

	/* Make copies of the two I/O requests provided. */
	ASSERT(s2cm->s2cm_Request[0]->io_Message.mn_Length == sizeof(*SerWriteRequest));
	memcpy(SerWriteRequest,s2cm->s2cm_Request[0],sizeof(*SerWriteRequest));
	SerWriteRequest->IOSer.io_Message.mn_ReplyPort = SerWritePort;

	ASSERT(s2cm->s2cm_Request[1]->io_Message.mn_Length == sizeof(*SerReadRequest));
	memcpy(SerReadRequest,s2cm->s2cm_Request[1],sizeof(*SerReadRequest));
	SerReadRequest->IOSer.io_Message.mn_ReplyPort = SerReadPort;

	/* Throw away any buffered modem responses, such as a previous 'OK'
	 * sent in response to a hangup command.
	 */
	SerWriteRequest->IOSer.io_Command = CMD_CLEAR;
	DoIO((struct IORequest *)SerWriteRequest);

	/* Is there a modem initialization command that needs to be sent? */
	if(find_s2c_option_str(s2cm->s2cm_Connection,"ppp.async.initialize",&init_command) && init_command[0] != '\0')
	{
		UBYTE line[256];
		LONG n;

		SHOWMSG("initializing modem");

		/* Start the timer so that we know when we should
		 * consider the initialization to have failed.
		 */
		start_timer(dial_timeout);

		send_command(init_command);

		/* Read the lines the modem returns to us. We stop
		 * when we've hit an error code or a confirmation.
		 * Also possible are an abort signal or a timeout.
		 */
		while((n = read_line(line,sizeof(line))) > 0)
		{
			if(strncmp(line,"ERROR",5) == SAME)
			{
				Printf("ERROR: Modem initialization command returned '%s'\n.",line);
				goto out;
			}

			if(strcmp(line,"OK") == SAME)
				break;
		}

		switch(n)
		{
			case READ_LINE_Abort:

				Printf("ERROR: Modem initialization command aborted.\n");
				goto out;

			case READ_LINE_Timeout:

				Printf("ERROR: Modem initialization command timed out.\n");
				goto out;
		}

		/* Stop the timer, if it's still running. */
		stop_timer();
	}

	/* Now for the dial command... */
	if(find_s2c_option_str(s2cm->s2cm_Connection,"ppp.async.dial",&dial_command) && dial_command[0] != '\0')
	{
		STATIC STRPTR error_responses[] =
		{
			"ERROR",
			"BUSY",
			"NO DIAL", /* NOTE: this covers both 'NO DIALTONE' and 'NO DIAL TONE' */
			"NO ANSWER",
			"NO CARRIER"
		};

		UBYTE line[256];
		LONG n;
		LONG i;

		SHOWMSG("dialing...");

		/* Start the timer so that we know when we should
		 * consider the dial command to have failed.
		 */
		start_timer(dial_timeout);

		send_command(dial_command);

		/* Read the lines the modem returns to us. We stop
		 * when we've hit an error code or a connection
		 * establishment report. Also possible are an abort
		 * signal or a timeout.
		 */
		while((n = read_line(line,sizeof(line))) > 0)
		{
			for(i = 0 ; i < (LONG)NUM_ENTRIES(error_responses) ; i++)
			{
				if(strncmp(line,error_responses[i],strlen(error_responses[i])) == SAME)
				{
					Printf("ERROR: Modem dial command returned '%s'.\n",line);
					goto out;
				}
			}

			if(strncmp(line,"CONNECT",7) == SAME)
				break;
		}

		switch(n)
		{
			case READ_LINE_Abort:

				Printf("ERROR: Modem dial command aborted.\n");
				goto out;

			case READ_LINE_Timeout:

				Printf("ERROR: Modem dial command timed out.\n");
				goto out;
		}
	}

	success = TRUE;

 out:

	/* We have to clean up after ourselves and make sure that the
	 * modem read request is not still pending when we return
	 * from this routine.
	 */
	if(SerReadPending)
	{
		if(CheckIO((struct IORequest *)SerReadRequest) == BUSY)
			AbortIO((struct IORequest *)SerReadRequest);

		WaitIO((struct IORequest *)SerReadRequest);

		SerReadPending = FALSE;
	}

	/* Stop the timer, if it's still running. */
	stop_timer();

	RETURN(success);
	return(success);
}

/****************************************************************************/

/* Take a look at the connection message, then perform the
 * necessary actions to take the modem offline.
 */
VOID
disconnect_interface(struct Sana2ConnectionMessage * s2cm)
{
	STRPTR hangup_command;
	LONG nullmodem;

	ENTER();

	ASSERT( s2cm != NULL );

	if(s2cm->s2cm_Size < 20)
	{
		D(("size of connection message %ld is too small",s2cm->s2cm_Size));
		goto out;
	}

	if(s2cm->s2cm_RequestType != NSDEVTYPE_SERIAL)
	{
		D(("I/O requests are of type %ld, and not of NSDTYPE_SERIAL",s2cm->s2cm_RequestType));
		goto out;
	}

	if(find_s2c_option_num(s2cm->s2cm_Connection,"ppp.async.nullmodem",&nullmodem) && (nullmodem != 0))
	{
		SHOWMSG("no hangup command sent; nullmodem connection");
		goto out;
	}

	/* If no hangup command is provided, use the default. */
	if(CANNOT find_s2c_option_str(s2cm->s2cm_Connection,"ppp.async.hangup",&hangup_command))
		hangup_command = "~~~+++~~~ATH0\\r";

	SHOWSTRING(hangup_command);

	if(hangup_command[0] != '\0')
	{
		UBYTE line[256];
		LONG n;

		SHOWMSG("sending hangup command");

		/* Make copies of the two I/O requests provided. */
		ASSERT(s2cm->s2cm_Request[0]->io_Message.mn_Length == sizeof(*SerWriteRequest));
		memcpy(SerWriteRequest,s2cm->s2cm_Request[0],sizeof(*SerWriteRequest));
		SerWriteRequest->IOSer.io_Message.mn_ReplyPort = SerWritePort;

		ASSERT(s2cm->s2cm_Request[1]->io_Message.mn_Length == sizeof(*SerReadRequest));
		memcpy(SerReadRequest,s2cm->s2cm_Request[1],sizeof(*SerReadRequest));
		SerReadRequest->IOSer.io_Message.mn_ReplyPort = SerReadPort;

		/* Throw away any buffered modem responses. */
		SerWriteRequest->IOSer.io_Command = CMD_CLEAR;
		DoIO((struct IORequest *)SerWriteRequest);

		start_timer(10);

		/* Send the hangup command and ignore the results. */
		send_command(hangup_command);

		/* Read the lines the modem returns to us. We stop
		 * when we've hit an error code or a confirmation.
		 * Also possible are an abort signal or a timeout.
		 */
		while((n = read_line(line,sizeof(line))) > 0)
		{
			if(strcmp(line,"OK") == SAME ||
			   strcmp(line,"ERROR") == SAME ||
			   strcmp(line,"NO CARRIER") == SAME)
			{
				break;
			}
		}
	}

 out:

	/* We have to clean up after ourselves and make sure that the
	 * modem read request is not still pending when we return
	 * from this routine.
	 */
	if(SerReadPending)
	{
		if(CheckIO((struct IORequest *)SerReadRequest) == BUSY)
			AbortIO((struct IORequest *)SerReadRequest);

		WaitIO((struct IORequest *)SerReadRequest);

		SerReadPending = FALSE;
	}

	/* Stop the timer, if it's still running. */
	stop_timer();

	LEAVE();
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

enum dial_result_t
{
	dial_result_no_connection,
	dial_result_passive_offline,
	dial_result_active_offline
};

/****************************************************************************/

/* Tell the interface to establish a connection, wait for it to go
 * online, then wait for the user to indicate that he wants to go
 * offline again, and eventually take the interface offline again.
 */
enum dial_result_t
dial(STRPTR interface)
{
	struct Sana2Connection * s2c = &Sana2Connection;
	ULONG net_event_mask;
	ULONG net_mask;
	ULONG hook_mask;
	ULONG signal_mask;
	ULONG signals;
	enum dial_result_t result = dial_result_no_connection;
	BOOL net_command_pending;
	BOOL connected;

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
					/* Did this work out well? */
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

				if(result == dial_result_no_connection)
					result = dial_result_passive_offline;

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
				else if (hm->hm_Connect != NULL)
				{
					SHOWMSG("connect_interface");

					/* We have to dial out. */
					hm->hm_Success = connect_interface(hm->hm_Connect);

					ReplyMsg((struct Message *)hm);
				}
				else if (hm->hm_Disconnect != NULL)
				{
					SHOWMSG("disconnect_interface");

					/* We have to take the modem offline. */
					disconnect_interface(hm->hm_Disconnect);

					ReplyMsg((struct Message *)hm);
				}
				else
				{
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

				if(result == dial_result_no_connection)
					result = dial_result_active_offline;
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
			SWITCH	Redial;
		} args;

		struct RDArgs * rda;

		memset(&args,0,sizeof(args));

		rda = ReadArgs("NAME/A,REDIAL/S",(LONG *)&args,NULL);
		if(rda != NULL)
		{
			UBYTE interface[40];

			if(configure_dialer(args.Name,interface,sizeof(interface)))
			{
				BOOL done;

				do
				{
					done = TRUE;

					switch(dial(interface))
					{
						case dial_result_no_connection:

							result = RETURN_WARN;
							break;

						case dial_result_active_offline:

							result = RETURN_OK;
							break;

						case dial_result_passive_offline:

							if(args.Redial)
							{
								Printf("Redialing...\n");

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
			PrintFault(IoErr(),"PPP_Dialer");
		}
	}

	cleanup();

	return(result);
}
