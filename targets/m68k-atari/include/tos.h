/*
 * todo
 */

#ifndef __TOS_H__
#define __TOS_H__

#ifndef BYTE
#define BYTE char
#endif

#ifndef UBYTE
#define UBYTE unsigned char
#endif

#ifndef WORD
#define WORD short int
#endif

#ifndef UWORD
#define UWORD unsigned short int
#endif

#ifndef LONG
#define LONG long int
#endif

#ifndef ULONG
#define ULONG unsigned long int
#endif

#ifndef VOIDP
#define VOIDP void*
#endif

#ifndef VOID
#define VOID void
#endif

/* BIOS Errors */

#define E_OK			0
#define ERROR			-1
#define EDRVNR			-2
#define EUNCMD			-3
#define E_CRC			-4
#define EBADRQ			-5
#define E_SEEK			-6
#define EMEDIA			-7
#define ESECNF			-8
#define EPAPER			-9
#define EWRITF			-10
#define EREADF			-11
#define EGENRL			-12
#define EWRPRO			-13
#define E_CHNG			-14
#define EUNDEV			-15
#define EBADSF			-16
#define EOTHER			-17
#define EINSERT			-18
#define EDVNRSP			-19

/* Variables used by BIOS */

#define DEV_PRINTER		0
#define DEV_AUX			1
#define DEV_CONSOLE		2
#define DEV_MIDI		3
#define DEV_IKBD		4
#define DEV_RAW			5

#define RW_READ			0
#define RW_WRITE		0
#define RW_NOMEDIACH	1
#define RW_NORETRIES	2
#define RW_NOTRANSLATE	3

#define VEC_BUSERROR	0x02
#define VEC_ADDRESSERROR	0x03
#define VEC_ILLEGALINSTRUCTION	0x04
#define VEC_GEMDOS		0x21
#define VEC_GEM			0x22
#define VEC_BIOS		0x2d
#define VEC_XBIOS		0x2e
#define VEC_TIMER		0x100
#define VEC_CRITICALERROR	0x101
#define VEC_CRITICALERR	VEC_CRITICALERROR
#define VEC_TERMINATE	0x102
#define VEC_PROCTERM	VEC_TERMINATE
#define VEC_INQUIRE		((VOIDP)-1)

#define MED_NOCHANGE	0
#define MED_UNKNOWN		1
#define MED_CHANGED		2

#define K_RSHIFT		0x0001
#define K_LSHIFT		0x0002
#define K_CTRL			0x0004
#define K_ALT			0x0008
#define K_CAPSLOCK		0x0010
#define K_CLRHOME		0x0020
#define K_INSERT		0x0040

typedef struct
{
	BYTE d_reserved[21];
	BYTE d_attrib;
	UWORD d_time;
	UWORD d_date;
	LONG d_length;
	char d_fname[14];
} DTA;

typedef struct __pd
{
	VOID *p_lowtpa;
	VOID *p_hitpa;
	VOID *p_tbase;
	LONG p_tlen;
	VOID *p_dbase;
	LONG p_dlen;
	VOID *p_bbase;
	LONG p_blen;
	DTA *p_dta;
	
	struct __pd *p_parent;

	WORD p_res0;
	WORD p_res1;
	BYTE *p_env; 
	BYTE p_devx[6];
	BYTE p_res2;
	BYTE p_defdrv;
	LONG p_res3[18];
	BYTE p_cmdlin[128];
} PD;

#define BASEPAGE PD

typedef struct __md
{
	struct __md *m_link;
	VOIDP m_start;
	LONG m_length;
	BASEPAGE *m_own;
} MD;

typedef struct
{
	MD *mp_mfl;
	MD *mp_mal;
	MD *mp_rover;
} MPB;

typedef struct
{
	WORD recsiz;
	WORD clsiz;
	WORD clsizb;
	WORD rdlen;
	WORD fsiz;
	WORD fatrec;
	WORD datrec;
	WORD numcl;
	WORD bflags;
} BPB;

/* BIOS Calls */

__regsused("d0/d1/d2/a0/a1/a2") VOID Getmpb(__reg("a0")MPB*) =
  "\tmove.l\ta0,-(sp)\n"
  "\tmove.w\t#0,-(sp)\n"
  "\ttrap\t#13\n"
  "\taddq.l\t#6,sp";

__regsused("d0/d1/d2/a0/a1/a2") LONG Bconstat(__reg("d0")WORD) =
  "\tmove.w\td0,-(sp)\n"
  "\tmove.w\t#1,-(sp)\n"
  "\ttrap\t#13\n"
  "\taddq.l\t#4,sp";

__regsused("d0/d1/d2/a0/a1/a2") LONG Bconin(__reg("d0")WORD) =
  "\tmove.w\td0,-(sp)\n"
  "\tmove.w\t#2,-(sp)\n"
  "\ttrap\t#13\n"
  "\taddq.l\t#4,sp";

__regsused("d0/d1/d2/a0/a1/a2") LONG Bconout(__reg("d0")WORD,__reg("d1")WORD) =
  "\tmove.w\td1,-(sp)\n"
  "\tmove.w\td0,-(sp)\n"
  "\tmove.w\t#3,-(sp)\n"
  "\ttrap\t#13\n"
  "\taddq.l\t#6,sp";

__regsused("d0/d1/d2/a0/a1/a2") LONG Rwabs(__reg("d0")WORD,__reg("a0")VOIDP,__reg("d1")WORD,__reg("d2")WORD,__reg("d3")WORD,__reg("d4")LONG) =
  "\tmove.l\td4,-(sp)\n"
  "\tmove.w\td3,-(sp)\n"
  "\tmove.w\td2,-(sp)\n"
  "\tmove.w\td1,-(sp)\n"
  "\tmove.l\ta0,-(sp)\n"
  "\tmove.w\td0,-(sp)\n"
  "\tmove.w\t#4,-(sp)\n"
  "\ttrap\t#13\n"
  "\tlea\t(18,sp),sp";

__regsused("d0/d1/d2/a0/a1/a2") VOIDP Setexc(__reg("d0")WORD,__reg("a0")VOID(*)()) =
  "\tmove.l\ta0,-(sp)\n"
  "\tmove.w\td0,-(sp)\n"
  "\tmove.w\t#5,-(sp)\n"
  "\ttrap\t#13\n"
  "\taddq.l\t#8,sp";

__regsused("d0/d1/d2/a0/a1/a2") LONG Tickcal(VOID) =
  "\tmove.w\t#6,-(sp)\n"
  "\ttrap\t#13\n"
  "\taddq.l\t#2,sp";

__regsused("d0/d1/d2/a0/a1/a2") BPB* Getbpb(__reg("d0")WORD) =
  "\tmove.w\td0,-(sp)\n"
  "\tmove.w\t#7,-(sp)\n"
  "\ttrap\t#13\n"
  "\taddq.l\t#4,sp";

__regsused("d0/d1/d2/a0/a1/a2") LONG Bcostat(__reg("d0")WORD) =
  "\tmove.w\td0,-(sp)\n"
  "\tmove.w\t#8,-(sp)\n"
  "\ttrap\t#13\n"
  "\taddq.l\t#4,sp";

__regsused("d0/d1/d2/a0/a1/a2") LONG Mediach(__reg("d0")WORD) =
  "\tmove.w\td0,-(sp)\n"
  "\tmove.w\t#9,-(sp)\n"
  "\ttrap\t#13\n"
  "\taddq.l\t#4,sp";

__regsused("d0/d1/d2/a0/a1/a2") ULONG Drvmap(VOID) =
  "\tmove.w\t#10,-(sp)\n"
  "\ttrap\t#13\n"
  "\taddq.l\t#2,sp";

__regsused("d0/d1/d2/a0/a1/a2") LONG Kbshift(__reg("d0")WORD) =
  "\tmove.w\td0,-(sp)\n"
  "\tmove.w\t#11,-(sp)\n"
  "\ttrap\t#13\n"
  "\taddq.l\t#4,sp";

/* Variables used by XBIOS */

#define IM_DISABLE		0
#define IM_RELATIVE		1
#define IM_ABSOLUTE		2
#define IM_KEYCODE		4

#define IM_YBOT			0
#define IM_YTOP			1
#define IM_PACKETS		2
#define IM_KEYS			3

#define SCR_NOCHANGE	-1
#define SCR_MODECODE	3

#define COL_INQUIRE		-1

#define FLOP_DRIVEA		0
#define FLOP_DRIVEB		1

#define FLOP_NOSKEW		 1
#define FLOP_SKEW		-1

#define FLOP_MAGIC		0x8754321
#define FLOP_VIRGIN		0xe5e5

#define FLOPPY_DSDD		0
#define FLOPPY_DSHD		1
#define FLOPPY_DSED		2

#define DB_NULLSTRING	0xf000
#define DB_COMMAND		0xf100

#define MFP_PARALLEL	0
#define	MFP_DCD			1
#define MFP_CTS			2
#define MFP_BITBLT		3
#define MFP_TIMERD		4
#define MFP_BAUDRATE	MFP_TIMERD
#define MFP_200HZ		5
#define MFP_ACIA		6
#define MFP_DISK		7
#define MFP_TIMERB		8
#define MFP_HBLANK		MFP_TIMERB
#define MFP_TERR		9
#define MFP_TBE			10
#define MFP_RERR		11
#define MFP_RBF			12
#define MFP_TIMERA		13
#define MFP_DMASOUND	MFP_TIMERA
#define MFP_RING		14
#define MFP_MONODETECT	15

#define IO_SERIAL		0
#define IO_KEYBOARD		1
#define IO_MIDI			2

#define BAUD_19200		0
#define BAUD_9600		1
#define BAUD_4800		2
#define BAUD_3600		3
#define BAUD_2400		4
#define BAUD_2000		5
#define BAUD_1800		6
#define BAUD_1200		7
#define BAUD_600		8
#define BAUD_300		9
#define BAUD_200		10
#define BAUD_150		11
#define BAUD_134		12
#define BAUD_110		13
#define BAUD_75			14
#define BAUD_50			15
#define BAUD_INQUIRE	-2

#define FLOW_NONE		0
#define FLOW_SOFT		1
#define FLOW_HARD		2
#define FLOW_BOTH		3

#define RS_RECVENABLE	0x01
#define RS_SYNCSTRIP	0x02
#define RS_MATCHBUSY	0x04
#define RS_BRKDETECT	0x08
#define RS_FRAMEERR		0x10
#define RS_PARITYERR	0x20
#define RS_OVERRUNERR	0x40
#define RS_BUFFUL		0x80

#define RS_ODDPARITY	0x02
#define RS_EVENPARITY	0x00
#define RS_PARITYENABLE	0x04

#define RS_NOSTOP		0x00
#define RS_1STOP		0x08
#define RS_15STOP		0x10
#define RS_2STOP		0x18

#define RS_8BITS		0x00
#define RS_7BITS		0x20
#define RS_6BITS		0x40
#define RS_5BITS		0x60

#define RS_CLK16		0x80

#define RS_INQUIRE		-1
#define RS_LASTBAUD		2

#define KT_NOCHANGE		((char*)-1)

#define SERIAL_NOCHANGE	-1
#define SERIAL_RANDOM	0x01000001

#define DISK_NOCHANGE	-1
#define DISK_SSSD		0
#define DISK_DSSD		1
#define DISK_SSDD		2
#define DISK_DSDD		3
#define DISK_DSHD		4
#define DISK_DSED		5

#define EXEC_NOCHANGE	-1
#define EXEC_NO			0
#define EXEC_YES		1

#define CURS_HIDE		0
#define CURS_SHOW		1
#define CURS_BLINK		2
#define CURS_NOBLINK	3
#define CURS_SETRATE	4
#define CURS_GETRATE	5

#define PSG_APITCHLOW	0
#define PSG_APITCHHIGH	1
#define PSG_BPITCHLOW	2
#define PSG_BPTICHHIGH	3
#define PSG_CPITCHLOW	4
#define PSG_CPITCHHIGH	5
#define PSG_NOISEPITCH	6
#define PSG_MODE		7
#define PSG_AVOLUME		8
#define PSG_BVOLUME		9
#define PSG_CVOLUME		10
#define PSG_FREQLOW		11
#define PSG_FREQHIGH	12
#define PSG_ENVELOPE	13
#define PSG_PORTA		14
#define PSG_PORTB		15

#define PSG_ENABLEA		0x01
#define PSG_ENABLEB		0x02
#define PSG_ENABLEC		0x04
#define PSG_NOISEA		0x08
#define PSG_NOISEB		0x10
#define PSG_NOISEC		0x20
#define PSG_PRTAOUT		0x40
#define PSG_PRTBOUT		0x80

#define GI_FLOPPYSIDE	0x01
#define GI_FLOPPYA		0x02
#define GI_FLOPPYB		0x04
#define GI_RTS			0x08
#define GI_DTR			0x10
#define GI_STROBE		0x20
#define GI_GPO			0x40
#define GI_SCCPORT		0x80

#define XB_TIMERA		0
#define XB_TIMERB		1
#define XB_TIMERC		2
#define XB_TIMERD		3

#define DS_INQUIRE		-1

#define PRT_DOTMATRIX	0x01
#define PRT_MONO		0x02
#define PRT_ATARI		0x04
#define PRT_DRAFT		0x08
#define PRT_PARALLEL	0x10
#define PRT_CONTINUOUS	0x20

#define PRT_DAISY		0x01
#define PRT_COLOR		0x02
#define PRT_EPSON		0x04
#define PRT_FINAL		0x08
#define PRT_SERIAL		0x10
#define PRT_SINGLE		0x20

#define PRT_INQUIRE		-1

#define	KB_INQUIRE		-1

#define FRATE_6			0
#define FRATE_12		1
#define FRATE_2			2
#define FRATE_3			3
#define	FRATE_INQUIRE	-1

#define BMAP_CHECK		0
#define BMAP_INQUIRE	-1
#define BMAP_MAPTAB		-2

#define NVM_READ		0
#define MVM_WRITE		1
#define NVM_RESET		2

#define BLIT_SOFT		0
#define BLIT_HARD		1

#define ST_LOW			0x0000
#define ST_MED			0x0100
#define ST_HIGH			0x0200
#define TT_MED			0x0300
#define TT_HIGH			0x0600
#define TT_LOW			0x0700

#define ES_GRAY			12
#define ES_SMEAR		15

#define ESB_INQUIRE		-1
#define EC_INQUIRE		-1

#define ESG_INQUIRE		-1
#define ESG_COLOR		0
#define	ESG_GRAY		1

#define ESM_INQUIRE		-1
#define ESM_NORMAL		0
#define	ESM_SMEAR		1

#define BPS1			0x00
#define BPS2			0x01
#define BPS4			0x02
#define BPS8			0x03
#define BPS16			0x04

#define COL80			0x08
#define COL40			0x00

#define VGA				0x10
#define TV				0x00

#define PAL				0x20
#define NTSC			0x00

#define OVERSCAN		0x40

#define STMODES			0x80

#define VERTFLAG		0x100

#define VM_INQUIRE		-1

#define MON_MONO		0
#define MON_COLOR		1
#define MON_VGA			2
#define MON_TV			3

#define VCLK_EXTERNAL	0
#define VCLK_EXTVSYNC	1
#define VCLK_EXTHSYNC	2

#define OVERLAY_ON		1
#define OVERLAY_OFF		0

#define RTS_OFF			0x01
#define RTR_OFF			0x02

#define HF_CLEAR		0
#define HF_SET			1
#define HF_INQUIRE		-1

#define ICR_RXDF		0
#define ICR_TXDE		1
#define ICR_TRDY		2
#define ICR_HF2			3
#define ICR_HF3			4
#define ICR_DMA			6
#define ICR_HREQ		7

#define DSPSEND_NOTHING	0x00000000
#define DSPSEND_ZERO	0xff000000

#define SNDLOCKED		-129

#define SNDNOTLOCK		-128

#define LTATTEN			0
#define RTATTEN			1
#define LTGAIN			2
#define RTGAIN			3
#define ADDERIN			4
#define ADCINPUT		5
#define SETPRESCALE		6

#define CCLK_6K			0
#define CCLK_12K		1
#define CCLK_25K		2
#define CCLK_50K		3

#define LEFT_MIC		0x00
#define LEFT_PSG		0x02
#define RIGHT_MIC		0x00
#define RIGHT_PSG		0x01

#define SND_INQUIRE		-1

#define PLAY			0
#define RECORD			1

#define MODE_STEREO8	0
#define MODE_STEREO16	1
#define MODE_MONO		2

#define INT_DISABLE		0
#define INT_PLAY		1
#define INT_RECORD		2
#define INT_BOTH		3

#define PLAY_ENABLE		0x01
#define PLAY_REPEAT		0x02
#define RECORD_ENABLE	0x04
#define RECORD_REPEAT	0x08

#define DSP_TRISTATE	0
#define DSP_ENABLE		1

#define GPIO_INQUIRE	0
#define GPIO_READ		1
#define GPIO_WRITE		2

#define DMAPLAY			0
#define DSPXMIT			1
#define EXTINP			2
#define ADC				3

#define DMAREC			0x01
#define DSPRECV			0x02
#define EXTOUT			0x04
#define DAC				0x08

#define CLK_25M			0
#define	CLK_EXT			1
#define CLK_32M			2

#define CLK_COMPAT		0
#define CLK_50K			1
#define CLK_33K			2
#define CLK_25K			3
#define CLK_20K			4
#define CLK_16K			5
#define CLK_12K			7
#define CLK_10K			9
#define CLK_8K			11

#define HANDSHAKE		0
#define NO_SHAKE		1

#define SND_ERROR		0xf
#define SND_OK			0
#define SND_BADCONTROL	1
#define SND_BADSYNC		2
#define SND_BADCLOCK	3
#define SND_LEFTCLIP	0x10
#define SND_RIGHTCLIP	0x20

#define WP_MONO			0x00
#define WP_STEREO		0x01
#define WP_8BIT			0x00
#define WP_16BIT		0x02
#define WP_MACRO		0x100

#define AG_FIND			0
#define AG_REPLACE		1
#define AG_CUT			2
#define AG_COPY			3
#define AG_PASTE		4
#define AG_DELETE		5
#define AG_HELP			6
#define AG_PRINT		7
#define AG_SAVE			8
#define AG_ERROR		9
#define AG_QUIT			10

#define WP_OK			0
#define WP_ERROR		-1
#define WP_NOSOUND		1

typedef struct
{
	char *ibuf;
	WORD ibufsize;
	WORD ibufhd;
	WORD ibuftl;
	WORD ibuflow;
	WORD ibufhi;
} IOREC;

typedef struct
{
	char *unshift;
	char *shift;
	char *caps;
} KEYTAB;

typedef struct
{
	void (*midivec)( UBYTE data );
	void (*vkbderr)( UBYTE data );
	void (*vmiderr)( UBYTE data );
	void (*statvec)( char *packet );
	void (*mousevec)( char *packet );
	void (*clockvec)( char *packet );
	void (*joyvec)( char *packet );
	void (*midisys)( VOID );
	void (*ikbdsys)( VOID );
	char ikbdstate;
} KBDVECS;

typedef struct
{
	VOIDP blkptr;
	UWORD offset;
	UWORD width;
	UWORD height;
	UWORD left;
	UWORD right;
	UWORD srcres;
	UWORD destres;
	UWORD *colpal;
	UWORD type;
	UWORD port;
	char  *masks;
} PRTBLK;

/* Available from MetaDOS version 2.30 */
typedef struct
{
	UWORD mi_version;
	LONG mi_magic;
	const BYTE *mi_log2phys;
} META_INFO_2;

typedef struct
{
	ULONG drivemap;
	BYTE *version;
	LONG reserved[2];

	META_INFO_2 *info;	/* Available from MetaDOS version 2.30 */
} META_INFO_1;

#ifndef METAINFO
#define METAINFO META_INFO_1
#endif

typedef struct
{
	BYTE reserved;
	BYTE red;
	BYTE green;
	BYTE blue;
} RGB;

typedef struct
{
	WORD  blocktype;
	LONG  blocksize;
	VOIDP blockaddr;
} DSPBLOCK;

typedef struct
{
	VOIDP playptr;
	VOIDP recordptr;
	VOIDP reserved1;
	VOIDP reserved2;
} SBUFPTR;

typedef struct
{
	WORD (*Bconstat)();
	LONG (*Bconin)();
	LONG (*Bcostat)();
	VOID (*Bconout)();
	ULONG (*Rsconf)();
	IOREC *iorec;
} MAPTAB;

typedef struct
{
	MAPTAB *maptab;
	WORD maptabsize;
} BCONMAP;

/* XBIOS Calls */

__regsused("d0/d1/d2/a0/a1/a2") VOID Initmous(__reg("d0")WORD,__reg("a0")VOIDP,__reg("a1")VOID(*)()) =
  "\tmove.l\ta1,-(sp)\n"
  "\tmove.l\ta0,-(sp)\n"
  "\tmove.w\td0,-(sp)\n"
  "\tmove.w\t#0,-(sp)\n"
  "\ttrap\t#14\n"
  "\tlea\t(12,sp),sp";

__regsused("d0/d1/d2/a0/a1/a2") VOIDP Ssbrk(__reg("d0")WORD) =
  "\tmove.w\td0,-(sp)\n"
  "\tmove.w\t#1,-(sp)\n"
  "\ttrap\t#14\n"
  "\taddq.l\t#4,sp";

__regsused("d0/d1/d2/a0/a1/a2") VOIDP Physbase(VOID) =
  "\tmove.w\t#2,-(sp)\n"
  "\ttrap\t#14\n"
  "\taddq.l\t#2,sp";

__regsused("d0/d1/d2/a0/a1/a2") VOIDP Logbase(VOID) =
  "\tmove.w\t#3,-(sp)\n"
  "\ttrap\t#14\n"
  "\taddq.l\t#2,sp";

__regsused("d0/d1/d2/a0/a1/a2") WORD Getrez(VOID) =
  "\tmove.w\t#4,-(sp)\n"
  "\ttrap\t#14\n"
  "\taddq.l\t#2,sp";

__regsused("d0/d1/d2/a0/a1/a2") VOID Setscreen(__reg("a0")VOIDP,__reg("a1")VOIDP,__reg("d0")WORD) =
  "\tmove.w\td0,-(sp)\n"
  "\tmove.l\ta1,-(sp)\n"
  "\tmove.l\ta0,-(sp)\n"
  "\tmove.w\t#5,-(sp)\n"
  "\ttrap\t#14\n"
  "\tlea\t(12,sp),sp";

__regsused("d0/d1/d2/a0/a1/a2") VOID Vsetscreen(__reg("a0")VOIDP,__reg("a1")VOIDP,__reg("d0")WORD,__reg("d1")WORD) =
  "\tmove.w\td1,-(sp)\n"
  "\tmove.w\td0,-(sp)\n"
  "\tmove.l\ta1,-(sp)\n"
  "\tmove.l\ta0,-(sp)\n"
  "\tmove.w\t#5,-(sp)\n"
  "\ttrap\t#14\n"
  "\tlea\t(14,sp),sp";

__regsused("d0/d1/d2/a0/a1/a2") VOID Setpalette(__reg("a0")WORD*) =
  "\tmove.l\ta0,-(sp)\n"
  "\tmove.w\t#6,-(sp)\n"
  "\ttrap\t#14\n"
  "\taddq.l\t#6,sp";

__regsused("d0/d1/d2/a0/a1/a2") WORD Setcolor(__reg("d0")WORD,__reg("d1")WORD) =
  "\tmove.w\td1,-(sp)\n"
  "\tmove.w\td0,-(sp)\n"
  "\tmove.w\t#7,-(sp)\n"
  "\ttrap\t#14\n"
  "\taddq.l\t#6,sp";

__regsused("d0/d1/d2/a0/a1/a2") WORD Floprd(__reg("a0")VOIDP,__reg("d0")LONG,__reg("d1")WORD,__reg("d2")WORD,__reg("d3")WORD,__reg("d4")WORD,__reg("d5")WORD) =
  "\tmove.w\td5,-(sp)\n"
  "\tmove.w\td4,-(sp)\n"
  "\tmove.w\td3,-(sp)\n"
  "\tmove.w\td2,-(sp)\n"
  "\tmove.w\td1,-(sp)\n"
  "\tmove.l\td0,-(sp)\n"
  "\tmove.l\ta0,-(sp)\n"
  "\tmove.w\t#8,-(sp)\n"
  "\ttrap\t#14\n"
  "\tlea\t(20,sp),sp";

__regsused("d0/d1/d2/a0/a1/a2") WORD Flopwr(__reg("a0")VOIDP,__reg("d0")LONG,__reg("d1")WORD,__reg("d2")WORD,__reg("d3")WORD,__reg("d4")WORD,__reg("d5")WORD) =
  "\tmove.w\td5,-(sp)\n"
  "\tmove.w\td4,-(sp)\n"
  "\tmove.w\td3,-(sp)\n"
  "\tmove.w\td2,-(sp)\n"
  "\tmove.w\td1,-(sp)\n"
  "\tmove.l\td0,-(sp)\n"
  "\tmove.l\ta0,-(sp)\n"
  "\tmove.w\t#9,-(sp)\n"
  "\ttrap\t#14\n"
  "\tlea\t(20,sp),sp";

__regsused("d0/d1/d2/a0/a1/a2") WORD Flopfmt(__reg("a0")VOIDP,__reg("a1")WORD*,__reg("d0")WORD,__reg("d1")WORD,__reg("d2")WORD,__reg("d3")WORD,__reg("d4")WORD,__reg("d5")LONG,__reg("d6")WORD) =
  "\tmove.w\td6,-(sp)\n"
  "\tmove.l\td5,-(sp)\n"
  "\tmove.w\td4,-(sp)\n"
  "\tmove.w\td3,-(sp)\n"
  "\tmove.w\td2,-(sp)\n"
  "\tmove.w\td1,-(sp)\n"
  "\tmove.w\td0,-(sp)\n"
  "\tmove.l\ta1,-(sp)\n"
  "\tmove.l\ta0,-(sp)\n"
  "\tmove.w\t#10,-(sp)\n"
  "\ttrap\t#14\n"
  "\tlea\t(26,sp),sp";

__regsused("d0/d1/d2/a0/a1/a2") VOID Dbmsg(__reg("d0")WORD,__reg("d1")WORD,__reg("d2")LONG) =
  "\tmove.l\td2,-(sp)\n"
  "\tmove.w\td1,-(sp)\n"
  "\tmove.w\td0,-(sp)\n"
  "\tmove.w\t#11,-(sp)\n"
  "\ttrap\t#14\n"
  "\tlea\t(10,sp),sp";

__regsused("d0/d1/d2/a0/a1/a2") VOID Midiws(__reg("d0")WORD,__reg("a0")char*) =
  "\tmove.l\ta0,-(sp)\n"
  "\tmove.w\td0,-(sp)\n"
  "\tmove.w\t#12,-(sp)\n"
  "\ttrap\t#14\n"
  "\taddq.l\t#8,sp";

__regsused("d0/d1/d2/a0/a1/a2") VOID Mfpint(__reg("d0")WORD,__reg("a0")VOID(*)()) =
  "\tmove.l\ta0,-(sp)\n"
  "\tmove.w\td0,-(sp)\n"
  "\tmove.w\t#13,-(sp)\n"
  "\ttrap\t#14\n"
  "\taddq.l\t#8,sp";

__regsused("d0/d1/d2/a0/a1/a2") IOREC* Iorec(__reg("d0")WORD) =
  "\tmove.w\td0,-(sp)\n"
  "\tmove.w\t#14,-(sp)\n"
  "\ttrap\t#14\n"
  "\taddq.l\t#4,sp";

__regsused("d0/d1/d2/a0/a1/a2") ULONG Rsconf(__reg("d0")WORD,__reg("d1")WORD,__reg("d2")WORD,__reg("d3")WORD,__reg("d4")WORD,__reg("d5")WORD) =
  "\tmove.w\td5,-(sp)\n"
  "\tmove.w\td4,-(sp)\n"
  "\tmove.w\td3,-(sp)\n"
  "\tmove.w\td2,-(sp)\n"
  "\tmove.w\td1,-(sp)\n"
  "\tmove.w\td0,-(sp)\n"
  "\tmove.w\t#15,-(sp)\n"
  "\ttrap\t#14\n"
  "\tlea\t(14,sp),sp";

__regsused("d0/d1/d2/a0/a1/a2") KEYTAB* Keytbl(__reg("a0")char*,__reg("a1")char*,__reg("a2")char*) =
  "\tmove.l\ta2,-(sp)\n"
  "\tmove.l\ta1,-(sp)\n"
  "\tmove.l\ta0,-(sp)\n"
  "\tmove.w\t#16,-(sp)\n"
  "\ttrap\t#14\n"
  "\tlea\t(14,sp),sp";

__regsused("d0/d1/d2/a0/a1/a2") LONG Random(VOID) =
  "\tmove.w\t#17,-(sp)\n"
  "\ttrap\t#14\n"
  "\taddq.l\t#2,sp";

__regsused("d0/d1/d2/a0/a1/a2") VOID Protobt(__reg("a0")VOIDP,__reg("d0")LONG,__reg("d1")WORD,__reg("d2")WORD) =
  "\tmove.w\td2,-(sp)\n"
  "\tmove.w\td1,-(sp)\n"
  "\tmove.l\td0,-(sp)\n"
  "\tmove.l\ta0,-(sp)\n"
  "\tmove.w\t#18,-(sp)\n"
  "\ttrap\t#14\n"
  "\tlea\t(14,sp),sp";

__regsused("d0/d1/d2/a0/a1/a2") WORD Flopver(__reg("a0")VOIDP,__reg("d0")LONG,__reg("d1")WORD,__reg("d2")WORD,__reg("d3")WORD,__reg("d4")WORD,__reg("d5")WORD) =
  "\tmove.w\td5,-(sp)\n"
  "\tmove.w\td4,-(sp)\n"
  "\tmove.w\td3,-(sp)\n"
  "\tmove.w\td2,-(sp)\n"
  "\tmove.w\td1,-(sp)\n"
  "\tmove.l\td0,-(sp)\n"
  "\tmove.l\ta0,-(sp)\n"
  "\tmove.w\t#19,-(sp)\n"
  "\ttrap\t#14\n"
  "\tlea\t(20,sp),sp";

__regsused("d0/d1/d2/a0/a1/a2") VOID Scrdmp(VOID) =
  "\tmove.w\t#20,-(sp)\n"
  "\ttrap\t#14\n"
  "\taddq.l\t#2,sp";

__regsused("d0/d1/d2/a0/a1/a2") WORD Cursconf(__reg("d0")WORD,__reg("d1")WORD) =
  "\tmove.w\td1,-(sp)\n"
  "\tmove.w\td0,-(sp)\n"
  "\tmove.w\t#21,-(sp)\n"
  "\ttrap\t#14\n"
  "\taddq.l\t#6,sp";

__regsused("d0/d1/d2/a0/a1/a2") VOID Settime(__reg("d0")LONG) =
  "\tmove.l\td0,-(sp)\n"
  "\tmove.w\t#22,-(sp)\n"
  "\ttrap\t#14\n"
  "\taddq.l\t#6,sp";

__regsused("d0/d1/d2/a0/a1/a2") LONG Gettime(VOID) =
  "\tmove.w\t#23,-(sp)\n"
  "\ttrap\t#14\n"
  "\taddq.l\t#2,sp";

__regsused("d0/d1/d2/a0/a1/a2") VOID Bioskeys(VOID) =
  "\tmove.w\t#24,-(sp)\n"
  "\ttrap\t#14\n"
  "\taddq.l\t#2,sp";

__regsused("d0/d1/d2/a0/a1/a2") VOID Ikbdws(__reg("d0")WORD,__reg("a0")char*) =
  "\tmove.l\ta0,-(sp)\n"
  "\tmove.w\td0,-(sp)\n"
  "\tmove.w\t#25,-(sp)\n"
  "\ttrap\t#14\n"
  "\taddq.l\t#8,sp";

__regsused("d0/d1/d2/a0/a1/a2") VOID Jdisint(__reg("d0")WORD) =
  "\tmove.w\td0,-(sp)\n"
  "\tmove.w\t#26,-(sp)\n"
  "\ttrap\t#14\n"
  "\taddq.l\t#4,sp";

__regsused("d0/d1/d2/a0/a1/a2") VOID Jenabint(__reg("d0")WORD) =
  "\tmove.w\td0,-(sp)\n"
  "\tmove.w\t#27,-(sp)\n"
  "\ttrap\t#14\n"
  "\taddq.l\t#4,sp";

__regsused("d0/d1/d2/a0/a1/a2") WORD Giaccess(__reg("d0")WORD,__reg("d1")WORD) =
  "\tmove.w\td1,-(sp)\n"
  "\tmove.w\td0,-(sp)\n"
  "\tmove.w\t#28,-(sp)\n"
  "\ttrap\t#14\n"
  "\taddq.l\t#6,sp";

__regsused("d0/d1/d2/a0/a1/a2") VOID Offgibit(__reg("d0")WORD) =
  "\tmove.w\td0,-(sp)\n"
  "\tmove.w\t#29,-(sp)\n"
  "\ttrap\t#14\n"
  "\taddq.l\t#4,sp";

__regsused("d0/d1/d2/a0/a1/a2") VOID Ongibit(__reg("d0")WORD) =
  "\tmove.w\td0,-(sp)\n"
  "\tmove.w\t#30,-(sp)\n"
  "\ttrap\t#14\n"
  "\taddq.l\t#4,sp";

__regsused("d0/d1/d2/a0/a1/a2") VOID Xbtimer(__reg("d0")WORD,__reg("d1")WORD,__reg("d2")WORD,__reg("a0")VOID(*)()) =
  "\tmove.l\ta0,-(sp)\n"
  "\tmove.w\td2,-(sp)\n"
  "\tmove.w\td1,-(sp)\n"
  "\tmove.w\td0,-(sp)\n"
  "\tmove.w\t#31,-(sp)\n"
  "\ttrap\t#14\n"
  "\tlea\t(12,sp),sp";

__regsused("d0/d1/d2/a0/a1/a2") VOID Dosound(__reg("a0")char*) =
  "\tmove.l\ta0,-(sp)\n"
  "\tmove.w\t#32,-(sp)\n"
  "\ttrap\t#14\n"
  "\taddq.l\t#6,sp";

__regsused("d0/d1/d2/a0/a1/a2") WORD Setprt(__reg("d0")WORD) =
  "\tmove.w\td0,-(sp)\n"
  "\tmove.w\t#33,-(sp)\n"
  "\ttrap\t#14\n"
  "\taddq.l\t#4,sp";

__regsused("d0/d1/d2/a0/a1/a2") KBDVECS* Kbdvbase(VOID) =
  "\tmove.w\t#34,-(sp)\n"
  "\ttrap\t#14\n"
  "\taddq.l\t#2,sp";

__regsused("d0/d1/d2/a0/a1/a2") WORD Kbrate(__reg("d0")WORD,__reg("d1")WORD) =
  "\tmove.w\td1,-(sp)\n"
  "\tmove.w\td0,-(sp)\n"
  "\tmove.w\t#35,-(sp)\n"
  "\ttrap\t#14\n"
  "\taddq.l\t#6,sp";

__regsused("d0/d1/d2/a0/a1/a2") WORD Prtblk(__reg("a0")PRTBLK*) =
  "\tmove.l\ta0,-(sp)\n"
  "\tmove.w\t#36,-(sp)\n"
  "\ttrap\t#14\n"
  "\taddq.l\t#6,sp";

__regsused("d0/d1/d2/a0/a1/a2") VOID Vsync(VOID) =
  "\tmove.w\t#37,-(sp)\n"
  "\ttrap\t#14\n"
  "\taddq.l\t#2,sp";

__regsused("d0/d1/d2/a0/a1/a2") LONG Supexec(__reg("a0")LONG(*)()) =
  "\tmove.l\ta0,-(sp)\n"
  "\tmove.w\t#38,-(sp)\n"
  "\ttrap\t#14\n"
  "\taddq.l\t#6,sp";

__regsused("d0/d1/d2/a0/a1/a2") VOID Puntaes(VOID) =
  "\tmove.w\t#39,-(sp)\n"
  "\ttrap\t#14\n"
  "\taddq.l\t#2,sp";

__regsused("d0/d1/d2/a0/a1/a2") WORD Floprate(__reg("d0")WORD,__reg("d1")WORD) =
  "\tmove.w\td1,-(sp)\n"
  "\tmove.w\td0,-(sp)\n"
  "\tmove.w\t#41,-(sp)\n"
  "\ttrap\t#14\n"
  "\taddq.l\t#6,sp";

__regsused("d0/d1/d2/a0/a1/a2") LONG DMAread(__reg("d0")LONG,__reg("d1")WORD,__reg("a0")VOIDP,__reg("d2")WORD) =
  "\tmove.w\td2,-(sp)\n"
  "\tmove.l\ta0,-(sp)\n"
  "\tmove.w\td1,-(sp)\n"
  "\tmove.l\td0,-(sp)\n"
  "\tmove.w\t#42,-(sp)\n"
  "\ttrap\t#14\n"
  "\tlea\t(14,sp),sp";

__regsused("d0/d1/d2/a0/a1/a2") LONG DMAwrite(__reg("d0")LONG,__reg("d1")WORD,__reg("a0")VOIDP,__reg("d2")WORD) =
  "\tmove.w\td2,-(sp)\n"
  "\tmove.l\ta0,-(sp)\n"
  "\tmove.w\td1,-(sp)\n"
  "\tmove.l\td0,-(sp)\n"
  "\tmove.w\t#43,-(sp)\n"
  "\ttrap\t#14\n"
  "\tlea\t(14,sp),sp";

__regsused("d0/d1/d2/a0/a1/a2") LONG Bconmap(__reg("d0")WORD) =
  "\tmove.w\td0,-(sp)\n"
  "\tmove.w\t#44,-(sp)\n"
  "\ttrap\t#14\n"
  "\taddq.l\t#4,sp";

__regsused("d0/d1/d2/a0/a1/a2") WORD NVMaccess(__reg("d0")WORD,__reg("d1")WORD,__reg("d2")WORD,__reg("a0")char*) =
  "\tmove.l\ta0,-(sp)\n"
  "\tmove.w\td2,-(sp)\n"
  "\tmove.w\td1,-(sp)\n"
  "\tmove.w\td0,-(sp)\n"
  "\tmove.w\t#46,-(sp)\n"
  "\ttrap\t#14\n"
  "\tlea\t(12,sp),sp";

__regsused("d0/d1/d2/a0/a1/a2") VOID WakeTime(__reg("d0")WORD,__reg("d1")WORD) =
  "\tmove.w\td1,-(sp)\n"
  "\tmove.w\td0,-(sp)\n"
  "\tmove.w\t#47,-(sp)\n"
  "\ttrap\t#14\n"
  "\taddq.l\t#6,sp";

__regsused("d0/d1/d2/a0/a1/a2") VOID Metainit(__reg("a0")METAINFO*) =
  "\tmove.l\ta0,-(sp)\n"
  "\tmove.w\t#48,-(sp)\n"
  "\ttrap\t#14\n"
  "\taddq.l\t#6,sp";

__regsused("d0/d1/d2/a0/a1/a2") WORD Blitmode(__reg("d0")WORD) =
  "\tmove.w\td0,-(sp)\n"
  "\tmove.w\t#64,-(sp)\n"
  "\ttrap\t#14\n"
  "\taddq.l\t#4,sp";

__regsused("d0/d1/d2/a0/a1/a2") WORD EsetShift(__reg("d0")WORD) =
  "\tmove.w\td0,-(sp)\n"
  "\tmove.w\t#80,-(sp)\n"
  "\ttrap\t#14\n"
  "\taddq.l\t#4,sp";

__regsused("d0/d1/d2/a0/a1/a2") WORD EgetShift(VOID) =
  "\tmove.w\t#81,-(sp)\n"
  "\ttrap\t#14\n"
  "\taddq.l\t#2,sp";

__regsused("d0/d1/d2/a0/a1/a2") WORD EsetBank(__reg("d0")WORD) =
  "\tmove.w\td0,-(sp)\n"
  "\tmove.w\t#82,-(sp)\n"
  "\ttrap\t#14\n"
  "\taddq.l\t#4,sp";

__regsused("d0/d1/d2/a0/a1/a2") WORD EsetColor(__reg("d0")WORD,__reg("d1")WORD) =
  "\tmove.w\td1,-(sp)\n"
  "\tmove.w\td0,-(sp)\n"
  "\tmove.w\t#83,-(sp)\n"
  "\ttrap\t#14\n"
  "\taddq.l\t#6,sp";

__regsused("d0/d1/d2/a0/a1/a2") VOID EsetPalette(__reg("d0")WORD,__reg("d1")WORD,__reg("a0")WORD*) =
  "\tmove.l\ta0,-(sp)\n"
  "\tmove.w\td1,-(sp)\n"
  "\tmove.w\td0,-(sp)\n"
  "\tmove.w\t#84,-(sp)\n"
  "\ttrap\t#14\n"
  "\tlea\t(10,sp),sp";

__regsused("d0/d1/d2/a0/a1/a2") VOID EgetPalette(__reg("d0")WORD,__reg("d1")WORD,__reg("a0")WORD*) =
  "\tmove.l\ta0,-(sp)\n"
  "\tmove.w\td1,-(sp)\n"
  "\tmove.w\td0,-(sp)\n"
  "\tmove.w\t#85,-(sp)\n"
  "\ttrap\t#14\n"
  "\tlea\t(10,sp),sp";

__regsused("d0/d1/d2/a0/a1/a2") WORD EsetGray(__reg("d0")WORD) =
  "\tmove.w\td0,-(sp)\n"
  "\tmove.w\t#86,-(sp)\n"
  "\ttrap\t#14\n"
  "\taddq.l\t#4,sp";

__regsused("d0/d1/d2/a0/a1/a2") WORD EsetSmear(__reg("d0")WORD) =
  "\tmove.w\td0,-(sp)\n"
  "\tmove.w\t#87,-(sp)\n"
  "\ttrap\t#14\n"
  "\taddq.l\t#4,sp";

__regsused("d0/d1/d2/a0/a1/a2") WORD VsetMode(__reg("d0")WORD) =
  "\tmove.w\td0,-(sp)\n"
  "\tmove.w\t#88,-(sp)\n"
  "\ttrap\t#14\n"
  "\taddq.l\t#4,sp";

__regsused("d0/d1/d2/a0/a1/a2") WORD VgetMonitor(VOID) =
  "\tmove.w\t#89,-(sp)\n"
  "\ttrap\t#14\n"
  "\taddq.l\t#2,sp";
#define mon_type		VgetMonitor

__regsused("d0/d1/d2/a0/a1/a2") VOID VsetSync(__reg("d0")WORD) =
  "\tmove.w\td0,-(sp)\n"
  "\tmove.w\t#90,-(sp)\n"
  "\ttrap\t#14\n"
  "\taddq.l\t#4,sp";

__regsused("d0/d1/d2/a0/a1/a2") LONG VgetSize(__reg("d0")WORD) =
  "\tmove.w\td0,-(sp)\n"
  "\tmove.w\t#91,-(sp)\n"
  "\ttrap\t#14\n"
  "\taddq.l\t#4,sp";

__regsused("d0/d1/d2/a0/a1/a2") VOID VsetMask(__reg("d0")LONG,__reg("d1")LONG,__reg("d2")WORD) =
  "\tmove.w\td2,-(sp)\n"
  "\tmove.l\td1,-(sp)\n"
  "\tmove.l\td0,-(sp)\n"
  "\tmove.w\t#92,-(sp)\n"
  "\ttrap\t#14\n"
  "\tlea\t(12,sp),sp";

__regsused("d0/d1/d2/a0/a1/a2") VOID VsetRGB(__reg("d0")WORD,__reg("d1")WORD,__reg("a0")RGB*) =
  "\tmove.l\ta0,-(sp)\n"
  "\tmove.w\td1,-(sp)\n"
  "\tmove.w\td0,-(sp)\n"
  "\tmove.w\t#93,-(sp)\n"
  "\ttrap\t#14\n"
  "\tlea\t(10,sp),sp";

__regsused("d0/d1/d2/a0/a1/a2") VOID VgetRGB(__reg("d0")WORD,__reg("d1")WORD,__reg("a0")RGB*) =
  "\tmove.l\ta0,-(sp)\n"
  "\tmove.w\td1,-(sp)\n"
  "\tmove.w\td0,-(sp)\n"
  "\tmove.w\t#94,-(sp)\n"
  "\ttrap\t#14\n"
  "\tlea\t(10,sp),sp";

__regsused("d0/d1/d2/a0/a1/a2") VOID Dsp_DoBlock(__reg("a0")char*,__reg("d0")LONG,__reg("a1")char*,__reg("d1")LONG) =
  "\tmove.l\td1,-(sp)\n"
  "\tmove.l\ta1,-(sp)\n"
  "\tmove.l\td0,-(sp)\n"
  "\tmove.l\ta0,-(sp)\n"
  "\tmove.w\t#96,-(sp)\n"
  "\ttrap\t#14\n"
  "\tlea\t(18,sp),sp";

__regsused("d0/d1/d2/a0/a1/a2") VOID Dsp_BlkHandShake(__reg("a0")char*,__reg("d0")LONG,__reg("a1")char*,__reg("d1")LONG) =
  "\tmove.l\td1,-(sp)\n"
  "\tmove.l\ta1,-(sp)\n"
  "\tmove.l\td0,-(sp)\n"
  "\tmove.l\ta0,-(sp)\n"
  "\tmove.w\t#97,-(sp)\n"
  "\ttrap\t#14\n"
  "\tlea\t(18,sp),sp";

__regsused("d0/d1/d2/a0/a1/a2") VOID Dsp_BlkUnpacked(__reg("a0")LONG*,__reg("d0")LONG,__reg("a1")LONG*,__reg("d1")LONG) =
  "\tmove.l\td1,-(sp)\n"
  "\tmove.l\ta1,-(sp)\n"
  "\tmove.l\td0,-(sp)\n"
  "\tmove.l\ta0,-(sp)\n"
  "\tmove.w\t#98,-(sp)\n"
  "\ttrap\t#14\n"
  "\tlea\t(18,sp),sp";

__regsused("d0/d1/d2/a0/a1/a2") VOID Dsp_InStream(__reg("a0")char*,__reg("d0")LONG,__reg("d1")LONG,__reg("a1")LONG*) =
  "\tmove.l\ta1,-(sp)\n"
  "\tmove.l\td1,-(sp)\n"
  "\tmove.l\td0,-(sp)\n"
  "\tmove.l\ta0,-(sp)\n"
  "\tmove.w\t#99,-(sp)\n"
  "\ttrap\t#14\n"
  "\tlea\t(18,sp),sp";

__regsused("d0/d1/d2/a0/a1/a2") VOID Dsp_OutStream(__reg("a0")char*,__reg("d0")LONG,__reg("d1")LONG,__reg("a1")LONG*) =
  "\tmove.l\ta1,-(sp)\n"
  "\tmove.l\td1,-(sp)\n"
  "\tmove.l\td0,-(sp)\n"
  "\tmove.l\ta0,-(sp)\n"
  "\tmove.w\t#100,-(sp)\n"
  "\ttrap\t#14\n"
  "\tlea\t(18,sp),sp";

__regsused("d0/d1/d2/a0/a1/a2") VOID Dsp_IOStream(__reg("a0")char*,__reg("a1")char*,__reg("d0")LONG,__reg("d1")LONG,__reg("d2")LONG,__reg("a2")LONG*) =
  "\tmove.l\ta2,-(sp)\n"
  "\tmove.l\td2,-(sp)\n"
  "\tmove.l\td1,-(sp)\n"
  "\tmove.l\td0,-(sp)\n"
  "\tmove.l\ta1,-(sp)\n"
  "\tmove.l\ta0,-(sp)\n"
  "\tmove.w\t#101,-(sp)\n"
  "\ttrap\t#14\n"
  "\tlea\t(26,sp),sp";

__regsused("d0/d1/d2/a0/a1/a2") VOID Dsp_RemoveInterrupts(__reg("d0")WORD) =
  "\tmove.w\td0,-(sp)\n"
  "\tmove.w\t#102,-(sp)\n"
  "\ttrap\t#14\n"
  "\taddq.l\t#4,sp";

__regsused("d0/d1/d2/a0/a1/a2") WORD Dsp_GetWordSize(VOID) =
  "\tmove.w\t#103,-(sp)\n"
  "\ttrap\t#14\n"
  "\taddq.l\t#2,sp";

__regsused("d0/d1/d2/a0/a1/a2") WORD Dsp_Lock(VOID) =
  "\tmove.w\t#104,-(sp)\n"
  "\ttrap\t#14\n"
  "\taddq.l\t#2,sp";

__regsused("d0/d1/d2/a0/a1/a2") VOID Dsp_Unlock(VOID) =
  "\tmove.w\t#105,-(sp)\n"
  "\ttrap\t#14\n"
  "\taddq.l\t#2,sp";

__regsused("d0/d1/d2/a0/a1/a2") VOID Dsp_Available(__reg("a0")LONG*,__reg("a1")LONG*) =
  "\tmove.l\ta1,-(sp)\n"
  "\tmove.l\ta0,-(sp)\n"
  "\tmove.w\t#106,-(sp)\n"
  "\ttrap\t#14\n"
  "\tlea\t(10,sp),sp";

__regsused("d0/d1/d2/a0/a1/a2") WORD Dsp_Reserve(__reg("d0")LONG,__reg("d1")LONG) =
  "\tmove.l\td1,-(sp)\n"
  "\tmove.l\td0,-(sp)\n"
  "\tmove.w\t#107,-(sp)\n"
  "\ttrap\t#14\n"
  "\tlea\t(10,sp),sp";

__regsused("d0/d1/d2/a0/a1/a2") WORD Dsp_LoadProg(__reg("a0")char*,__reg("d0")WORD,__reg("a1")char*) =
  "\tmove.l\ta1,-(sp)\n"
  "\tmove.w\td0,-(sp)\n"
  "\tmove.l\ta0,-(sp)\n"
  "\tmove.w\t#108,-(sp)\n"
  "\ttrap\t#14\n"
  "\tlea\t(12,sp),sp";

__regsused("d0/d1/d2/a0/a1/a2") VOID Dsp_ExecProg(__reg("a0")char*,__reg("d0")LONG,__reg("d1")WORD) =
  "\tmove.w\td1,-(sp)\n"
  "\tmove.l\td0,-(sp)\n"
  "\tmove.l\ta0,-(sp)\n"
  "\tmove.w\t#109,-(sp)\n"
  "\ttrap\t#14\n"
  "\tlea\t(12,sp),sp";

__regsused("d0/d1/d2/a0/a1/a2") VOID Dsp_ExecBoot(__reg("a0")char*,__reg("d0")LONG,__reg("d1")WORD) =
  "\tmove.w\td1,-(sp)\n"
  "\tmove.l\td0,-(sp)\n"
  "\tmove.l\ta0,-(sp)\n"
  "\tmove.w\t#110,-(sp)\n"
  "\ttrap\t#14\n"
  "\tlea\t(12,sp),sp";

__regsused("d0/d1/d2/a0/a1/a2") LONG Dsp_LodToBinary(__reg("a0")char*,__reg("a1")char*) =
  "\tmove.l\ta1,-(sp)\n"
  "\tmove.l\ta0,-(sp)\n"
  "\tmove.w\t#111,-(sp)\n"
  "\ttrap\t#14\n"
  "\tlea\t(10,sp),sp";

__regsused("d0/d1/d2/a0/a1/a2") VOID Dsp_TriggerHC(__reg("d0")WORD) =
  "\tmove.w\td0,-(sp)\n"
  "\tmove.w\t#112,-(sp)\n"
  "\ttrap\t#14\n"
  "\taddq.l\t#4,sp";

__regsused("d0/d1/d2/a0/a1/a2") WORD Dsp_RequestUniqueAbility(VOID) =
  "\tmove.w\t#113,-(sp)\n"
  "\ttrap\t#14\n"
  "\taddq.l\t#2,sp";

__regsused("d0/d1/d2/a0/a1/a2") WORD Dsp_GetProgAbility(VOID) =
  "\tmove.w\t#114,-(sp)\n"
  "\ttrap\t#14\n"
  "\taddq.l\t#2,sp";

__regsused("d0/d1/d2/a0/a1/a2") VOID Dsp_FlushSubroutines(VOID) =
  "\tmove.w\t#115,-(sp)\n"
  "\ttrap\t#14\n"
  "\taddq.l\t#2,sp";

__regsused("d0/d1/d2/a0/a1/a2") WORD Dsp_LoadSubroutine(__reg("a0")char*,__reg("d0")LONG,__reg("d1")WORD) =
  "\tmove.w\td1,-(sp)\n"
  "\tmove.l\td0,-(sp)\n"
  "\tmove.l\ta0,-(sp)\n"
  "\tmove.w\t#116,-(sp)\n"
  "\ttrap\t#14\n"
  "\tlea\t(12,sp),sp";

__regsused("d0/d1/d2/a0/a1/a2") WORD Dsp_InqSubrAbility(__reg("d0")WORD) =
  "\tmove.w\td0,-(sp)\n"
  "\tmove.w\t#117,-(sp)\n"
  "\ttrap\t#14\n"
  "\taddq.l\t#4,sp";

__regsused("d0/d1/d2/a0/a1/a2") WORD Dsp_RunSubroutine(__reg("d0")WORD) =
  "\tmove.w\td0,-(sp)\n"
  "\tmove.w\t#118,-(sp)\n"
  "\ttrap\t#14\n"
  "\taddq.l\t#4,sp";

__regsused("d0/d1/d2/a0/a1/a2") WORD Dsp_Hf0(__reg("d0")WORD) =
  "\tmove.w\td0,-(sp)\n"
  "\tmove.w\t#119,-(sp)\n"
  "\ttrap\t#14\n"
  "\taddq.l\t#4,sp";

__regsused("d0/d1/d2/a0/a1/a2") WORD Dsp_Hf1(__reg("d0")WORD) =
  "\tmove.w\td0,-(sp)\n"
  "\tmove.w\t#120,-(sp)\n"
  "\ttrap\t#14\n"
  "\taddq.l\t#4,sp";

__regsused("d0/d1/d2/a0/a1/a2") WORD Dsp_Hf2(VOID) =
  "\tmove.w\t#121,-(sp)\n"
  "\ttrap\t#14\n"
  "\taddq.l\t#2,sp";

__regsused("d0/d1/d2/a0/a1/a2") WORD Dsp_Hf3(VOID) =
  "\tmove.w\t#122,-(sp)\n"
  "\ttrap\t#14\n"
  "\taddq.l\t#2,sp";

__regsused("d0/d1/d2/a0/a1/a2") VOID Dsp_BlkWords(__reg("a0")WORD*,__reg("d0")LONG,__reg("a1")WORD*,__reg("d1")LONG) =
  "\tmove.l\td1,-(sp)\n"
  "\tmove.l\ta1,-(sp)\n"
  "\tmove.l\td0,-(sp)\n"
  "\tmove.l\ta0,-(sp)\n"
  "\tmove.w\t#123,-(sp)\n"
  "\ttrap\t#14\n"
  "\tlea\t(18,sp),sp";

__regsused("d0/d1/d2/a0/a1/a2") VOID Dsp_BlkBytes(__reg("a0")UBYTE*,__reg("d0")LONG,__reg("a1")UBYTE*,__reg("d1")LONG) =
  "\tmove.l\td1,-(sp)\n"
  "\tmove.l\ta1,-(sp)\n"
  "\tmove.l\td0,-(sp)\n"
  "\tmove.l\ta0,-(sp)\n"
  "\tmove.w\t#124,-(sp)\n"
  "\ttrap\t#14\n"
  "\tlea\t(18,sp),sp";

__regsused("d0/d1/d2/a0/a1/a2") BYTE Dsp_Hstat(VOID) =
  "\tmove.w\t#125,-(sp)\n"
  "\ttrap\t#14\n"
  "\taddq.l\t#2,sp";

__regsused("d0/d1/d2/a0/a1/a2") VOID Dsp_SetVectors(__reg("a0")VOID(*)(),__reg("a1")LONG(*)()) =
  "\tmove.l\ta1,-(sp)\n"
  "\tmove.l\ta0,-(sp)\n"
  "\tmove.w\t#126,-(sp)\n"
  "\ttrap\t#14\n"
  "\tlea\t(10,sp),sp";

__regsused("d0/d1/d2/a0/a1/a2") VOID Dsp_MultBlocks(__reg("d0")LONG,__reg("d1")LONG,__reg("a0")DSPBLOCK*,__reg("a1")DSPBLOCK*) =
  "\tmove.l\ta1,-(sp)\n"
  "\tmove.l\ta0,-(sp)\n"
  "\tmove.l\td1,-(sp)\n"
  "\tmove.l\td0,-(sp)\n"
  "\tmove.w\t#127,-(sp)\n"
  "\ttrap\t#14\n"
  "\tlea\t(18,sp),sp";

__regsused("d0/d1/d2/a0/a1/a2") LONG Locksnd(VOID) =
  "\tmove.w\t#128,-(sp)\n"
  "\ttrap\t#14\n"
  "\taddq.l\t#2,sp";

__regsused("d0/d1/d2/a0/a1/a2") LONG Unlocksnd(VOID) =
  "\tmove.w\t#129,-(sp)\n"
  "\ttrap\t#14\n"
  "\taddq.l\t#2,sp";

__regsused("d0/d1/d2/a0/a1/a2") LONG Soundcmd(__reg("d0")WORD,__reg("d1")WORD) =
  "\tmove.w\td1,-(sp)\n"
  "\tmove.w\td0,-(sp)\n"
  "\tmove.w\t#130,-(sp)\n"
  "\ttrap\t#14\n"
  "\taddq.l\t#6,sp";

__regsused("d0/d1/d2/a0/a1/a2") LONG Setbuffer(__reg("d0")WORD,__reg("a0")VOIDP,__reg("a1")VOIDP) =
  "\tmove.l\ta1,-(sp)\n"
  "\tmove.l\ta0,-(sp)\n"
  "\tmove.w\td0,-(sp)\n"
  "\tmove.w\t#131,-(sp)\n"
  "\ttrap\t#14\n"
  "\tlea\t(12,sp),sp";

__regsused("d0/d1/d2/a0/a1/a2") LONG Setmode(__reg("d0")WORD) =
  "\tmove.w\td0,-(sp)\n"
  "\tmove.w\t#132,-(sp)\n"
  "\ttrap\t#14\n"
  "\taddq.l\t#4,sp";

__regsused("d0/d1/d2/a0/a1/a2") LONG Settracks(__reg("d0")WORD,__reg("d1")WORD) =
  "\tmove.w\td1,-(sp)\n"
  "\tmove.w\td0,-(sp)\n"
  "\tmove.w\t#133,-(sp)\n"
  "\ttrap\t#14\n"
  "\taddq.l\t#6,sp";

__regsused("d0/d1/d2/a0/a1/a2") LONG Setmontracks(__reg("d0")WORD) =
  "\tmove.w\td0,-(sp)\n"
  "\tmove.w\t#134,-(sp)\n"
  "\ttrap\t#14\n"
  "\taddq.l\t#4,sp";

__regsused("d0/d1/d2/a0/a1/a2") LONG Setinterrupt(__reg("d0")WORD,__reg("d1")WORD) =
  "\tmove.w\td1,-(sp)\n"
  "\tmove.w\td0,-(sp)\n"
  "\tmove.w\t#135,-(sp)\n"
  "\ttrap\t#14\n"
  "\taddq.l\t#6,sp";

__regsused("d0/d1/d2/a0/a1/a2") LONG Buffoper(__reg("d0")WORD) =
  "\tmove.w\td0,-(sp)\n"
  "\tmove.w\t#136,-(sp)\n"
  "\ttrap\t#14\n"
  "\taddq.l\t#4,sp";

__regsused("d0/d1/d2/a0/a1/a2") LONG Dsptristate(__reg("d0")WORD,__reg("d1")WORD) =
  "\tmove.w\td1,-(sp)\n"
  "\tmove.w\td0,-(sp)\n"
  "\tmove.w\t#137,-(sp)\n"
  "\ttrap\t#14\n"
  "\taddq.l\t#6,sp";

__regsused("d0/d1/d2/a0/a1/a2") LONG Gpio(__reg("d0")WORD,__reg("d1")WORD) =
  "\tmove.w\td1,-(sp)\n"
  "\tmove.w\td0,-(sp)\n"
  "\tmove.w\t#138,-(sp)\n"
  "\ttrap\t#14\n"
  "\taddq.l\t#6,sp";

__regsused("d0/d1/d2/a0/a1/a2") LONG Devconnect(__reg("d0")WORD,__reg("d1")WORD,__reg("d2")WORD,__reg("d3")WORD,__reg("d4")WORD) =
  "\tmove.w\td4,-(sp)\n"
  "\tmove.w\td3,-(sp)\n"
  "\tmove.w\td2,-(sp)\n"
  "\tmove.w\td1,-(sp)\n"
  "\tmove.w\td0,-(sp)\n"
  "\tmove.w\t#139,-(sp)\n"
  "\ttrap\t#14\n"
  "\tlea\t(12,sp),sp";

__regsused("d0/d1/d2/a0/a1/a2") LONG Sndstatus(__reg("d0")WORD) =
  "\tmove.w\td0,-(sp)\n"
  "\tmove.w\t#140,-(sp)\n"
  "\ttrap\t#14\n"
  "\taddq.l\t#4,sp";

__regsused("d0/d1/d2/a0/a1/a2") LONG Buffptr(__reg("a0")SBUFPTR*) =
  "\tmove.l\ta0,-(sp)\n"
  "\tmove.w\t#141,-(sp)\n"
  "\ttrap\t#14\n"
  "\taddq.l\t#6,sp";

__regsused("d0/d1/d2/a0/a1/a2") WORD WavePlay(__reg("d0")WORD,__reg("d1")LONG,__reg("a0")VOIDP,__reg("d2")LONG) =
  "\tmove.l\td2,-(sp)\n"
  "\tmove.l\ta0,-(sp)\n"
  "\tmove.l\td1,-(sp)\n"
  "\tmove.w\td0,-(sp)\n"
  "\tmove.w\t#165,-(sp)\n"
  "\ttrap\t#14\n"
  "\tlea\t(16,sp),sp";

/* GEMDOS Errors */

#define EINVFN			-32
#define EFILNF			-33
#define EPTHNF			-34
#define ENHNDL			-35
#define EACCDN			-36
#define EIHNDL			-37
#define ENSMEM			-39
#define EIMBA			-40
#define EDRIVE			-46
#define ENSAME			-48
#define ENMFIL			-49
#define ELOCKED			-58
#define ENSLOCK			-59
#define ERANGE			-64
#define ENAMETOOLONG	ERANGE
#define EINTRN			-65
#define EPLFMT			-66
#define EGSBF			-67
#define EBREAK			-68
#define EXCPT			-69
#define EPTHOV			-70
#define ELOOP			-80
#define EMOUNT			-200

/* Variables used by GEMDOS */

#define DEV_READY		-1
#define DEV_BUSY		0

#define SUP_SET			0L
#define SUP_INQUIRE		1L
#define SUP_USER		0L
#define SUP_SUPER		-1L

#define FA_READONLY		0x01
#define FA_HIDDEN		0x02
#define FA_SYSTEM		0x04
#define FA_VOLUME		0x08
#define FA_DIR			0x10
#define FA_ARCHIVE		0x20

#define S_READ			0
#define S_WRITE			1
#define S_READWRITE		2

#define FO_READ         	S_READ
#define FO_WRITE        	S_WRITE
#define FO_RW           	S_READWRITE

#define SEEK_SET		0
#define SEEK_CUR		1
#define SEEK_END		2

#define FA_INQUIRE		0
#define FA_SET			1

#define MX_STRAM		0
#define MX_TTRAM		1
#define MX_PREFSTRAM	2
#define MX_PREFTTRAM	3

#define GSH_CONIN		0
#define GSH_CONOUT		1
#define GSH_AUX			2
#define GSH_PRN			3
#define GSH_BIOSCON		-1
#define GSH_BIOSAUX		-2
#define GSH_BIOSPRN		-3
#define GSH_MIDIIN		-4
#define GSH_MIDIOUT		-5

#define DEFAULT_DRIVE	0

#define PE_LOADGO		0
#define PE_LOAD			3
#define PE_GO			4
#define PE_BASEPAGE		5
#define PE_GOTHENFREE	6
#define PE_CLOADGO		100
#define PE_CGO			104
#define PE_NOSHARE		106
#define PE_REPLACE		200

#define TERM_OK			0
#define TERM_ERROR		1
#define TERM_BADPARAMS	2
#define TERM_CRASH		-1
#define TERM_CTRLC		-32

typedef struct
{
	ULONG b_free;
	ULONG b_total;
	ULONG b_secsize;
	ULONG b_clsize;
} DISKINFO;

typedef struct
{
	unsigned hour   : 5;
	unsigned minute : 6;
	unsigned second : 5;
	unsigned year   : 7;
	unsigned month  : 4;
	unsigned day    : 5;
} DATETIME;

typedef struct
{
	LONG gem_magic;			/* $87654321 if GEM present */
	LONG gem_end;			/* End address of OS RAM usage */
	LONG gem_entry;			/* Execution address of GEM */
} GEM_MUPB;

/* system variable _sysbase (0x4F2L) points to next structure         */
typedef struct _osheader
{
	UWORD		os_entry;	/* BRA to Reset Code */
	UWORD		os_version;	/* TOS Version */
	VOID		*reseth;	/* Reset Code */
	struct _osheader *os_beg;	/* Pointer to OSBASE */
	VOID		*os_end;	/* Pointer to OS end*/
	LONG		os_rsvl;	/* Reserved */
	GEM_MUPB	*os_magic;	/* Memory Usage PB */
	LONG		os_date;	/* OS Date $YYYYMMDD */
	UWORD		os_conf;	/* OS Conf.Bits */
	UWORD		os_dosdate;	/* DOS OS Date */

	/* As of TOS 1.2: */
	char		**p_root;	/* Base of OS Pool */
	char		**pkbshift;	/* Key. Shift State */
	BASEPAGE	**p_run;	/* Current Process */
	char		*p_rsv2;	/* Reserved */
} OSHEADER;

#define SYSHDR OSHEADER
#define _sysbase ((OSHEADER **)0x4F2)

/* GEMDOS Calls */

__regsused("d0/d1/d2/a0/a1/a2") VOID Pterm0(VOID) =
  "\tmove.w\t#0,-(sp)\n"
  "\ttrap\t#1\n"
  "\taddq.l\t#2,sp";

__regsused("d0/d1/d2/a0/a1/a2") LONG Cconin(VOID) =
  "\tmove.w\t#1,-(sp)\n"
  "\ttrap\t#1\n"
  "\taddq.l\t#2,sp";

__regsused("d0/d1/d2/a0/a1/a2") VOID Cconout(__reg("d0")WORD) =
  "\tmove.w\td0,-(sp)\n"
  "\tmove.w\t#2,-(sp)\n"
  "\ttrap\t#1\n"
  "\taddq.l\t#4,sp";

__regsused("d0/d1/d2/a0/a1/a2") WORD Cauxin(VOID) =
  "\tmove.w\t#3,-(sp)\n"
  "\ttrap\t#1\n"
  "\taddq.l\t#2,sp";

__regsused("d0/d1/d2/a0/a1/a2") VOID Cauxout(__reg("d0")WORD) =
  "\tmove.w\td0,-(sp)\n"
  "\tmove.w\t#4,-(sp)\n"
  "\ttrap\t#1\n"
  "\taddq.l\t#4,sp";

__regsused("d0/d1/d2/a0/a1/a2") WORD Cprnout(__reg("d0")WORD) =
  "\tmove.w\td0,-(sp)\n"
  "\tmove.w\t#5,-(sp)\n"
  "\ttrap\t#1\n"
  "\taddq.l\t#4,sp";

__regsused("d0/d1/d2/a0/a1/a2") LONG Crawio(__reg("d0")WORD) =
  "\tmove.w\td0,-(sp)\n"
  "\tmove.w\t#6,-(sp)\n"
  "\ttrap\t#1\n"
  "\taddq.l\t#4,sp";

__regsused("d0/d1/d2/a0/a1/a2") LONG Crawcin(VOID) =
  "\tmove.w\t#7,-(sp)\n"
  "\ttrap\t#1\n"
  "\taddq.l\t#2,sp";

__regsused("d0/d1/d2/a0/a1/a2") LONG Cnecin(VOID) =
  "\tmove.w\t#8,-(sp)\n"
  "\ttrap\t#1\n"
  "\taddq.l\t#2,sp";

__regsused("d0/d1/d2/a0/a1/a2") WORD Cconws(__reg("a0")char*) =
  "\tmove.l\ta0,-(sp)\n"
  "\tmove.w\t#9,-(sp)\n"
  "\ttrap\t#1\n"
  "\taddq.l\t#6,sp";

__regsused("d0/d1/d2/a0/a1/a2") VOID Cconrs(__reg("a0")char*) =
  "\tmove.l\ta0,-(sp)\n"
  "\tmove.w\t#10,-(sp)\n"
  "\ttrap\t#1\n"
  "\taddq.l\t#6,sp";

__regsused("d0/d1/d2/a0/a1/a2") WORD Cconis(VOID) =
  "\tmove.w\t#11,-(sp)\n"
  "\ttrap\t#1\n"
  "\taddq.l\t#2,sp";

__regsused("d0/d1/d2/a0/a1/a2") LONG Dsetdrv(__reg("d0")WORD) =
  "\tmove.w\td0,-(sp)\n"
  "\tmove.w\t#14,-(sp)\n"
  "\ttrap\t#1\n"
  "\taddq.l\t#4,sp";

__regsused("d0/d1/d2/a0/a1/a2") WORD Cconos(VOID) =
  "\tmove.w\t#16,-(sp)\n"
  "\ttrap\t#1\n"
  "\taddq.l\t#2,sp";

__regsused("d0/d1/d2/a0/a1/a2") WORD Cprnos(VOID) =
  "\tmove.w\t#17,-(sp)\n"
  "\ttrap\t#1\n"
  "\taddq.l\t#2,sp";

__regsused("d0/d1/d2/a0/a1/a2") WORD Cauxis(VOID) =
  "\tmove.w\t#18,-(sp)\n"
  "\ttrap\t#1\n"
  "\taddq.l\t#2,sp";

__regsused("d0/d1/d2/a0/a1/a2") WORD Cauxos(VOID) =
  "\tmove.w\t#19,-(sp)\n"
  "\ttrap\t#1\n"
  "\taddq.l\t#2,sp";

__regsused("d0/d1/d2/a0/a1/a2") LONG Maddalt(__reg("a0")VOIDP,__reg("d0")LONG) =
  "\tmove.l\td0,-(sp)\n"
  "\tmove.l\ta0,-(sp)\n"
  "\tmove.w\t#20,-(sp)\n"
  "\ttrap\t#1\n"
  "\tlea\t(10,sp),sp";

__regsused("d0/d1/d2/a0/a1/a2") WORD Dgetdrv(VOID) =
  "\tmove.w\t#25,-(sp)\n"
  "\ttrap\t#1\n"
  "\taddq.l\t#2,sp";

__regsused("d0/d1/d2/a0/a1/a2") VOID Fsetdta(__reg("a0")DTA*) =
  "\tmove.l\ta0,-(sp)\n"
  "\tmove.w\t#26,-(sp)\n"
  "\ttrap\t#1\n"
  "\taddq.l\t#6,sp";

__regsused("d0/d1/d2/a0/a1/a2") VOIDP Super(__reg("a0")VOIDP) =
  "\tmove.l\ta0,-(sp)\n"
  "\tmove.w\t#32,-(sp)\n"
  "\ttrap\t#1\n"
  "\taddq.l\t#6,sp";

__regsused("d0/d1/d2/a0/a1/a2") UWORD Tgetdate(VOID) =
  "\tmove.w\t#42,-(sp)\n"
  "\ttrap\t#1\n"
  "\taddq.l\t#2,sp";

__regsused("d0/d1/d2/a0/a1/a2") WORD Tsetdate(__reg("d0")UWORD) =
  "\tmove.w\td0,-(sp)\n"
  "\tmove.w\t#43,-(sp)\n"
  "\ttrap\t#1\n"
  "\taddq.l\t#4,sp";

__regsused("d0/d1/d2/a0/a1/a2") UWORD Tgettime(VOID) =
  "\tmove.w\t#44,-(sp)\n"
  "\ttrap\t#1\n"
  "\taddq.l\t#2,sp";

__regsused("d0/d1/d2/a0/a1/a2") WORD Tsettime(__reg("d0")UWORD) =
  "\tmove.w\td0,-(sp)\n"
  "\tmove.w\t#45,-(sp)\n"
  "\ttrap\t#1\n"
  "\taddq.l\t#4,sp";

__regsused("d0/d1/d2/a0/a1/a2") DTA* Fgetdta(VOID) =
  "\tmove.w\t#47,-(sp)\n"
  "\ttrap\t#1\n"
  "\taddq.l\t#2,sp";

__regsused("d0/d1/d2/a0/a1/a2") UWORD Sversion(VOID) =
  "\tmove.w\t#48,-(sp)\n"
  "\ttrap\t#1\n"
  "\taddq.l\t#2,sp";

__regsused("d0/d1/d2/a0/a1/a2") VOID Ptermres(__reg("d0")LONG,__reg("d1")WORD) =
  "\tmove.w\td1,-(sp)\n"
  "\tmove.l\td0,-(sp)\n"
  "\tmove.w\t#49,-(sp)\n"
  "\ttrap\t#1\n"
  "\taddq.l\t#8,sp";

__regsused("d0/d1/d2/a0/a1/a2") LONG Dfree(__reg("a0")DISKINFO*,__reg("d0")WORD) =
  "\tmove.w\td0,-(sp)\n"
  "\tmove.l\ta0,-(sp)\n"
  "\tmove.w\t#54,-(sp)\n"
  "\ttrap\t#1\n"
  "\taddq.l\t#8,sp";

__regsused("d0/d1/d2/a0/a1/a2") LONG Dcreate(__reg("a0")char*) =
  "\tmove.l\ta0,-(sp)\n"
  "\tmove.w\t#57,-(sp)\n"
  "\ttrap\t#1\n"
  "\taddq.l\t#6,sp";

__regsused("d0/d1/d2/a0/a1/a2") LONG Ddelete(__reg("a0")char*) =
  "\tmove.l\ta0,-(sp)\n"
  "\tmove.w\t#58,-(sp)\n"
  "\ttrap\t#1\n"
  "\taddq.l\t#6,sp";

__regsused("d0/d1/d2/a0/a1/a2") LONG Dsetpath(__reg("a0")char*) =
  "\tmove.l\ta0,-(sp)\n"
  "\tmove.w\t#59,-(sp)\n"
  "\ttrap\t#1\n"
  "\taddq.l\t#6,sp";

__regsused("d0/d1/d2/a0/a1/a2") LONG Fcreate(__reg("a0")char*,__reg("d0")WORD) =
  "\tmove.w\td0,-(sp)\n"
  "\tmove.l\ta0,-(sp)\n"
  "\tmove.w\t#60,-(sp)\n"
  "\ttrap\t#1\n"
  "\taddq.l\t#8,sp";

__regsused("d0/d1/d2/a0/a1/a2") LONG Fopen(__reg("a0")char*,__reg("d0")WORD) =
  "\tmove.w\td0,-(sp)\n"
  "\tmove.l\ta0,-(sp)\n"
  "\tmove.w\t#61,-(sp)\n"
  "\ttrap\t#1\n"
  "\taddq.l\t#8,sp";

__regsused("d0/d1/d2/a0/a1/a2") LONG Fclose(__reg("d0")WORD) =
  "\tmove.w\td0,-(sp)\n"
  "\tmove.w\t#62,-(sp)\n"
  "\ttrap\t#1\n"
  "\taddq.l\t#4,sp";

__regsused("d0/d1/d2/a0/a1/a2") LONG Fread(__reg("d0")WORD,__reg("d1")LONG,__reg("a0")VOIDP) =
  "\tmove.l\ta0,-(sp)\n"
  "\tmove.l\td1,-(sp)\n"
  "\tmove.w\td0,-(sp)\n"
  "\tmove.w\t#63,-(sp)\n"
  "\ttrap\t#1\n"
  "\tlea\t(12,sp),sp";

__regsused("d0/d1/d2/a0/a1/a2") LONG Fwrite(__reg("d0")WORD,__reg("d1")LONG,__reg("a0")VOIDP) =
  "\tmove.l\ta0,-(sp)\n"
  "\tmove.l\td1,-(sp)\n"
  "\tmove.w\td0,-(sp)\n"
  "\tmove.w\t#64,-(sp)\n"
  "\ttrap\t#1\n"
  "\tlea\t(12,sp),sp";

__regsused("d0/d1/d2/a0/a1/a2") LONG Fdelete(__reg("a0")char*) =
  "\tmove.l\ta0,-(sp)\n"
  "\tmove.w\t#65,-(sp)\n"
  "\ttrap\t#1\n"
  "\taddq.l\t#6,sp";

__regsused("d0/d1/d2/a0/a1/a2") LONG Fseek(__reg("d0")LONG,__reg("d1")WORD,__reg("d2")WORD) =
  "\tmove.w\td2,-(sp)\n"
  "\tmove.w\td1,-(sp)\n"
  "\tmove.l\td0,-(sp)\n"
  "\tmove.w\t#66,-(sp)\n"
  "\ttrap\t#1\n"
  "\tlea\t(10,sp),sp";

__regsused("d0/d1/d2/a0/a1/a2") LONG Fattrib(__reg("a0")char*,__reg("d0")WORD,__reg("d1")WORD) =
  "\tmove.w\td1,-(sp)\n"
  "\tmove.w\td0,-(sp)\n"
  "\tmove.l\ta0,-(sp)\n"
  "\tmove.w\t#67,-(sp)\n"
  "\ttrap\t#1\n"
  "\tlea\t(10,sp),sp";

__regsused("d0/d1/d2/a0/a1/a2") VOIDP Mxalloc(__reg("d0")LONG,__reg("d1")WORD) =
  "\tmove.w\td1,-(sp)\n"
  "\tmove.l\td0,-(sp)\n"
  "\tmove.w\t#68,-(sp)\n"
  "\ttrap\t#1\n"
  "\taddq.l\t#8,sp";

__regsused("d0/d1/d2/a0/a1/a2") LONG Fdup(__reg("d0")WORD) =
  "\tmove.w\td0,-(sp)\n"
  "\tmove.w\t#69,-(sp)\n"
  "\ttrap\t#1\n"
  "\taddq.l\t#4,sp";

__regsused("d0/d1/d2/a0/a1/a2") LONG Fforce(__reg("d0")WORD,__reg("d1")WORD) =
  "\tmove.w\td1,-(sp)\n"
  "\tmove.w\td0,-(sp)\n"
  "\tmove.w\t#70,-(sp)\n"
  "\ttrap\t#1\n"
  "\taddq.l\t#6,sp";

__regsused("d0/d1/d2/a0/a1/a2") LONG Dgetpath(__reg("a0")char*,__reg("d0")WORD) =
  "\tmove.w\td0,-(sp)\n"
  "\tmove.l\ta0,-(sp)\n"
  "\tmove.w\t#71,-(sp)\n"
  "\ttrap\t#1\n"
  "\taddq.l\t#8,sp";

__regsused("d0/d1/d2/a0/a1/a2") VOIDP Malloc(__reg("d0")LONG) =
  "\tmove.l\td0,-(sp)\n"
  "\tmove.w\t#72,-(sp)\n"
  "\ttrap\t#1\n"
  "\taddq.l\t#6,sp";

__regsused("d0/d1/d2/a0/a1/a2") WORD Mfree(__reg("a0")VOIDP) =
  "\tmove.l\ta0,-(sp)\n"
  "\tmove.w\t#73,-(sp)\n"
  "\ttrap\t#1\n"
  "\taddq.l\t#6,sp";

__regsused("d0/d1/d2/a0/a1/a2") WORD Mshrink(__reg("a0")VOIDP,__reg("d0")LONG) =
  "\tmove.l\td0,-(sp)\n"
  "\tmove.l\ta0,-(sp)\n"
  "\tclr.w\t-(sp)\n"
  "\tmove.w\t#74,-(sp)\n"
  "\ttrap\t#1\n"
  "\tlea\t(12,sp),sp";

__regsused("d0/d1/d2/a0/a1/a2") LONG Pexec(__reg("d0")WORD,__reg("a0")char*,__reg("a1")char*,__reg("a2")char*) =
  "\tmove.l\ta2,-(sp)\n"
  "\tmove.l\ta1,-(sp)\n"
  "\tmove.l\ta0,-(sp)\n"
  "\tmove.w\td0,-(sp)\n"
  "\tmove.w\t#75,-(sp)\n"
  "\ttrap\t#1\n"
  "\tlea\t(16,sp),sp";

__regsused("d0/d1/d2/a0/a1/a2") VOID Pterm(__reg("d0")WORD) =
  "\tmove.w\td0,-(sp)\n"
  "\tmove.w\t#76,-(sp)\n"
  "\ttrap\t#1\n"
  "\taddq.l\t#4,sp";

__regsused("d0/d1/d2/a0/a1/a2") WORD Fsfirst(__reg("a0")char*,__reg("d0")WORD) =
  "\tmove.w\td0,-(sp)\n"
  "\tmove.l\ta0,-(sp)\n"
  "\tmove.w\t#78,-(sp)\n"
  "\ttrap\t#1\n"
  "\taddq.l\t#8,sp";

__regsused("d0/d1/d2/a0/a1/a2") WORD Fsnext(VOID) =
  "\tmove.w\t#79,-(sp)\n"
  "\ttrap\t#1\n"
  "\taddq.l\t#2,sp";

__regsused("d0/d1/d2/a0/a1/a2") LONG Frename(__reg("a0")char*,__reg("a1")char*) =
  "\tmove.l\ta1,-(sp)\n"
  "\tmove.l\ta0,-(sp)\n"
  "\tclr.w\t-(sp)\n"
  "\tmove.w\t#86,-(sp)\n"
  "\ttrap\t#1\n"
  "\tlea\t(12,sp),sp";

__regsused("d0/d1/d2/a0/a1/a2") LONG Fdatime(__reg("a0")DATETIME*,__reg("d0")WORD,__reg("d1")WORD) =
  "\tmove.w\td1,-(sp)\n"
  "\tmove.w\td0,-(sp)\n"
  "\tmove.l\ta0,-(sp)\n"
  "\tmove.w\t#87,-(sp)\n"
  "\ttrap\t#1\n"
  "\tlea\t(10,sp),sp";

__regsused("d0/d1/d2/a0/a1/a2") LONG Flock(__reg("d0")WORD,__reg("d1")WORD,__reg("d2")LONG,__reg("d3")LONG) =
  "\tmove.l\td3,-(sp)\n"
  "\tmove.l\td2,-(sp)\n"
  "\tmove.w\td1,-(sp)\n"
  "\tmove.w\td0,-(sp)\n"
  "\tmove.w\t#92,-(sp)\n"
  "\ttrap\t#1\n"
  "\tlea\t(14,sp),sp";

#endif
