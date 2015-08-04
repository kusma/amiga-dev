/*
 *	$Id: SampleNetSpeed.c,v 1.13 2007-08-26 12:30:26 obarthel Exp $
 *
 *	:ts=4
 *
 *	Copyright © 2001-2007 by Olaf Barthel. All Rights Reserved.
 */

/****************************************************************************/

#include <dos/dosextens.h>
#include <dos/dosasl.h>
#include <dos/dostags.h>
#include <dos/rdargs.h>
#include <dos/stdio.h>

#include <exec/memory.h>
#include <exec/execbase.h>

#include <devices/timer.h>

#include <graphics/gfxbase.h>

#include <intuition/intuition.h>
#include <intuition/intuitionbase.h>

#include <clib/alib_protos.h>

/*****************************************************************************/

#undef __NOGLOBALIFACE__
#define __USE_INLINE__
#define __NOLIBBASE__

/*****************************************************************************/

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/timer.h>
#include <proto/bsdsocket.h>

/*****************************************************************************/

#include <string.h>
#include <stdarg.h>
#include <stddef.h>

/****************************************************************************/

/* A quick workaround for the timeval/timerequest->TimeVal/TimeRequest
   change in the recent OS4 header files. */

#if defined(__NEW_TIMEVAL_DEFINITION_USED__)

#define timeval		TimeVal
#define tv_secs		Seconds
#define tv_micro	Microseconds

#define timerequest	TimeRequest
#define tr_node		Request
#define tr_time		Time

#endif /* __NEW_TIMEVAL_DEFINITION_USED__ */

/****************************************************************************/

#include "SampleNetSpeed_rev.h"
STRPTR Version = VERSTAG;

/****************************************************************************/

#include "macros.h"

/****************************************************************************/

/*#define DEBUG*/
#include "assert.h"

/****************************************************************************/

#define PORT_SIG_MASK(mp) (1UL << (mp)->mp_SigBit)

/****************************************************************************/

extern struct Library *	SysBase;
extern struct Library *	DOSBase;

/****************************************************************************/

struct Library *		IntuitionBase;
struct Library *		GfxBase;
struct Library *		SocketBase;
struct Device *			TimerBase;

/****************************************************************************/

#if defined(__amigaos4__)

struct IntuitionIFace *	IIntuition;
struct GraphicsIFace *	IGraphics;
struct SocketIFace *	ISocket;
struct TimerIFace *		ITimer;

#endif /* __amigaos4__ */

/****************************************************************************/

struct MsgPort *		TimePort;
struct timerequest * 	TimeRequest;

/****************************************************************************/

UBYTE					Title[100];
struct Window *			Window;
struct DrawInfo *		DrawInfo;
struct TextFont *		Font;

/****************************************************************************/

UBYTE					InterfaceName[20];
struct timeval			LastSampleTime;
SBQUAD_T				LastSampleIn;
SBQUAD_T				LastSampleOut;

/****************************************************************************/

ULONG					MaxSent;
ULONG					MaxSentReceived;
ULONG					MaxReceived;
ULONG					LastSent;
ULONG					LastReceived;
ULONG					TotalReceived;
ULONG					TotalSent;
ULONG					TotalCounted;

/****************************************************************************/

ULONG *					HistoryReceived;
ULONG *					HistorySent;
LONG					HistorySize;

/****************************************************************************/

VOID local_sprintf(STRPTR buffer, STRPTR formatString, ...);
VOID cleanup(void);
BOOL setup(void);
void print_stats(void);
void clear_display(void);
void redraw_display(void);
void update_display(BOOL can_draw);

/****************************************************************************/

#if defined(__GNUC__) && !defined(__amigaos4__)

#undef Printf

LONG
Printf(STRPTR format,...)
{
  va_list arg;
  LONG result;

  va_start(arg,format);
  result = VPrintf(format,arg);
  va_end(arg);

  return(result);
}

#endif /* __GNUC__ && !__amigaos4__ */

/****************************************************************************/

VOID VARARGS68K
local_sprintf(STRPTR buffer, STRPTR formatString, ...)
{
	va_list varArgs;

	#if defined(__amigaos4__)
	{
		va_startlinear(varArgs,formatString);
		RawDoFmt(formatString,va_getlinearva(varArgs,APTR),(VOID (*)())NULL,buffer);
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

	#if defined(__amigaos4__)
	{
		DropInterface((struct Interface *)ITimer);
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
		DropInterface((struct Interface *)ISocket);
		DropInterface((struct Interface *)IGraphics);
		DropInterface((struct Interface *)IIntuition);
	}
	#endif /* __amigaos4__ */

	if(SocketBase != NULL)
	{
		CloseLibrary(SocketBase);
		SocketBase = NULL;
	}

	if(GfxBase != NULL)
	{
		CloseLibrary(GfxBase);
		GfxBase = NULL;
	}

	if(IntuitionBase != NULL)
	{
		CloseLibrary(IntuitionBase);
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
		STRPTR	Interface;
		LONG *	Left;
		LONG *	Top;
		LONG *	Width;
		LONG *	Height;
		STRPTR	Screen;
	} args;

	struct RDArgs * rda = NULL;
	BOOL success = FALSE;
	LONG left,top,width,height;

	if(SysBase->lib_Version < 36)
		goto out;

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

	SocketBase = OpenLibrary("bsdsocket.library",4);

	#if defined(__amigaos4__)
	{
		if(SocketBase != NULL)
		{
			ISocket = (struct SocketIFace *)GetInterface(SocketBase, "main", 1, 0);
			if(ISocket == NULL)
			{
				CloseLibrary(SocketBase);
				SocketBase = NULL;
			}
		}
	}
	#endif /* __amigaos4__ */

	if(SocketBase == NULL)
	{
		Printf("Could not open 'bsdsocket.library' V4.\n");
		goto out;
	}

	IntuitionBase = OpenLibrary("intuition.library",37);

	#if defined(__amigaos4__)
	{
		if(IntuitionBase != NULL)
		{
			IIntuition = (struct IntuitionIFace *)GetInterface(IntuitionBase, "main", 1, 0);
			if(IIntuition == NULL)
			{
				CloseLibrary(IntuitionBase);
				IntuitionBase = NULL;
			}
		}
	}
	#endif /* __amigaos4__ */

	if(IntuitionBase == NULL)
	{
		Printf("Could not open 'intuition.library' V37.\n");
		goto out;
	}

	GfxBase = OpenLibrary("graphics.library",37);

	#if defined(__amigaos4__)
	{
		if(GfxBase != NULL)
		{
			IGraphics = (struct GraphicsIFace *)GetInterface(GfxBase, "main", 1, 0);
			if(IGraphics == NULL)
			{
				CloseLibrary(GfxBase);
				GfxBase = NULL;
			}
		}
	}
	#endif /* __amigaos4__ */

	if(GfxBase == NULL)
	{
		Printf("Could not open 'graphics.library' V37.\n");
		goto out;
	}

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
		ITimer = (struct TimerIFace *)GetInterface((struct Library *)TimerBase, "main", 1, 0);
		if(ITimer == NULL)
		{
			Printf("Could not access 'timer.device' interface.\n");
			goto out;
		}
	}
	#endif /* __amigaos4__ */

	memset(&args,0,sizeof(args));

	rda = ReadArgs("INTERFACE,LEFT/N,TOP/N,WIDTH/N,HEIGHT/N,SCREEN/K",(LONG *)&args,NULL);
	if(rda == NULL)
	{
		PrintFault(IoErr(),"Sample");
		goto out;
	}

	if(args.Interface != NULL)
	{
		LONG len;

		len = strlen(args.Interface);
		if(len > sizeof(InterfaceName)-1)
			len = sizeof(InterfaceName)-1;

		memcpy(InterfaceName,args.Interface,len);
		InterfaceName[len] = '\0';
	}

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

	GetSysTime(&LastSampleTime);

	if(InterfaceName[0] != '\0')
	{
		local_sprintf(Title,"%s I/O performance",InterfaceName);

		if(QueryInterfaceTags(InterfaceName,
			IFQ_GetBytesIn,		&LastSampleIn,
			IFQ_GetBytesOut,	&LastSampleOut,
		TAG_END) != 0)
		{
			Printf("Could not query data throughput statistics for interface '%s'.\n",InterfaceName);
			goto out;
		}
	}
	else
	{
		strcpy(Title,"TCP/IP I/O performance");

		if(SocketBaseTags(
			SBTM_GETREF(SBTC_GET_BYTES_RECEIVED),	&LastSampleIn,
			SBTM_GETREF(SBTC_GET_BYTES_SENT),		&LastSampleOut,
		TAG_END) != 0)
		{
			Printf("Could not query data throughput statistics.\n");
			goto out;
		}
	}

	Font = OpenFont(&topaz);
	if(Font == NULL)
	{
		Printf("Could not open default font topaz/8.\n");
		goto out;
	}

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
		(args.Screen != NULL) ? WA_PubScreenName : TAG_IGNORE, args.Screen,
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

	local_sprintf(buffer,"max %lD/%lD avg %lD/%lD last %lD/%lD",
		MaxReceived,MaxSent,
		avg_received,avg_sent,
		LastReceived,LastSent);

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

ULONG
quad_delta(const SBQUAD_T * a,const SBQUAD_T * b)
{
	ULONG result = 0;
	SBQUAD_T delta;

	delta = (*a);

	if(delta.sbq_Low < b->sbq_Low)
	{
		if(delta.sbq_High == 0)
			goto out;

		delta.sbq_High--;

		delta.sbq_Low = (~0UL) - b->sbq_Low + 1 + a->sbq_Low;
	}
	else
	{
		delta.sbq_Low -= b->sbq_Low;
	}

	if(delta.sbq_High < b->sbq_High)
		goto out;

	/*delta.sbq_High -= b->sbq_High;*/

	result = delta.sbq_Low;

 out:

	return(result);
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
	struct timeval now;
	SBQUAD_T sample_in;
	SBQUAD_T sample_out;
	struct timeval tv;
	LONG height;
	LONG mid;
	LONG top;
	LONG bottom;
	ULONG sent;
	ULONG received;
	BOOL redraw;

	GetSysTime(&now);

	if(InterfaceName[0] != '\0')
	{
		if(QueryInterfaceTags(InterfaceName,
			IFQ_GetBytesIn,		&sample_in,
			IFQ_GetBytesOut,	&sample_out,
		TAG_END) != 0)
		{
			memset(&sample_in,0,sizeof(sample_in));
			memset(&sample_out,0,sizeof(sample_out));
		}
	}
	else
	{
		if(SocketBaseTags(
			SBTM_GETREF(SBTC_GET_BYTES_RECEIVED),	&sample_in,
			SBTM_GETREF(SBTC_GET_BYTES_SENT),		&sample_out,
		TAG_END) != 0)
		{
			memset(&sample_in,0,sizeof(sample_in));
			memset(&sample_out,0,sizeof(sample_out));
		}
	}

	if(NOT can_draw)
		catch_up = TRUE;

	height = Window->Height - (Window->BorderTop + Window->BorderBottom + (Font->tf_YSize+1));

	mid = height / 2;

	top = height - (mid + 1);
	bottom = height - (top + 1);

	tv = now;
	SubTime(&tv,&LastSampleTime);

	received	= quad_delta(&sample_in,&LastSampleIn);
	sent		= quad_delta(&sample_out,&LastSampleOut);

	LastSampleTime	= now;
	LastSampleIn	= sample_in;
	LastSampleOut	= sample_out;

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

/****************************************************************************/

/****** ROADSHOW/SAMPLENETSPEED ************************************************
*
*   NAME
*	SampleNetSpeed - Display network I/O performance
*
*   FORMAT
*	SampleNetSpeed [[INTERFACE] <interface name>] [LEFT <window left edge>]
*	               [TOP <window top edge>] [WIDTH <window width>]
*	               [HEIGHT <window height>] [SCREEN <name>]
*
*   TEMPLATE
*	INTERFACE,LEFT/N,TOP/N,WIDTH/N,HEIGHT/N,SCREEN/K
*
*   FUNCTION
*	The command can be used to gather statistics on the performance of
*	a specific network interface or all network interfaces used by the
*	TCP/IP stack. Performance is measured as data throughput when sending
*	or receiving data packets to/from the network.
*
*	The performance data is sampled once every second and visualized in
*	a window which is updated regularly. In the top half of the window
*	the amount of data received is displayed, while the bottom half shows
*	the amount of data sent. The sizes of the bars representing the
*	performance are always using the same scale. At the bottom of the
*	window three figures are printed (from left to right): maximum
*	throughput, averaged throughput, last throughput. For each of these
*	figures the values are displayed in the form "number of bytes
*	received per second / number of bytes sent per second".
*
*   OPTIONS
*	INTERFACE
*	    The name of the interface whose performance should be sampled.
*	    If omitted, information on all networking interfaces will be
*	    gathered.
*
*	LEFT
*	TOP
*	    These parameters are optional; you can specify the position of
*	    the window to open. Default is to open it in the top left corner
*	    of the screen.
*
*	WIDTH
*	HEIGHT
*	    These parameters are optional; you can specify the size of the
*	    window to open. Default is to open a window that can display a
*	    300 by 50 pixel area. Note that width and height actually refer
*	    to the size of the display area and not the total size of the
*	    window.
*
*	SCREEN
*	    You can specify the name of a public screen to open the window
*	    on. If omitted, the window will open on the default public
*	    screen, which is usually the Workbench screen.
*
*   NOTES
*	To stop the command, either click on the window close gadget in the
*	top left corner or send a break signal to it in the shell.
*
******************************************************************************
*/

int
main(int argc,char **argv)
{
	if(setup())
	{
		ULONG time_mask;
		ULONG window_mask;
		ULONG signal_mask;
		ULONG signals;
		BOOL done;

		time_mask = PORT_SIG_MASK(TimePort);
		window_mask = PORT_SIG_MASK(Window->UserPort);

		signal_mask = SIGBREAKF_CTRL_C | time_mask | window_mask;
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
