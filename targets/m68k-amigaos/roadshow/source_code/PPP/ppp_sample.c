/*
 *	$Id: ppp_sample.c,v 1.6 2005/01/07 13:32:03 obarthel Exp $
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
#include <devices/timer.h>

#include <graphics/gfxbase.h>

#include <intuition/intuition.h>
#include <intuition/intuitionbase.h>

/*****************************************************************************/

#include <clib/alib_protos.h>

/*****************************************************************************/

#define __NOLIBBASE__
#define __NOGLOBALIFACE__
#define __USE_INLINE__

/*****************************************************************************/

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/timer.h>

/*****************************************************************************/

#include <string.h>
#include <stdarg.h>
#include <stddef.h>

/****************************************************************************/

#include "ppp_sample_rev.h"
STRPTR Version = VERSTAG;

/****************************************************************************/

#include "macros.h"

/****************************************************************************/

/*#define DEBUG*/
#include "assert.h"

/****************************************************************************/

#if defined(__amigaos4__)
#define VAR_ARGS __attribute__((linearvarargs))
#else
#define VAR_ARGS
#endif /* __amigaos4__ */

/****************************************************************************/

#define MILLION 1000000

/****************************************************************************/

extern struct Library *		SysBase;
extern struct Library *		DOSBase;

/****************************************************************************/

struct IntuitionBase *		IntuitionBase;
struct GfxBase *			GfxBase;

/****************************************************************************/

struct Device *				TimerBase;
struct MsgPort *			TimePort;
struct timerequest * 		TimeRequest;

/****************************************************************************/

#if defined(__amigaos4__)

extern struct ExecIFace *	IExec;
extern struct DOSIFace *	IDOS;

struct IntuitionIFace *		IIntuition;
struct GraphicsIFace *		IGraphics;
struct TimerIFace *			ITimer;

#endif /* __amigaos4__ */

/****************************************************************************/

UBYTE						Title[400];
struct Window *				Window;
struct DrawInfo *			DrawInfo;
struct TextFont *			Font;

/****************************************************************************/

struct MsgPort *			NetPort;
struct IOSana2Req *			NetRequest;
struct IOSana2Req *			NetStatRequest;
BOOL						NetStatPending;
struct Sana2ThroughputStats	NetStat;
struct Sana2ThroughputStats	NetStatLast;
ULONG						MaxSent;
ULONG						MaxSentReceived;
ULONG						MaxReceived;
ULONG						LastSent;
ULONG						LastReceived;
ULONG						TotalReceived;
ULONG						TotalSent;
ULONG						TotalCounted;
ULONG						TotalSeconds;

/****************************************************************************/

ULONG *						HistoryReceived;
ULONG *						HistorySent;
LONG						HistorySize;

/****************************************************************************/

STRPTR get_io_error_string(LONG error, UBYTE *buffer);
STRPTR get_wire_error_string(LONG error, UBYTE *buffer);
VOID VAR_ARGS local_sprintf(STRPTR buffer, STRPTR formatString, ...);
VOID cleanup(void);
BOOL setup(void);
void print_stats(void);
void clear_display(void);
void redraw_display(void);
void update_display(BOOL can_draw);

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
			local_sprintf(buffer,"Unknown I/O error %ld",error);
		else
			local_sprintf(buffer,"Unknown SANA-II error %ld",error);
	}

	return(buffer);
}

/****************************************************************************/

/* Return a descriptive text for an wire error code returned
 * by a SANA-II device.
 */
STRPTR
get_wire_error_string(LONG error,UBYTE * buffer)
{
	STATIC struct { LONG val; STRPTR str; } tab[] =
	{
		{ S2WERR_GENERIC_ERROR,		"No specific information available" },
		{ S2WERR_NOT_CONFIGURED,	"Unit is not configured" },
		{ S2WERR_UNIT_ONLINE,		"Unit is currently online" },
		{ S2WERR_UNIT_OFFLINE,		"Unit is currently offline" },
		{ S2WERR_ALREADY_TRACKED,	"Protocol is already tracked" },
		{ S2WERR_NOT_TRACKED,		"Protocol is not tracked" },
		{ S2WERR_BUFF_ERROR,		"Buffer management function returned an error" },
		{ S2WERR_SRC_ADDRESS,		"Source address problem" },
		{ S2WERR_DST_ADDRESS,		"Destination address problem" },
		{ S2WERR_BAD_BROADCAST,		"Broadcast address problem" },
		{ S2WERR_BAD_MULTICAST,		"Multicast address problem" },
		{ S2WERR_MULTICAST_FULL,	"Multicast address list is full" },
		{ S2WERR_BAD_EVENT,			"Unsupported event class" },
		{ S2WERR_BAD_STATDATA,		"StatData failed sanity check" },
		{ S2WERR_IS_CONFIGURED,		"Attempted to configure twice" },
		{ S2WERR_NULL_POINTER,		"NULL pointer detected" },
		{ S2WERR_TOO_MANY_RETRIES,	"Transmission failed - too many retries" },
		{ S2WERR_RCVREL_HDW_ERR,	"Driver fixable hardware error" }
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
		local_sprintf(buffer,"Unknown wire error %ld",error);

	return(buffer);
}

/****************************************************************************/

VOID VAR_ARGS
local_sprintf(STRPTR buffer, STRPTR formatString,...)
{
	va_list varArgs;

	#if defined(__amigaos4__)
	{
		va_startlinear(varArgs,formatString);
		RawDoFmt(formatString,va_getlinearva(varArgs, APTR),NULL,buffer);
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

VOID
cleanup(void)
{
	if(HistoryReceived != NULL)
	{
		FreeVec(HistoryReceived);
		HistoryReceived = NULL;
	}

	if(DrawInfo != NULL)
	{
		FreeScreenDrawInfo(Window->WScreen,DrawInfo);
		DrawInfo = NULL;
	}

	if(Window != NULL)
	{
		CloseWindow(Window);
		Window = NULL;
	}

	if(NetStatPending)
	{
		if(CheckIO((struct IORequest *)NetStatRequest) == BUSY)
			AbortIO((struct IORequest *)NetStatRequest);

		WaitIO((struct IORequest *)NetStatRequest);

		NetStatPending = FALSE;
	}

	if(NetStatRequest != NULL)
	{
		DeleteIORequest((struct IORequest *)NetStatRequest);
		NetStatRequest = NULL;
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

	#if defined(__amigaos4__)
	{
		if(ITimer != NULL)
		{
			DropInterface((struct Interface *)ITimer);
			ITimer = NULL;
		}
	}
	#endif /* __amigaos4__ */

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

	if(Font != NULL)
	{
		CloseFont(Font);
		Font = NULL;
	}

	#if defined(__amigaos4__)
	{
		if(IGraphics != NULL)
		{
			DropInterface((struct Interface *)IGraphics);
			IGraphics = NULL;
		}
	}
	#endif /* __amigaos4__ */

	if(GfxBase != NULL)
	{
		CloseLibrary((struct Library *)GfxBase);
		GfxBase = NULL;
	}

	#if defined(__amigaos4__)
	{
		if(IIntuition != NULL)
		{
			DropInterface((struct Interface *)IIntuition);
			IIntuition = NULL;
		}
	}
	#endif /* __amigaos4__ */

	if(IntuitionBase != NULL)
	{
		CloseLibrary((struct Library *)IntuitionBase);
		IntuitionBase = NULL;
	}
}

/****************************************************************************/

BOOL
setup(void)
{
	STATIC struct TextAttr topaz =
	{
		"topaz.font",
		8,
		FS_NORMAL,
		FPF_ROMFONT|FPF_DESIGNED
	};

	struct
	{
		STRPTR	Name;
		LONG *	Unit;
		LONG *	Left;
		LONG *	Top;
		LONG *	Width;
		LONG *	Height;
	} args;

	STRPTR name;
	LONG unit;
	struct RDArgs * rda = NULL;
	BOOL success = FALSE;
	LONG left,top,width,height;
	LONG error;

	if(SysBase->lib_Version < 36)
		goto out;

	IntuitionBase = (struct IntuitionBase *)OpenLibrary("intuition.library",37);
	if(IntuitionBase == NULL)
	{
		Printf("Could not open 'intuition.library' V37.\n");
		goto out;
	}

	#if defined(__amigaos4__)
	{
		IIntuition = (struct IntuitionIFace *)GetInterface((struct Library *)IntuitionBase, "main", 1, NULL);
		if(IIntuition == NULL)
		{
			Printf("Could not obtain 'intuition.library' interface.\n");
			goto out;
		}
	}
	#endif /* __amigaos4__ */

	GfxBase = (struct GfxBase *)OpenLibrary("graphics.library",37);
	if(GfxBase == NULL)
	{
		Printf("Could not open 'graphics.library' V37.\n");
		goto out;
	}

	#if defined(__amigaos4__)
	{
		IGraphics = (struct GraphicsIFace *)GetInterface((struct Library *)GfxBase, "main", 1, NULL);
		if(IGraphics == NULL)
		{
			Printf("Could not obtain 'graphics.library' interface.\n");
			goto out;
		}
	}
	#endif /* __amigaos4__ */

	memset(&args,0,sizeof(args));

	rda = ReadArgs("NAME/A,UNIT/N,LEFT/N,TOP/N,WIDTH/N,HEIGHT/N",(LONG *)&args,NULL);
	if(rda == NULL)
	{
		PrintFault(IoErr(),"PPP_Sample");
		goto out;
	}

	name = args.Name;
	if(args.Unit != NULL)
		unit = (*args.Unit);
	else
		unit = 0;

	if(args.Left != NULL)
		left = (*args.Left);
	else
		left = -1;

	if(args.Top != NULL)
		top = (*args.Top);
	else
		top = -1;

	if(args.Width != NULL)
		width = (*args.Width);
	else
		width = 300;

	if(args.Height != NULL)
		height = (*args.Height);
	else
		height = 50;

	if(width < 30)
		width = 30;

	if(height < 40)
		height = 40;

	TimePort = CreateMsgPort();
	if(TimePort == NULL)
	{
		Printf("Could not create timer message port.\n");
		goto out;
	}

	TimeRequest = (struct timerequest *)CreateIORequest(TimePort,sizeof(*TimeRequest));
	if(TimeRequest == NULL)
	{
		Printf("Could not create timer I/O request.\n");
		goto out;
	}

	if(OpenDevice(TIMERNAME,UNIT_VBLANK,(struct IORequest *)TimeRequest,0) != OK)
	{
		Printf("Could not open 'timer.device'.\n");
		goto out;
	}

	TimerBase = TimeRequest->tr_node.io_Device;

	#if defined(__amigaos4__)
	{
		ITimer = (struct TimerIFace *)GetInterface((struct Library *)TimerBase, "main", 1, NULL);
		if(ITimer == NULL)
		{
			Printf("Could not obtain 'timer.device' interface.\n");
			goto out;
		}
	}
	#endif /* __amigaos4__ */

	NetPort = CreateMsgPort();
	if(NetPort == NULL)
	{
		Printf("Could not create net I/O message port.\n");
		goto out;
	}

	NetRequest = (struct IOSana2Req *)CreateIORequest(NetPort,sizeof(*NetRequest));
	if(NetRequest == NULL)
	{
		Printf("Could not create net I/O request.\n");
		goto out;
	}

	error = OpenDevice(name,unit,(struct IORequest *)NetRequest,0);
	if(error != OK)
	{
		UBYTE io_error_string[100];

		Printf("Could not open '%s', unit %ld (%ld, %s).\n",
			name,unit,error,get_io_error_string(error,io_error_string));

		goto out;
	}

	NetStatRequest = (struct IOSana2Req *)CreateIORequest(NetPort,sizeof(*NetRequest));
	if(NetStatRequest == NULL)
	{
		Printf("Could not create net I/O request.\n");
		goto out;
	}

	memcpy(NetStatRequest,NetRequest,sizeof(*NetRequest));

	memset(&NetStat,0,sizeof(NetStat));
	NetStat.s2ts_Length = sizeof(NetStat);

	NetStatRequest->ios2_Req.io_Command	= S2_SAMPLE_THROUGHPUT;
	NetStatRequest->ios2_StatData		= &NetStat;

	SendIO((struct IORequest *)NetStatRequest);
	NetStatPending = TRUE;

	Font = OpenFont(&topaz);
	if(Font == NULL)
	{
		Printf("Could not open default font topaz/8.\n");
		goto out;
	}

	local_sprintf(Title,"PPP Sample (%s/%ld)",FilePart(name),unit);

	Window = OpenWindowTags(NULL,
		WA_Title,			Title,
		WA_SizeGadget,		TRUE,
		WA_DragBar,			TRUE,
		WA_DepthGadget,		TRUE,
		WA_CloseGadget,		TRUE,
		WA_NoCareRefresh,	TRUE,
		WA_RMBTrap,			TRUE,
		WA_SmartRefresh,	TRUE,
		WA_SizeBRight,		TRUE,
		(left >= 0) ? WA_Left : TAG_IGNORE,left,
		(top >= 0) ? WA_Top : TAG_IGNORE,top,
		WA_InnerWidth,		width,
		WA_InnerHeight,		height,
		WA_IDCMP,			IDCMP_CLOSEWINDOW | IDCMP_NEWSIZE | IDCMP_VANILLAKEY,
	TAG_END);
	if(Window == NULL)
	{
		Printf("Could not open window.\n");
		goto out;
	}

	HistorySize = Window->WScreen->Width;

	HistoryReceived = AllocVec(HistorySize * 2 * sizeof(ULONG),MEMF_ANY|MEMF_CLEAR);
	if(HistoryReceived)
		HistorySent = HistoryReceived + HistorySize;
	else
		HistorySize = 0;

	WindowLimits(Window,
		Window->BorderLeft + 10 + Window->BorderRight,
		Window->BorderTop + 20 + Font->tf_YSize+1 + Window->BorderBottom,
		Window->WScreen->Width,
		Window->WScreen->Height);

	DrawInfo = GetScreenDrawInfo(Window->WScreen);
	if(DrawInfo == NULL)
	{
		Printf("Could not get screen drawing information.\n");
		goto out;
	}

	SetBPen(Window->RPort,DrawInfo->dri_Pens[BACKGROUNDPEN]);
	SetDrMd(Window->RPort,JAM2);

	SetFont(Window->RPort,Font);

	clear_display();

	success = TRUE;

 out:

	if(rda != NULL)
		FreeArgs(rda);

	if(NO success)
		cleanup();

	return(success);
}

/****************************************************************************/

void
print_stats(void)
{
	struct RastPort * rp = Window->RPort;
	UBYTE buffer[300];
	ULONG avg_sent;
	ULONG avg_received;
	struct TextExtent te;
	LONG width;
	LONG len;
	struct Sana2ExtDeviceStats s2xds;
	ULONG seconds;

	memset(&s2xds,0,sizeof(s2xds));

	s2xds.s2xds_Length = sizeof(s2xds);

	NetRequest->ios2_Req.io_Command	= S2_GETEXTENDEDGLOBALSTATS;
	NetRequest->ios2_StatData		= &s2xds;

	if(DoIO((struct IORequest *)NetRequest) == OK)
		seconds = s2xds.s2xds_TimeConnected.tv_secs;
	else
		seconds = 0;

	if(TotalCounted > 0)
	{
		avg_sent		= TotalSent / TotalCounted;
		avg_received	= TotalReceived / TotalCounted;
	}
	else
	{
		avg_sent		= 0;
		avg_received	= 0;
	}

	if(seconds > 0)
	{
		local_sprintf(buffer,"connected %lD:%02ld:%02ld max %lD/%lD avg %lD/%lD last %lD/%lD",
			seconds / (60 * 60),(seconds / 60) % 60,(seconds % 60),
			MaxReceived,MaxSent,
			avg_received,avg_sent,
			LastReceived,LastSent);
	}
	else
	{
		local_sprintf(buffer,"max %lD/%lD avg %lD/%lD last %lD/%lD",
			MaxReceived,MaxSent,
			avg_received,avg_sent,
			LastReceived,LastSent);
	}

	width = Window->Width - (Window->BorderLeft + Window->BorderRight);

	len = TextFit(rp,buffer,strlen(buffer),&te,NULL,1,width,32767);

	if(len > 0)
	{
		SetAPen(rp,DrawInfo->dri_Pens[TEXTPEN]);

		Move(rp,Window->BorderLeft,Window->Height - (Window->BorderBottom + Font->tf_YSize - Font->tf_Baseline));
		Text(rp,buffer,len);

		SetAPen(rp,DrawInfo->dri_Pens[BACKGROUNDPEN]);

		RectFill(rp,
			Window->BorderLeft,Window->Height - (Window->BorderBottom + Font->tf_YSize + 1),
			Window->Width - (Window->BorderRight + 1),Window->Height - (Window->BorderBottom + Font->tf_YSize + 1));
	}

	if(te.te_Width < width)
	{
		SetAPen(rp,DrawInfo->dri_Pens[BACKGROUNDPEN]);

		RectFill(rp,
			Window->BorderLeft + te.te_Width,Window->Height - (Window->BorderBottom + Font->tf_YSize),
			Window->Width - (Window->BorderRight + 1),Window->Height - (Window->BorderBottom + 1));
	}
}

/****************************************************************************/

void
clear_display(void)
{
	struct RastPort * rp = Window->RPort;
	LONG height;
	LONG top;

	SetAPen(rp,DrawInfo->dri_Pens[BACKGROUNDPEN]);
	RectFill(rp,
		Window->BorderLeft,
		Window->BorderTop,
		Window->Width - (Window->BorderRight+1),
		Window->Height - (Window->BorderBottom+1));

	height = Window->Height - (Window->BorderTop + Window->BorderBottom + (Font->tf_YSize+1));
	top = height - ((height / 2) + 1);

	SetAPen(rp,DrawInfo->dri_Pens[SHADOWPEN]);
	RectFill(rp,
		Window->BorderLeft,Window->BorderTop + top,
		Window->Width - (Window->BorderRight + 1),Window->BorderTop + top);

	print_stats();
}

/****************************************************************************/

void
redraw_display(void)
{
	struct RastPort * rp = Window->RPort;
	LONG width,height;
	LONG mid;
	LONG top;
	LONG bottom;
	LONG i;

	SetAPen(rp,DrawInfo->dri_Pens[BACKGROUNDPEN]);
	RectFill(rp,
		Window->BorderLeft,
		Window->BorderTop,
		Window->Width - (Window->BorderRight+1),
		Window->Height - (Window->BorderBottom+1));

	height	= Window->Height - (Window->BorderTop + Window->BorderBottom + (Font->tf_YSize+1));
	width	= Window->Width - (Window->BorderLeft + Window->BorderRight);

	mid = height / 2;

	top = height - (mid + 1);
	bottom = height - (top + 1);

	for(i = 0 ; i < width ; i++)
	{
		if(MaxSentReceived > 0)
		{
			ULONG received = (HistoryReceived[HistorySize - width + i] * top) / MaxSentReceived;
			ULONG sent = (HistorySent[HistorySize - width + i] * bottom) / MaxSentReceived;

			if(received > 0)
			{
				SetAPen(rp,DrawInfo->dri_Pens[SHINEPEN]);
				RectFill(rp,
					Window->BorderLeft+i,Window->BorderTop + top - received,
					Window->BorderLeft+i,Window->BorderTop + top - 1);
			}

			if(sent > 0)
			{
				SetAPen(rp,DrawInfo->dri_Pens[FILLPEN]);
				RectFill(rp,
					Window->BorderLeft+i,Window->BorderTop + top + 1,
					Window->BorderLeft+i,Window->BorderTop + top + 1 + sent - 1);
			}
		}
	}

	SetAPen(rp,DrawInfo->dri_Pens[SHADOWPEN]);
	RectFill(rp,
		Window->BorderLeft,Window->BorderTop + top,
		Window->Width - (Window->BorderRight + 1),Window->BorderTop + top);

	print_stats();
}

/****************************************************************************/

#define REDRAW_DISPLAY				TRUE
#define PRETEND_TO_REDRAW_DISPLAY	FALSE

/****************************************************************************/

void
update_display(BOOL can_draw)
{
	STATIC BOOL catch_up;
	struct RastPort * rp = Window->RPort;
	struct Sana2ThroughputStats s2ts;
	LONG height;
	LONG mid;
	LONG top;
	LONG bottom;
	BOOL scrolled;

	Forbid();
	s2ts = NetStat;
	Permit();

	if(NOT can_draw)
		catch_up = TRUE;

	scrolled = FALSE;

	height = Window->Height - (Window->BorderTop + Window->BorderBottom + (Font->tf_YSize+1));

	mid = height / 2;

	top = height - (mid + 1);
	bottom = height - (top + 1);

	if(NetStatLast.s2ts_Actual == 0)
	{
		NetStatLast = s2ts;
	}
	else if (-CmpTime(&s2ts.s2ts_EndTime,&NetStatLast.s2ts_EndTime) > 0)
	{
		struct timeval tv;
		struct Sana2ThroughputStats s2ts_last;
		ULONG sent;
		ULONG received;
		BOOL redraw;

		tv = s2ts.s2ts_EndTime;
		SubTime(&tv,&NetStatLast.s2ts_EndTime);

		s2ts_last = NetStatLast;
		NetStatLast = s2ts;

		sent		= s2ts.s2ts_BytesSent.s2q_Low - s2ts_last.s2ts_BytesSent.s2q_Low;
		received	= s2ts.s2ts_BytesReceived.s2q_Low - s2ts_last.s2ts_BytesReceived.s2q_Low;

		if(tv.tv_secs > 0 || tv.tv_micro > 0)
		{
			double f;

			f = (double)tv.tv_secs + (double)tv.tv_micro / 1000000.0;

			sent		= (ULONG)((double)sent / f);
			received	= (ULONG)((double)received / f);
		}
		else
		{
			sent = received = 0;
		}

		tv = s2ts.s2ts_EndTime;
		SubTime(&tv,&s2ts_last.s2ts_StartTime);
		TotalSeconds = tv.tv_secs;

		if(HistorySize > 0)
		{
			memmove(HistoryReceived,HistoryReceived+1,sizeof(ULONG) * (HistorySize-1));
			memmove(HistorySent,HistorySent+1,sizeof(ULONG) * (HistorySize-1));

			HistoryReceived[HistorySize-1]	= received;
			HistorySent[HistorySize-1]		= sent;
		}

		if(sent != 0)
			LastSent = sent;

		if(received != 0)
			LastReceived = received;

		TotalSent		+= sent;
		TotalReceived	+= received;
		TotalCounted	+= 1;

		redraw = FALSE;

		if(sent > MaxSent)
		{
			MaxSent = sent;
			if(sent > MaxSentReceived)
				MaxSentReceived = sent;

			redraw = TRUE;
		}

		if(received > MaxReceived)
		{
			MaxReceived = received;
			if(received > MaxSentReceived)
				MaxSentReceived = received;

			redraw = TRUE;
		}

		if(catch_up)
		{
			if(NOT redraw)
				clear_display();

			redraw = TRUE;
		}

		scrolled = TRUE;

		if(can_draw)
		{
			if(redraw && HistorySize > 0)
			{
				redraw_display();
			}
			else
			{
				SetBPen(rp,DrawInfo->dri_Pens[BACKGROUNDPEN]);
				ScrollRaster(rp,1,0,
					Window->BorderLeft,
					Window->BorderTop,
					Window->Width - (Window->BorderRight+1),
					Window->Height - (Window->BorderBottom+1+Font->tf_YSize+1));

				SetAPen(rp,DrawInfo->dri_Pens[SHADOWPEN]);
				WritePixel(rp,Window->Width - (Window->BorderRight+1),Window->BorderTop + top);

				if(MaxSentReceived > 0)
				{
					received	= (received * top) / MaxSentReceived;
					sent		= (sent * bottom) / MaxSentReceived;
				}
				else
				{
					received	= 0;
					sent		= 0;
				}

				if(received > 0)
				{
					SetAPen(rp,DrawInfo->dri_Pens[SHINEPEN]);
					RectFill(rp,
						Window->Width - (Window->BorderRight+1),Window->BorderTop + top - received,
						Window->Width - (Window->BorderRight+1),Window->BorderTop + top - 1);
				}

				if(sent > 0)
				{
					SetAPen(rp,DrawInfo->dri_Pens[FILLPEN]);
					RectFill(rp,
						Window->Width - (Window->BorderRight+1),Window->BorderTop + top + 1,
						Window->Width - (Window->BorderRight+1),Window->BorderTop + top + 1 + sent - 1);
				}
			}

			print_stats();

			catch_up = FALSE;
		}
	}

	if(can_draw && NOT scrolled)
	{
		SetBPen(rp,DrawInfo->dri_Pens[BACKGROUNDPEN]);
		ScrollRaster(rp,1,0,
			Window->BorderLeft,
			Window->BorderTop,
			Window->Width - (Window->BorderRight+1),
			Window->Height - (Window->BorderBottom+1+Font->tf_YSize+1));

		SetAPen(rp,DrawInfo->dri_Pens[SHADOWPEN]);
		WritePixel(rp,Window->Width - (Window->BorderRight+1),Window->BorderTop + top);

		print_stats();
	}
}

/****************************************************************************/

int
main(int argc,char **argv)
{
	if(setup())
	{
		ULONG time_mask;
		ULONG window_mask;
		ULONG net_mask;
		ULONG signal_mask;
		ULONG signals;
		BOOL done;

		time_mask = PORT_SIG_MASK(TimePort);
		window_mask = PORT_SIG_MASK(Window->UserPort);
		net_mask = PORT_SIG_MASK(NetPort);

		signal_mask = SIGBREAKF_CTRL_C | time_mask | window_mask | net_mask;
		signals = 0;

		done = FALSE;

		TimeRequest->tr_node.io_Command	= TR_ADDREQUEST;
		TimeRequest->tr_time.tv_secs	= 1;
		TimeRequest->tr_time.tv_micro	= 0;
		SendIO((struct IORequest *)TimeRequest);

		do
		{
			if(signals == 0)
				signals = Wait(signal_mask);
			else
				signals |= SetSignal(0,signal_mask) & signal_mask;

			if(signals & time_mask)
			{
				if(GetMsg(TimePort) != NULL)
				{
					WaitIO((struct IORequest *)TimeRequest);

					if(AttemptLockLayerRom(Window->WLayer))
					{
						update_display(REDRAW_DISPLAY);

						UnlockLayerRom(Window->WLayer);
					}
					else
					{
						update_display(PRETEND_TO_REDRAW_DISPLAY);
					}

					TimeRequest->tr_node.io_Command	= TR_ADDREQUEST;
					TimeRequest->tr_time.tv_secs	= 1;
					TimeRequest->tr_time.tv_micro	= 0;

					SendIO((struct IORequest *)TimeRequest);
				}

				signals &= ~time_mask;
			}

			if(signals & window_mask)
			{
				struct IntuiMessage * imsg;

				imsg = (struct IntuiMessage *)GetMsg(Window->UserPort);
				if(imsg != NULL)
				{
					ULONG class = imsg->Class;
					UWORD code = imsg->Code;

					ReplyMsg((struct Message *)imsg);

					if(class == IDCMP_CLOSEWINDOW)
					{
						done = TRUE;
					}
					else if (class == IDCMP_NEWSIZE)
					{
						LockLayerRom(Window->WLayer);

						if(HistorySize > 0)
						{
							redraw_display();
						}
						else
						{
							clear_display();

							update_display(REDRAW_DISPLAY);
						}

						UnlockLayerRom(Window->WLayer);

						if(CheckIO((struct IORequest *)TimeRequest) == BUSY)
							AbortIO((struct IORequest *)TimeRequest);

						WaitIO((struct IORequest *)TimeRequest);

						TimeRequest->tr_node.io_Command	= TR_ADDREQUEST;
						TimeRequest->tr_time.tv_secs	= 1;
						TimeRequest->tr_time.tv_micro	= 0;

						SendIO((struct IORequest *)TimeRequest);
					}
					else if (class == IDCMP_VANILLAKEY)
					{
						if(code == '\033' || code == '\034' || code == '\003')
							done = TRUE;
					}
				}
				else
				{
					signals &= ~window_mask;
				}
			}

			if(signals & net_mask)
			{
				struct IOSana2Req * ios2;

				ios2 = (struct IOSana2Req *)GetMsg(NetPort);
				if(ios2 != NULL)
				{
					UBYTE io_error_string[100];
					UBYTE wire_error_string[100];

					NetStatPending = FALSE;

					Printf("Data throughput query failed (%s, %s).\n",
						get_io_error_string(NetStatRequest->ios2_Req.io_Error,io_error_string),
						get_wire_error_string(NetStatRequest->ios2_WireError,wire_error_string));

					done = TRUE;
				}

				signals &= ~net_mask;
			}

			if(signals & SIGBREAKF_CTRL_C)
			{
				done = TRUE;

				signals &= ~SIGBREAKF_CTRL_C;
			}
		}
		while(NOT done);

		if(CheckIO((struct IORequest *)TimeRequest) == BUSY)
			AbortIO((struct IORequest *)TimeRequest);

		WaitIO((struct IORequest *)TimeRequest);

		cleanup();
	}

	return(0);
}
