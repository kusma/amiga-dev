#ifdef AMIGA
#include <exec/ports.h>
#undef PF_ACTION
#endif /* AMIGA */

/*  A Bison parser, made from grammar.y
 by  GNU Bison version 1.27
  */

#define YYBISON 1  /* Identify Bison output.  */

#define yyparse pcap_parse
#define yylex pcap_lex
#define yyerror pcap_error
#define yylval pcap_lval
#define yychar pcap_char
#define yydebug pcap_debug
#define yynerrs pcap_nerrs
#define	DST	257
#define	SRC	258
#define	HOST	259
#define	GATEWAY	260
#define	NET	261
#define	NETMASK	262
#define	PORT	263
#define	LESS	264
#define	GREATER	265
#define	PROTO	266
#define	PROTOCHAIN	267
#define	CBYTE	268
#define	ARP	269
#define	RARP	270
#define	IP	271
#define	SCTP	272
#define	TCP	273
#define	UDP	274
#define	ICMP	275
#define	IGMP	276
#define	IGRP	277
#define	PIM	278
#define	VRRP	279
#define	ATALK	280
#define	AARP	281
#define	DECNET	282
#define	LAT	283
#define	SCA	284
#define	MOPRC	285
#define	MOPDL	286
#define	TK_BROADCAST	287
#define	TK_MULTICAST	288
#define	NUM	289
#define	INBOUND	290
#define	OUTBOUND	291
#define	PF_IFNAME	292
#define	PF_RNR	293
#define	PF_REASON	294
#define	PF_ACTION	295
#define	LINK	296
#define	GEQ	297
#define	LEQ	298
#define	NEQ	299
#define	ID	300
#define	EID	301
#define	HID	302
#define	HID6	303
#define	AID	304
#define	LSH	305
#define	RSH	306
#define	LEN	307
#define	IPV6	308
#define	ICMPV6	309
#define	AH	310
#define	ESP	311
#define	VLAN	312
#define	ISO	313
#define	ESIS	314
#define	CLNP	315
#define	ISIS	316
#define	L1	317
#define	L2	318
#define	IIH	319
#define	LSP	320
#define	SNP	321
#define	CSNP	322
#define	PSNP	323
#define	STP	324
#define	IPX	325
#define	NETBEUI	326
#define	LANE	327
#define	LLC	328
#define	METAC	329
#define	BCC	330
#define	SC	331
#define	ILMIC	332
#define	OAMF4EC	333
#define	OAMF4SC	334
#define	OAM	335
#define	OAMF4	336
#define	CONNECTMSG	337
#define	METACONNECT	338
#define	VPI	339
#define	VCI	340
#define	OR	341
#define	AND	342
#define	UMINUS	343

#line 1 "grammar.y"

/*
 * Copyright (c) 1988, 1989, 1990, 1991, 1992, 1993, 1994, 1995, 1996
 *	The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that: (1) source code distributions
 * retain the above copyright notice and this paragraph in its entirety, (2)
 * distributions including binary code include the above copyright notice and
 * this paragraph in its entirety in the documentation or other materials
 * provided with the distribution, and (3) all advertising materials mentioning
 * features or use of this software display the following acknowledgement:
 * ``This product includes software developed by the University of California,
 * Lawrence Berkeley Laboratory and its contributors.'' Neither the name of
 * the University nor the names of its contributors may be used to endorse
 * or promote products derived from this software without specific prior
 * written permission.
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 */
#ifndef lint
static const char rcsid[] _U_ =
    "@(#) $Header: /Users/olsen/Code/cvs2svn-2010/os4-cvs-repository/contrib/roadshow/client-software/libpcap-0.8.1/grammar.c,v 1.1 2004-10-18 16:23:16 obarthel Exp $ (LBL)";
#endif

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef WIN32
#include <pcap-stdinc.h>
#else /* WIN32 */
#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#endif /* WIN32 */

#include <stdlib.h>

#ifndef WIN32
#if __STDC__
struct mbuf;
struct rtentry;
#endif

#include <netinet/in.h>
#endif /* WIN32 */

#include <stdio.h>

#include "pcap-int.h"

#include "gencode.h"
#include "pf.h"
#include <pcap-namedb.h>

#ifdef HAVE_OS_PROTO_H
#include "os-proto.h"
#endif

#define QSET(q, p, d, a) (q).proto = (p),\
			 (q).dir = (d),\
			 (q).addr = (a)

int n_errors = 0;

static struct qual qerr = { Q_UNDEF, Q_UNDEF, Q_UNDEF, Q_UNDEF };

static void
yyerror(char *msg)
{
	++n_errors;
	bpf_error("%s", msg);
	/* NOTREACHED */
}

#ifndef YYBISON
int yyparse(void);

int
pcap_parse()
{
	return (yyparse());
}
#endif


#line 91 "grammar.y"
typedef union {
	int i;
	bpf_u_int32 h;
	u_char *e;
	char *s;
	struct stmt *stmt;
	struct arth *a;
	struct {
		struct qual q;
		int atmfieldtype;
		struct block *b;
	} blk;
	struct block *rblk;
} YYSTYPE;
#include <stdio.h>

#ifndef __cplusplus
#ifndef __STDC__
#ifndef const
#define const
#endif
#endif
#endif



#define	YYFINAL		222
#define	YYFLAG		-32768
#define	YYNTBASE	105

#define YYTRANSLATE(x) ((unsigned)(x) <= 343 ? yytranslate[x] : 140)

static const char yytranslate[] = {     0,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,    89,     2,     2,     2,     2,    91,     2,    98,
    97,    94,    92,     2,    93,     2,    95,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,   104,     2,   101,
   100,    99,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
   102,     2,   103,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,    90,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     1,     3,     4,     5,     6,
     7,     8,     9,    10,    11,    12,    13,    14,    15,    16,
    17,    18,    19,    20,    21,    22,    23,    24,    25,    26,
    27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
    37,    38,    39,    40,    41,    42,    43,    44,    45,    46,
    47,    48,    49,    50,    51,    52,    53,    54,    55,    56,
    57,    58,    59,    60,    61,    62,    63,    64,    65,    66,
    67,    68,    69,    70,    71,    72,    73,    74,    75,    76,
    77,    78,    79,    80,    81,    82,    83,    84,    85,    86,
    87,    88,    96
};

#if YYDEBUG != 0
static const short yyprhs[] = {     0,
     0,     3,     5,     6,     8,    12,    16,    20,    24,    26,
    28,    30,    32,    36,    38,    42,    46,    48,    52,    54,
    56,    58,    61,    63,    65,    67,    71,    75,    77,    79,
    81,    84,    88,    91,    94,    97,   100,   103,   106,   110,
   112,   116,   120,   122,   124,   126,   129,   131,   132,   134,
   136,   140,   144,   148,   152,   154,   156,   158,   160,   162,
   164,   166,   168,   170,   172,   174,   176,   178,   180,   182,
   184,   186,   188,   190,   192,   194,   196,   198,   200,   202,
   204,   206,   208,   210,   212,   214,   216,   218,   220,   222,
   224,   226,   228,   230,   232,   234,   237,   240,   243,   246,
   251,   253,   255,   258,   260,   262,   265,   268,   271,   274,
   276,   278,   280,   282,   284,   286,   288,   290,   292,   294,
   296,   301,   308,   312,   316,   320,   324,   328,   332,   336,
   340,   343,   347,   349,   351,   353,   355,   357,   359,   361,
   365,   367,   369,   371,   373,   375,   377,   379,   381,   383,
   385,   387,   389,   391,   393,   395,   398,   401,   405,   407,
   409
};

static const short yyrhs[] = {   106,
   107,     0,   106,     0,     0,   116,     0,   107,   108,   116,
     0,   107,   108,   110,     0,   107,   109,   116,     0,   107,
   109,   110,     0,    88,     0,    87,     0,   111,     0,   133,
     0,   113,   114,    97,     0,    46,     0,    48,    95,    35,
     0,    48,     8,    48,     0,    48,     0,    49,    95,    35,
     0,    49,     0,    47,     0,    50,     0,   112,   110,     0,
    89,     0,    98,     0,   111,     0,   115,   108,   110,     0,
   115,   109,   110,     0,   133,     0,   114,     0,   118,     0,
   112,   116,     0,   119,   120,   121,     0,   119,   120,     0,
   119,   121,     0,   119,    12,     0,   119,    13,     0,   119,
   122,     0,   117,   110,     0,   113,   107,    97,     0,   123,
     0,   130,   128,   130,     0,   130,   129,   130,     0,   124,
     0,   134,     0,   135,     0,   136,   137,     0,   123,     0,
     0,     4,     0,     3,     0,     4,    87,     3,     0,     3,
    87,     4,     0,     4,    88,     3,     0,     3,    88,     4,
     0,     5,     0,     7,     0,     9,     0,     6,     0,    42,
     0,    17,     0,    15,     0,    16,     0,    18,     0,    19,
     0,    20,     0,    21,     0,    22,     0,    23,     0,    24,
     0,    25,     0,    26,     0,    27,     0,    28,     0,    29,
     0,    30,     0,    32,     0,    31,     0,    54,     0,    55,
     0,    56,     0,    57,     0,    59,     0,    60,     0,    62,
     0,    63,     0,    64,     0,    65,     0,    66,     0,    67,
     0,    69,     0,    68,     0,    61,     0,    70,     0,    71,
     0,    72,     0,   119,    33,     0,   119,    34,     0,    10,
    35,     0,    11,    35,     0,    14,    35,   132,    35,     0,
    36,     0,    37,     0,    58,   133,     0,    58,     0,   125,
     0,    38,    46,     0,    39,    35,     0,    40,   126,     0,
    41,   127,     0,    35,     0,    46,     0,    46,     0,    99,
     0,    43,     0,   100,     0,    44,     0,   101,     0,    45,
     0,   133,     0,   131,     0,   123,   102,   130,   103,     0,
   123,   102,   130,   104,    35,   103,     0,   130,    92,   130,
     0,   130,    93,   130,     0,   130,    94,   130,     0,   130,
    95,   130,     0,   130,    91,   130,     0,   130,    90,   130,
     0,   130,    51,   130,     0,   130,    52,   130,     0,    93,
   130,     0,   113,   131,    97,     0,    53,     0,    91,     0,
    90,     0,   101,     0,    99,     0,   100,     0,    35,     0,
   113,   133,    97,     0,    73,     0,    74,     0,    75,     0,
    76,     0,    79,     0,    80,     0,    77,     0,    78,     0,
    81,     0,    82,     0,    83,     0,    84,     0,    85,     0,
    86,     0,   138,     0,   128,    35,     0,   129,    35,     0,
   113,   139,    97,     0,    35,     0,   138,     0,   139,   109,
   138,     0
};

#endif

#if YYDEBUG != 0
static const short yyrline[] = { 0,
   154,   158,   160,   162,   163,   164,   165,   166,   168,   170,
   172,   173,   175,   177,   178,   180,   182,   187,   196,   205,
   214,   223,   225,   227,   229,   230,   231,   233,   235,   237,
   238,   240,   241,   242,   243,   244,   245,   247,   248,   249,
   250,   252,   254,   255,   256,   257,   260,   261,   264,   265,
   266,   267,   268,   269,   272,   273,   274,   277,   279,   280,
   281,   282,   283,   284,   285,   286,   287,   288,   289,   290,
   291,   292,   293,   294,   295,   296,   297,   298,   299,   300,
   301,   302,   303,   304,   305,   306,   307,   308,   309,   310,
   311,   312,   313,   314,   315,   317,   318,   319,   320,   321,
   322,   323,   324,   325,   326,   329,   330,   331,   332,   335,
   336,   349,   360,   361,   362,   364,   365,   366,   368,   369,
   371,   372,   373,   374,   375,   376,   377,   378,   379,   380,
   381,   382,   383,   385,   386,   387,   388,   389,   391,   392,
   394,   395,   396,   397,   398,   399,   400,   401,   403,   404,
   405,   406,   409,   410,   412,   413,   414,   415,   417,   424,
   425
};
#endif


#if YYDEBUG != 0 || defined (YYERROR_VERBOSE)

static const char * const yytname[] = {   "$","error","$undefined.","DST","SRC",
"HOST","GATEWAY","NET","NETMASK","PORT","LESS","GREATER","PROTO","PROTOCHAIN",
"CBYTE","ARP","RARP","IP","SCTP","TCP","UDP","ICMP","IGMP","IGRP","PIM","VRRP",
"ATALK","AARP","DECNET","LAT","SCA","MOPRC","MOPDL","TK_BROADCAST","TK_MULTICAST",
"NUM","INBOUND","OUTBOUND","PF_IFNAME","PF_RNR","PF_REASON","PF_ACTION","LINK",
"GEQ","LEQ","NEQ","ID","EID","HID","HID6","AID","LSH","RSH","LEN","IPV6","ICMPV6",
"AH","ESP","VLAN","ISO","ESIS","CLNP","ISIS","L1","L2","IIH","LSP","SNP","CSNP",
"PSNP","STP","IPX","NETBEUI","LANE","LLC","METAC","BCC","SC","ILMIC","OAMF4EC",
"OAMF4SC","OAM","OAMF4","CONNECTMSG","METACONNECT","VPI","VCI","OR","AND","'!'",
"'|'","'&'","'+'","'-'","'*'","'/'","UMINUS","')'","'('","'>'","'='","'<'","'['",
"']'","':'","prog","null","expr","and","or","id","nid","not","paren","pid","qid",
"term","head","rterm","pqual","dqual","aqual","ndaqual","pname","other","pfvar",
"reason","action","relop","irelop","arth","narth","byteop","pnum","atmtype",
"atmmultitype","atmfield","atmvalue","atmfieldvalue","atmlistvalue", NULL
};
#endif

static const short yyr1[] = {     0,
   105,   105,   106,   107,   107,   107,   107,   107,   108,   109,
   110,   110,   110,   111,   111,   111,   111,   111,   111,   111,
   111,   111,   112,   113,   114,   114,   114,   115,   115,   116,
   116,   117,   117,   117,   117,   117,   117,   118,   118,   118,
   118,   118,   118,   118,   118,   118,   119,   119,   120,   120,
   120,   120,   120,   120,   121,   121,   121,   122,   123,   123,
   123,   123,   123,   123,   123,   123,   123,   123,   123,   123,
   123,   123,   123,   123,   123,   123,   123,   123,   123,   123,
   123,   123,   123,   123,   123,   123,   123,   123,   123,   123,
   123,   123,   123,   123,   123,   124,   124,   124,   124,   124,
   124,   124,   124,   124,   124,   125,   125,   125,   125,   126,
   126,   127,   128,   128,   128,   129,   129,   129,   130,   130,
   131,   131,   131,   131,   131,   131,   131,   131,   131,   131,
   131,   131,   131,   132,   132,   132,   132,   132,   133,   133,
   134,   134,   134,   134,   134,   134,   134,   134,   135,   135,
   135,   135,   136,   136,   137,   137,   137,   137,   138,   139,
   139
};

static const short yyr2[] = {     0,
     2,     1,     0,     1,     3,     3,     3,     3,     1,     1,
     1,     1,     3,     1,     3,     3,     1,     3,     1,     1,
     1,     2,     1,     1,     1,     3,     3,     1,     1,     1,
     2,     3,     2,     2,     2,     2,     2,     2,     3,     1,
     3,     3,     1,     1,     1,     2,     1,     0,     1,     1,
     3,     3,     3,     3,     1,     1,     1,     1,     1,     1,
     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
     1,     1,     1,     1,     1,     2,     2,     2,     2,     4,
     1,     1,     2,     1,     1,     2,     2,     2,     2,     1,
     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
     4,     6,     3,     3,     3,     3,     3,     3,     3,     3,
     2,     3,     1,     1,     1,     1,     1,     1,     1,     3,
     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
     1,     1,     1,     1,     1,     2,     2,     3,     1,     1,
     3
};

static const short yydefact[] = {     3,
    48,     0,     0,     0,    61,    62,    60,    63,    64,    65,
    66,    67,    68,    69,    70,    71,    72,    73,    74,    75,
    77,    76,   139,   101,   102,     0,     0,     0,     0,    59,
   133,    78,    79,    80,    81,   104,    82,    83,    92,    84,
    85,    86,    87,    88,    89,    91,    90,    93,    94,    95,
   141,   142,   143,   144,   147,   148,   145,   146,   149,   150,
   151,   152,   153,   154,    23,     0,    24,     1,    48,    48,
     4,     0,    30,     0,    47,    43,   105,     0,   120,   119,
    44,    45,     0,    98,    99,     0,   106,   107,   110,   111,
   108,   112,   109,     0,   103,     0,     0,   131,    10,     9,
    48,    48,    31,     0,   120,   119,    14,    20,    17,    19,
    21,    38,    11,     0,     0,    12,    50,    49,    55,    58,
    56,    57,    35,    36,    96,    97,    33,    34,    37,     0,
   114,   116,   118,     0,     0,     0,     0,     0,     0,     0,
     0,   113,   115,   117,     0,     0,   159,     0,     0,     0,
    46,   155,   135,   134,   137,   138,   136,     0,     0,     0,
     6,    48,    48,     5,   119,     8,     7,    39,   132,   140,
     0,     0,     0,    22,    25,    29,     0,    28,     0,     0,
     0,     0,    32,     0,   129,   130,   128,   127,   123,   124,
   125,   126,    41,    42,   160,     0,   156,   157,   100,   119,
    16,    15,    18,    13,     0,     0,    52,    54,    51,    53,
   121,     0,   158,     0,    26,    27,     0,   161,   122,     0,
     0,     0
};

static const short yydefgoto[] = {   220,
     1,   104,   101,   102,   174,   113,   114,    96,   176,   177,
    71,    72,    73,    74,   127,   128,   129,    97,    76,    77,
    91,    93,   145,   146,    78,    79,   158,    80,    81,    82,
    83,   151,   152,   196
};

static const short yypact[] = {-32768,
   171,   -25,    -9,    16,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,    -4,    31,   -21,    24,-32768,
-32768,-32768,-32768,-32768,-32768,   -19,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,   425,-32768,   -54,   341,   341,
-32768,    77,-32768,    15,     1,-32768,-32768,   468,-32768,-32768,
-32768,-32768,    17,-32768,-32768,   -46,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,   -19,-32768,   425,   -18,-32768,-32768,-32768,
   256,   256,-32768,   -41,   -14,   -10,-32768,-32768,    -3,     0,
-32768,-32768,-32768,    77,    77,-32768,   -49,   -24,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,     8,-32768,-32768,   425,
-32768,-32768,-32768,   425,   425,   425,   425,   425,   425,   425,
   425,-32768,-32768,-32768,   425,   425,-32768,    56,    59,    64,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,    67,   -10,   127,
-32768,   256,   256,-32768,    23,-32768,-32768,-32768,-32768,-32768,
    71,    86,    87,-32768,-32768,    35,   -54,   -10,   133,   134,
   136,   137,-32768,    53,    36,    36,   295,   377,   -36,   -36,
-32768,-32768,   127,   127,-32768,   -47,-32768,-32768,-32768,    -7,
-32768,-32768,-32768,-32768,    77,    77,-32768,-32768,-32768,-32768,
-32768,   106,-32768,    56,-32768,-32768,    39,-32768,-32768,   149,
   150,-32768
};

static const short yypgoto[] = {-32768,
-32768,   151,   -26,  -166,   -70,  -106,     5,    -1,-32768,-32768,
   -65,-32768,-32768,-32768,-32768,    26,-32768,     7,-32768,-32768,
-32768,-32768,    72,    75,   369,   -67,-32768,   -29,-32768,-32768,
-32768,-32768,  -136,-32768
};


#define	YYLAST		569


static const short yytable[] = {    70,
   -40,   112,   105,   103,   171,    69,    95,    75,   175,    84,
   206,   195,   119,    89,   121,    23,   122,   117,   118,   119,
   120,   121,   -12,   122,    90,    85,   123,   124,   105,   214,
   161,   166,    99,   100,    94,   164,   167,   179,   180,    99,
   106,    87,   116,   153,   154,    99,   100,   125,   126,   213,
    86,   147,   155,   156,   157,   168,   175,   140,   141,   131,
   132,   133,   181,   182,   159,    88,   106,    70,    70,    92,
   115,   165,   165,    69,    69,    75,    75,   218,    67,   -28,
   -28,   148,   169,   130,   116,   178,   170,   -40,   -40,   170,
   147,   172,    94,   197,   173,   105,   103,   -40,   198,   163,
   163,   199,   130,   134,   135,   162,   162,    75,    75,   -12,
   -12,    23,   115,    94,    67,   142,   143,   144,   201,   -12,
   202,   203,   107,   108,   109,   110,   111,   138,   139,   140,
   141,   204,   165,   200,   215,   216,   207,   208,   209,   210,
   217,   219,   136,   137,   138,   139,   140,   141,   221,   222,
   205,    68,   183,     0,   149,   211,   212,   150,     0,     0,
   163,    70,     0,     0,     0,    65,   162,   162,    75,    75,
    -2,     0,     0,     0,    67,   116,   116,   134,   135,     0,
     2,     3,     0,     0,     4,     5,     6,     7,     8,     9,
    10,    11,    12,    13,    14,    15,    16,    17,    18,    19,
    20,    21,    22,   115,   115,    23,    24,    25,    26,    27,
    28,    29,    30,     0,     0,     0,   136,   137,   138,   139,
   140,   141,     0,    31,    32,    33,    34,    35,    36,    37,
    38,    39,    40,    41,    42,    43,    44,    45,    46,    47,
    48,    49,    50,    51,    52,    53,    54,    55,    56,    57,
    58,    59,    60,    61,    62,    63,    64,     0,     0,    65,
     0,     0,     0,    66,     0,     2,     3,     0,    67,     4,
     5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
    15,    16,    17,    18,    19,    20,    21,    22,     0,     0,
    23,    24,    25,    26,    27,    28,    29,    30,     0,     0,
     0,   107,   108,   109,   110,   111,     0,     0,    31,    32,
    33,    34,    35,    36,    37,    38,    39,    40,    41,    42,
    43,    44,    45,    46,    47,    48,    49,    50,    51,    52,
    53,    54,    55,    56,    57,    58,    59,    60,    61,    62,
    63,    64,     0,     0,    65,   134,   135,     0,    66,     0,
     2,     3,     0,    67,     4,     5,     6,     7,     8,     9,
    10,    11,    12,    13,    14,    15,    16,    17,    18,    19,
    20,    21,    22,     0,     0,    23,    24,    25,    26,    27,
    28,    29,    30,     0,     0,   137,   138,   139,   140,   141,
     0,     0,     0,    31,    32,    33,    34,    35,    36,    37,
    38,    39,    40,    41,    42,    43,    44,    45,    46,    47,
    48,    49,    50,    51,    52,    53,    54,    55,    56,    57,
    58,    59,    60,    61,    62,    63,    64,   134,   135,    65,
     0,     0,     0,    66,    98,     0,     0,     0,    67,     5,
     6,     7,     8,     9,    10,    11,    12,    13,    14,    15,
    16,    17,    18,    19,    20,    21,    22,     0,     0,    23,
     0,     0,     0,     0,   160,     0,    30,     0,   138,   139,
   140,   141,     0,     0,     0,     0,     0,    31,    32,    33,
    34,    35,     0,    37,    38,    39,    40,    41,    42,    43,
    44,    45,    46,    47,    48,    49,    50,     0,   184,     0,
     0,     0,   185,   186,   187,   188,   189,   190,   191,   192,
   131,   132,   133,   193,   194,     0,     0,    66,   134,   135,
     0,     0,    67,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,   136,   137,   138,
   139,   140,   141,     0,     0,     0,   142,   143,   144
};

static const short yycheck[] = {     1,
     0,    72,    70,    69,     8,     1,    36,     1,   115,    35,
   177,   148,     5,    35,     7,    35,     9,     3,     4,     5,
     6,     7,     0,     9,    46,    35,    12,    13,    96,   196,
   101,   102,    87,    88,    36,   101,   102,    87,    88,    87,
    70,    46,    72,    90,    91,    87,    88,    33,    34,    97,
    35,    35,    99,   100,   101,    97,   163,    94,    95,    43,
    44,    45,    87,    88,    94,    35,    96,    69,    70,    46,
    72,   101,   102,    69,    70,    69,    70,   214,    98,    87,
    88,    83,    97,   102,   114,   115,    97,    87,    88,    97,
    35,    95,    94,    35,    95,   163,   162,    97,    35,   101,
   102,    35,   102,    51,    52,   101,   102,   101,   102,    87,
    88,    35,   114,   115,    98,    99,   100,   101,    48,    97,
    35,    35,    46,    47,    48,    49,    50,    92,    93,    94,
    95,    97,   162,   163,   205,   206,     4,     4,     3,     3,
    35,   103,    90,    91,    92,    93,    94,    95,     0,     0,
   177,     1,   127,    -1,    83,   103,   104,    83,    -1,    -1,
   162,   163,    -1,    -1,    -1,    89,   162,   163,   162,   163,
     0,    -1,    -1,    -1,    98,   205,   206,    51,    52,    -1,
    10,    11,    -1,    -1,    14,    15,    16,    17,    18,    19,
    20,    21,    22,    23,    24,    25,    26,    27,    28,    29,
    30,    31,    32,   205,   206,    35,    36,    37,    38,    39,
    40,    41,    42,    -1,    -1,    -1,    90,    91,    92,    93,
    94,    95,    -1,    53,    54,    55,    56,    57,    58,    59,
    60,    61,    62,    63,    64,    65,    66,    67,    68,    69,
    70,    71,    72,    73,    74,    75,    76,    77,    78,    79,
    80,    81,    82,    83,    84,    85,    86,    -1,    -1,    89,
    -1,    -1,    -1,    93,    -1,    10,    11,    -1,    98,    14,
    15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
    25,    26,    27,    28,    29,    30,    31,    32,    -1,    -1,
    35,    36,    37,    38,    39,    40,    41,    42,    -1,    -1,
    -1,    46,    47,    48,    49,    50,    -1,    -1,    53,    54,
    55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
    65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
    75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
    85,    86,    -1,    -1,    89,    51,    52,    -1,    93,    -1,
    10,    11,    -1,    98,    14,    15,    16,    17,    18,    19,
    20,    21,    22,    23,    24,    25,    26,    27,    28,    29,
    30,    31,    32,    -1,    -1,    35,    36,    37,    38,    39,
    40,    41,    42,    -1,    -1,    91,    92,    93,    94,    95,
    -1,    -1,    -1,    53,    54,    55,    56,    57,    58,    59,
    60,    61,    62,    63,    64,    65,    66,    67,    68,    69,
    70,    71,    72,    73,    74,    75,    76,    77,    78,    79,
    80,    81,    82,    83,    84,    85,    86,    51,    52,    89,
    -1,    -1,    -1,    93,    66,    -1,    -1,    -1,    98,    15,
    16,    17,    18,    19,    20,    21,    22,    23,    24,    25,
    26,    27,    28,    29,    30,    31,    32,    -1,    -1,    35,
    -1,    -1,    -1,    -1,    96,    -1,    42,    -1,    92,    93,
    94,    95,    -1,    -1,    -1,    -1,    -1,    53,    54,    55,
    56,    57,    -1,    59,    60,    61,    62,    63,    64,    65,
    66,    67,    68,    69,    70,    71,    72,    -1,   130,    -1,
    -1,    -1,   134,   135,   136,   137,   138,   139,   140,   141,
    43,    44,    45,   145,   146,    -1,    -1,    93,    51,    52,
    -1,    -1,    98,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    90,    91,    92,
    93,    94,    95,    -1,    -1,    -1,    99,   100,   101
};
/* -*-C-*-  Note some compilers choke on comments on `#line' lines.  */
#line 3 "/gg/share/bison.simple"
/* This file comes from bison-1.27.  */

/* Skeleton output parser for bison,
   Copyright (C) 1984, 1989, 1990 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

/* As a special exception, when this file is copied by Bison into a
   Bison output file, you may use that output file without restriction.
   This special exception was added by the Free Software Foundation
   in version 1.24 of Bison.  */

/* This is the parser code that is written into each bison parser
  when the %semantic_parser declaration is not specified in the grammar.
  It was written by Richard Stallman by simplifying the hairy parser
  used when %semantic_parser is specified.  */

#ifndef YYSTACK_USE_ALLOCA
#ifdef alloca
#define YYSTACK_USE_ALLOCA
#else /* alloca not defined */
#ifdef __GNUC__
#define YYSTACK_USE_ALLOCA
#define alloca __builtin_alloca
#else /* not GNU C.  */
#if (!defined (__STDC__) && defined (sparc)) || defined (__sparc__) || defined (__sparc) || defined (__sgi) || (defined (__sun) && defined (__i386))
#define YYSTACK_USE_ALLOCA
#include <alloca.h>
#else /* not sparc */
/* We think this test detects Watcom and Microsoft C.  */
/* This used to test MSDOS, but that is a bad idea
   since that symbol is in the user namespace.  */
#if (defined (_MSDOS) || defined (_MSDOS_)) && !defined (__TURBOC__)
#if 0 /* No need for malloc.h, which pollutes the namespace;
	 instead, just don't use alloca.  */
#include <malloc.h>
#endif
#else /* not MSDOS, or __TURBOC__ */
#if defined(_AIX)
/* I don't know what this was needed for, but it pollutes the namespace.
   So I turned it off.   rms, 2 May 1997.  */
/* #include <malloc.h>  */
 #pragma alloca
#define YYSTACK_USE_ALLOCA
#else /* not MSDOS, or __TURBOC__, or _AIX */
#if 0
#ifdef __hpux /* haible@ilog.fr says this works for HPUX 9.05 and up,
		 and on HPUX 10.  Eventually we can turn this on.  */
#define YYSTACK_USE_ALLOCA
#define alloca __builtin_alloca
#endif /* __hpux */
#endif
#endif /* not _AIX */
#endif /* not MSDOS, or __TURBOC__ */
#endif /* not sparc */
#endif /* not GNU C */
#endif /* alloca not defined */
#endif /* YYSTACK_USE_ALLOCA not defined */

#ifdef YYSTACK_USE_ALLOCA
#define YYSTACK_ALLOC alloca
#else
#define YYSTACK_ALLOC malloc
#endif

/* Note: there must be only one dollar sign in this file.
   It is replaced by the list of actions, each action
   as one case of the switch.  */

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		-2
#define YYEOF		0
#define YYACCEPT	goto yyacceptlab
#define YYABORT 	goto yyabortlab
#define YYERROR		goto yyerrlab1
/* Like YYERROR except do call yyerror.
   This remains here temporarily to ease the
   transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */
#define YYFAIL		goto yyerrlab
#define YYRECOVERING()  (!!yyerrstatus)
#define YYBACKUP(token, value) \
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    { yychar = (token), yylval = (value);			\
      yychar1 = YYTRANSLATE (yychar);				\
      YYPOPSTACK;						\
      goto yybackup;						\
    }								\
  else								\
    { yyerror ("syntax error: cannot back up"); YYERROR; }	\
while (0)

#define YYTERROR	1
#define YYERRCODE	256

#ifndef YYPURE
#define YYLEX		yylex()
#endif

#ifdef YYPURE
#ifdef YYLSP_NEEDED
#ifdef YYLEX_PARAM
#define YYLEX		yylex(&yylval, &yylloc, YYLEX_PARAM)
#else
#define YYLEX		yylex(&yylval, &yylloc)
#endif
#else /* not YYLSP_NEEDED */
#ifdef YYLEX_PARAM
#define YYLEX		yylex(&yylval, YYLEX_PARAM)
#else
#define YYLEX		yylex(&yylval)
#endif
#endif /* not YYLSP_NEEDED */
#endif

/* If nonreentrant, generate the variables here */

#ifndef YYPURE

int	yychar;			/*  the lookahead symbol		*/
YYSTYPE	yylval;			/*  the semantic value of the		*/
				/*  lookahead symbol			*/

#ifdef YYLSP_NEEDED
YYLTYPE yylloc;			/*  location data for the lookahead	*/
				/*  symbol				*/
#endif

int yynerrs;			/*  number of parse errors so far       */
#endif  /* not YYPURE */

#if YYDEBUG != 0
int yydebug;			/*  nonzero means print parse trace	*/
/* Since this is uninitialized, it does not stop multiple parsers
   from coexisting.  */
#endif

/*  YYINITDEPTH indicates the initial size of the parser's stacks	*/

#ifndef	YYINITDEPTH
#define YYINITDEPTH 200
#endif

/*  YYMAXDEPTH is the maximum size the stacks can grow to
    (effective only if the built-in stack extension method is used).  */

#if YYMAXDEPTH == 0
#undef YYMAXDEPTH
#endif

#ifndef YYMAXDEPTH
#define YYMAXDEPTH 10000
#endif

/* Define __yy_memcpy.  Note that the size argument
   should be passed with type unsigned int, because that is what the non-GCC
   definitions require.  With GCC, __builtin_memcpy takes an arg
   of type size_t, but it can handle unsigned int.  */

#if __GNUC__ > 1		/* GNU C and GNU C++ define this.  */
#define __yy_memcpy(TO,FROM,COUNT)	__builtin_memcpy(TO,FROM,COUNT)
#else				/* not GNU C or C++ */
#ifndef __cplusplus

/* This is the most reliable way to avoid incompatibilities
   in available built-in functions on various systems.  */
static void
__yy_memcpy (to, from, count)
     char *to;
     char *from;
     unsigned int count;
{
  register char *f = from;
  register char *t = to;
  register int i = count;

  while (i-- > 0)
    *t++ = *f++;
}

#else /* __cplusplus */

/* This is the most reliable way to avoid incompatibilities
   in available built-in functions on various systems.  */
static void
__yy_memcpy (char *to, char *from, unsigned int count)
{
  register char *t = to;
  register char *f = from;
  register int i = count;

  while (i-- > 0)
    *t++ = *f++;
}

#endif
#endif

#line 216 "/gg/share/bison.simple"

/* The user can define YYPARSE_PARAM as the name of an argument to be passed
   into yyparse.  The argument should have type void *.
   It should actually point to an object.
   Grammar actions can access the variable by casting it
   to the proper pointer type.  */

#ifdef YYPARSE_PARAM
#ifdef __cplusplus
#define YYPARSE_PARAM_ARG void *YYPARSE_PARAM
#define YYPARSE_PARAM_DECL
#else /* not __cplusplus */
#define YYPARSE_PARAM_ARG YYPARSE_PARAM
#define YYPARSE_PARAM_DECL void *YYPARSE_PARAM;
#endif /* not __cplusplus */
#else /* not YYPARSE_PARAM */
#define YYPARSE_PARAM_ARG
#define YYPARSE_PARAM_DECL
#endif /* not YYPARSE_PARAM */

/* Prevent warning if -Wstrict-prototypes.  */
#ifdef __GNUC__
#ifdef YYPARSE_PARAM
int yyparse (void *);
#else
int yyparse (void);
#endif
#endif

int
yyparse(YYPARSE_PARAM_ARG)
     YYPARSE_PARAM_DECL
{
  register int yystate;
  register int yyn;
  register short *yyssp;
  register YYSTYPE *yyvsp;
  int yyerrstatus;	/*  number of tokens to shift before error messages enabled */
  int yychar1 = 0;		/*  lookahead token as an internal (translated) token number */

  short	yyssa[YYINITDEPTH];	/*  the state stack			*/
  YYSTYPE yyvsa[YYINITDEPTH];	/*  the semantic value stack		*/

  short *yyss = yyssa;		/*  refer to the stacks thru separate pointers */
  YYSTYPE *yyvs = yyvsa;	/*  to allow yyoverflow to reallocate them elsewhere */

#ifdef YYLSP_NEEDED
  YYLTYPE yylsa[YYINITDEPTH];	/*  the location stack			*/
  YYLTYPE *yyls = yylsa;
  YYLTYPE *yylsp;

#define YYPOPSTACK   (yyvsp--, yyssp--, yylsp--)
#else
#define YYPOPSTACK   (yyvsp--, yyssp--)
#endif

  int yystacksize = YYINITDEPTH;
  int yyfree_stacks = 0;

#ifdef YYPURE
  int yychar;
  YYSTYPE yylval;
  int yynerrs;
#ifdef YYLSP_NEEDED
  YYLTYPE yylloc;
#endif
#endif

  YYSTYPE yyval;		/*  the variable used to return		*/
				/*  semantic values from the action	*/
				/*  routines				*/

  int yylen;

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Starting parse\n");
#endif

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss - 1;
  yyvsp = yyvs;
#ifdef YYLSP_NEEDED
  yylsp = yyls;
#endif

/* Push a new state, which is found in  yystate  .  */
/* In all cases, when you get here, the value and location stacks
   have just been pushed. so pushing a state here evens the stacks.  */
yynewstate:

  *++yyssp = yystate;

  if (yyssp >= yyss + yystacksize - 1)
    {
      /* Give user a chance to reallocate the stack */
      /* Use copies of these so that the &'s don't force the real ones into memory. */
      YYSTYPE *yyvs1 = yyvs;
      short *yyss1 = yyss;
#ifdef YYLSP_NEEDED
      YYLTYPE *yyls1 = yyls;
#endif

      /* Get the current used size of the three stacks, in elements.  */
      int size = yyssp - yyss + 1;

#ifdef yyoverflow
      /* Each stack pointer address is followed by the size of
	 the data in use in that stack, in bytes.  */
#ifdef YYLSP_NEEDED
      /* This used to be a conditional around just the two extra args,
	 but that might be undefined if yyoverflow is a macro.  */
      yyoverflow("parser stack overflow",
		 &yyss1, size * sizeof (*yyssp),
		 &yyvs1, size * sizeof (*yyvsp),
		 &yyls1, size * sizeof (*yylsp),
		 &yystacksize);
#else
      yyoverflow("parser stack overflow",
		 &yyss1, size * sizeof (*yyssp),
		 &yyvs1, size * sizeof (*yyvsp),
		 &yystacksize);
#endif

      yyss = yyss1; yyvs = yyvs1;
#ifdef YYLSP_NEEDED
      yyls = yyls1;
#endif
#else /* no yyoverflow */
      /* Extend the stack our own way.  */
      if (yystacksize >= YYMAXDEPTH)
	{
	  yyerror("parser stack overflow");
	  if (yyfree_stacks)
	    {
	      free (yyss);
	      free (yyvs);
#ifdef YYLSP_NEEDED
	      free (yyls);
#endif
	    }
	  return 2;
	}
      yystacksize *= 2;
      if (yystacksize > YYMAXDEPTH)
	yystacksize = YYMAXDEPTH;
#ifndef YYSTACK_USE_ALLOCA
      yyfree_stacks = 1;
#endif
      yyss = (short *) YYSTACK_ALLOC (yystacksize * sizeof (*yyssp));
      __yy_memcpy ((char *)yyss, (char *)yyss1,
		   size * (unsigned int) sizeof (*yyssp));
      yyvs = (YYSTYPE *) YYSTACK_ALLOC (yystacksize * sizeof (*yyvsp));
      __yy_memcpy ((char *)yyvs, (char *)yyvs1,
		   size * (unsigned int) sizeof (*yyvsp));
#ifdef YYLSP_NEEDED
      yyls = (YYLTYPE *) YYSTACK_ALLOC (yystacksize * sizeof (*yylsp));
      __yy_memcpy ((char *)yyls, (char *)yyls1,
		   size * (unsigned int) sizeof (*yylsp));
#endif
#endif /* no yyoverflow */

      yyssp = yyss + size - 1;
      yyvsp = yyvs + size - 1;
#ifdef YYLSP_NEEDED
      yylsp = yyls + size - 1;
#endif

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Stack size increased to %d\n", yystacksize);
#endif

      if (yyssp >= yyss + yystacksize - 1)
	YYABORT;
    }

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Entering state %d\n", yystate);
#endif

  goto yybackup;
 yybackup:

/* Do appropriate processing given the current state.  */
/* Read a lookahead token if we need one and don't already have one.  */
/* yyresume: */

  /* First try to decide what to do without reference to lookahead token.  */

  yyn = yypact[yystate];
  if (yyn == YYFLAG)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* yychar is either YYEMPTY or YYEOF
     or a valid token in external form.  */

  if (yychar == YYEMPTY)
    {
#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Reading a token: ");
#endif
      yychar = YYLEX;
    }

  /* Convert token to internal form (in yychar1) for indexing tables with */

  if (yychar <= 0)		/* This means end of input. */
    {
      yychar1 = 0;
      yychar = YYEOF;		/* Don't call YYLEX any more */

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Now at end of input.\n");
#endif
    }
  else
    {
      yychar1 = YYTRANSLATE(yychar);

#if YYDEBUG != 0
      if (yydebug)
	{
	  fprintf (stderr, "Next token is %d (%s", yychar, yytname[yychar1]);
	  /* Give the individual parser a way to print the precise meaning
	     of a token, for further debugging info.  */
#ifdef YYPRINT
	  YYPRINT (stderr, yychar, yylval);
#endif
	  fprintf (stderr, ")\n");
	}
#endif
    }

  yyn += yychar1;
  if (yyn < 0 || yyn > YYLAST || yycheck[yyn] != yychar1)
    goto yydefault;

  yyn = yytable[yyn];

  /* yyn is what to do for this token type in this state.
     Negative => reduce, -yyn is rule number.
     Positive => shift, yyn is new state.
       New state is final state => don't bother to shift,
       just return success.
     0, or most negative number => error.  */

  if (yyn < 0)
    {
      if (yyn == YYFLAG)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }
  else if (yyn == 0)
    goto yyerrlab;

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Shift the lookahead token.  */

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Shifting token %d (%s), ", yychar, yytname[yychar1]);
#endif

  /* Discard the token being shifted unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  *++yyvsp = yylval;
#ifdef YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

  /* count tokens shifted since error; after three, turn off error status.  */
  if (yyerrstatus) yyerrstatus--;

  yystate = yyn;
  goto yynewstate;

/* Do the default action for the current state.  */
yydefault:

  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;

/* Do a reduction.  yyn is the number of a rule to reduce with.  */
yyreduce:
  yylen = yyr2[yyn];
  if (yylen > 0)
    yyval = yyvsp[1-yylen]; /* implement default value of the action */

#if YYDEBUG != 0
  if (yydebug)
    {
      int i;

      fprintf (stderr, "Reducing via rule %d (line %d), ",
	       yyn, yyrline[yyn]);

      /* Print the symbols being reduced, and their result.  */
      for (i = yyprhs[yyn]; yyrhs[i] > 0; i++)
	fprintf (stderr, "%s ", yytname[yyrhs[i]]);
      fprintf (stderr, " -> %s\n", yytname[yyr1[yyn]]);
    }
#endif


  switch (yyn) {

case 1:
#line 155 "grammar.y"
{
	finish_parse(yyvsp[0].blk.b);
;
    break;}
case 3:
#line 160 "grammar.y"
{ yyval.blk.q = qerr; ;
    break;}
case 5:
#line 163 "grammar.y"
{ gen_and(yyvsp[-2].blk.b, yyvsp[0].blk.b); yyval.blk = yyvsp[0].blk; ;
    break;}
case 6:
#line 164 "grammar.y"
{ gen_and(yyvsp[-2].blk.b, yyvsp[0].blk.b); yyval.blk = yyvsp[0].blk; ;
    break;}
case 7:
#line 165 "grammar.y"
{ gen_or(yyvsp[-2].blk.b, yyvsp[0].blk.b); yyval.blk = yyvsp[0].blk; ;
    break;}
case 8:
#line 166 "grammar.y"
{ gen_or(yyvsp[-2].blk.b, yyvsp[0].blk.b); yyval.blk = yyvsp[0].blk; ;
    break;}
case 9:
#line 168 "grammar.y"
{ yyval.blk = yyvsp[-1].blk; ;
    break;}
case 10:
#line 170 "grammar.y"
{ yyval.blk = yyvsp[-1].blk; ;
    break;}
case 12:
#line 173 "grammar.y"
{ yyval.blk.b = gen_ncode(NULL, (bpf_u_int32)yyvsp[0].i,
						   yyval.blk.q = yyvsp[-1].blk.q); ;
    break;}
case 13:
#line 175 "grammar.y"
{ yyval.blk = yyvsp[-1].blk; ;
    break;}
case 14:
#line 177 "grammar.y"
{ yyval.blk.b = gen_scode(yyvsp[0].s, yyval.blk.q = yyvsp[-1].blk.q); ;
    break;}
case 15:
#line 178 "grammar.y"
{ yyval.blk.b = gen_mcode(yyvsp[-2].s, NULL, yyvsp[0].i,
				    yyval.blk.q = yyvsp[-3].blk.q); ;
    break;}
case 16:
#line 180 "grammar.y"
{ yyval.blk.b = gen_mcode(yyvsp[-2].s, yyvsp[0].s, 0,
				    yyval.blk.q = yyvsp[-3].blk.q); ;
    break;}
case 17:
#line 182 "grammar.y"
{
				  /* Decide how to parse HID based on proto */
				  yyval.blk.q = yyvsp[-1].blk.q;
				  yyval.blk.b = gen_ncode(yyvsp[0].s, 0, yyval.blk.q);
				;
    break;}
case 18:
#line 187 "grammar.y"
{
#ifdef INET6
				  yyval.blk.b = gen_mcode6(yyvsp[-2].s, NULL, yyvsp[0].i,
				    yyval.blk.q = yyvsp[-3].blk.q);
#else
				  bpf_error("'ip6addr/prefixlen' not supported "
					"in this configuration");
#endif /*INET6*/
				;
    break;}
case 19:
#line 196 "grammar.y"
{
#ifdef INET6
				  yyval.blk.b = gen_mcode6(yyvsp[0].s, 0, 128,
				    yyval.blk.q = yyvsp[-1].blk.q);
#else
				  bpf_error("'ip6addr' not supported "
					"in this configuration");
#endif /*INET6*/
				;
    break;}
case 20:
#line 205 "grammar.y"
{ 
				  yyval.blk.b = gen_ecode(yyvsp[0].e, yyval.blk.q = yyvsp[-1].blk.q);
				  /*
				   * $1 was allocated by "pcap_ether_aton()",
				   * so we must free it now that we're done
				   * with it.
				   */
				  free(yyvsp[0].e);
				;
    break;}
case 21:
#line 214 "grammar.y"
{
				  yyval.blk.b = gen_acode(yyvsp[0].e, yyval.blk.q = yyvsp[-1].blk.q);
				  /*
				   * $1 was allocated by "pcap_ether_aton()",
				   * so we must free it now that we're done
				   * with it.
				   */
				  free(yyvsp[0].e);
				;
    break;}
case 22:
#line 223 "grammar.y"
{ gen_not(yyvsp[0].blk.b); yyval.blk = yyvsp[0].blk; ;
    break;}
case 23:
#line 225 "grammar.y"
{ yyval.blk = yyvsp[-1].blk; ;
    break;}
case 24:
#line 227 "grammar.y"
{ yyval.blk = yyvsp[-1].blk; ;
    break;}
case 26:
#line 230 "grammar.y"
{ gen_and(yyvsp[-2].blk.b, yyvsp[0].blk.b); yyval.blk = yyvsp[0].blk; ;
    break;}
case 27:
#line 231 "grammar.y"
{ gen_or(yyvsp[-2].blk.b, yyvsp[0].blk.b); yyval.blk = yyvsp[0].blk; ;
    break;}
case 28:
#line 233 "grammar.y"
{ yyval.blk.b = gen_ncode(NULL, (bpf_u_int32)yyvsp[0].i,
						   yyval.blk.q = yyvsp[-1].blk.q); ;
    break;}
case 31:
#line 238 "grammar.y"
{ gen_not(yyvsp[0].blk.b); yyval.blk = yyvsp[0].blk; ;
    break;}
case 32:
#line 240 "grammar.y"
{ QSET(yyval.blk.q, yyvsp[-2].i, yyvsp[-1].i, yyvsp[0].i); ;
    break;}
case 33:
#line 241 "grammar.y"
{ QSET(yyval.blk.q, yyvsp[-1].i, yyvsp[0].i, Q_DEFAULT); ;
    break;}
case 34:
#line 242 "grammar.y"
{ QSET(yyval.blk.q, yyvsp[-1].i, Q_DEFAULT, yyvsp[0].i); ;
    break;}
case 35:
#line 243 "grammar.y"
{ QSET(yyval.blk.q, yyvsp[-1].i, Q_DEFAULT, Q_PROTO); ;
    break;}
case 36:
#line 244 "grammar.y"
{ QSET(yyval.blk.q, yyvsp[-1].i, Q_DEFAULT, Q_PROTOCHAIN); ;
    break;}
case 37:
#line 245 "grammar.y"
{ QSET(yyval.blk.q, yyvsp[-1].i, Q_DEFAULT, yyvsp[0].i); ;
    break;}
case 38:
#line 247 "grammar.y"
{ yyval.blk = yyvsp[0].blk; ;
    break;}
case 39:
#line 248 "grammar.y"
{ yyval.blk.b = yyvsp[-1].blk.b; yyval.blk.q = yyvsp[-2].blk.q; ;
    break;}
case 40:
#line 249 "grammar.y"
{ yyval.blk.b = gen_proto_abbrev(yyvsp[0].i); yyval.blk.q = qerr; ;
    break;}
case 41:
#line 250 "grammar.y"
{ yyval.blk.b = gen_relation(yyvsp[-1].i, yyvsp[-2].a, yyvsp[0].a, 0);
				  yyval.blk.q = qerr; ;
    break;}
case 42:
#line 252 "grammar.y"
{ yyval.blk.b = gen_relation(yyvsp[-1].i, yyvsp[-2].a, yyvsp[0].a, 1);
				  yyval.blk.q = qerr; ;
    break;}
case 43:
#line 254 "grammar.y"
{ yyval.blk.b = yyvsp[0].rblk; yyval.blk.q = qerr; ;
    break;}
case 44:
#line 255 "grammar.y"
{ yyval.blk.b = gen_atmtype_abbrev(yyvsp[0].i); yyval.blk.q = qerr; ;
    break;}
case 45:
#line 256 "grammar.y"
{ yyval.blk.b = gen_atmmulti_abbrev(yyvsp[0].i); yyval.blk.q = qerr; ;
    break;}
case 46:
#line 257 "grammar.y"
{ yyval.blk.b = yyvsp[0].blk.b; yyval.blk.q = qerr; ;
    break;}
case 48:
#line 261 "grammar.y"
{ yyval.i = Q_DEFAULT; ;
    break;}
case 49:
#line 264 "grammar.y"
{ yyval.i = Q_SRC; ;
    break;}
case 50:
#line 265 "grammar.y"
{ yyval.i = Q_DST; ;
    break;}
case 51:
#line 266 "grammar.y"
{ yyval.i = Q_OR; ;
    break;}
case 52:
#line 267 "grammar.y"
{ yyval.i = Q_OR; ;
    break;}
case 53:
#line 268 "grammar.y"
{ yyval.i = Q_AND; ;
    break;}
case 54:
#line 269 "grammar.y"
{ yyval.i = Q_AND; ;
    break;}
case 55:
#line 272 "grammar.y"
{ yyval.i = Q_HOST; ;
    break;}
case 56:
#line 273 "grammar.y"
{ yyval.i = Q_NET; ;
    break;}
case 57:
#line 274 "grammar.y"
{ yyval.i = Q_PORT; ;
    break;}
case 58:
#line 277 "grammar.y"
{ yyval.i = Q_GATEWAY; ;
    break;}
case 59:
#line 279 "grammar.y"
{ yyval.i = Q_LINK; ;
    break;}
case 60:
#line 280 "grammar.y"
{ yyval.i = Q_IP; ;
    break;}
case 61:
#line 281 "grammar.y"
{ yyval.i = Q_ARP; ;
    break;}
case 62:
#line 282 "grammar.y"
{ yyval.i = Q_RARP; ;
    break;}
case 63:
#line 283 "grammar.y"
{ yyval.i = Q_SCTP; ;
    break;}
case 64:
#line 284 "grammar.y"
{ yyval.i = Q_TCP; ;
    break;}
case 65:
#line 285 "grammar.y"
{ yyval.i = Q_UDP; ;
    break;}
case 66:
#line 286 "grammar.y"
{ yyval.i = Q_ICMP; ;
    break;}
case 67:
#line 287 "grammar.y"
{ yyval.i = Q_IGMP; ;
    break;}
case 68:
#line 288 "grammar.y"
{ yyval.i = Q_IGRP; ;
    break;}
case 69:
#line 289 "grammar.y"
{ yyval.i = Q_PIM; ;
    break;}
case 70:
#line 290 "grammar.y"
{ yyval.i = Q_VRRP; ;
    break;}
case 71:
#line 291 "grammar.y"
{ yyval.i = Q_ATALK; ;
    break;}
case 72:
#line 292 "grammar.y"
{ yyval.i = Q_AARP; ;
    break;}
case 73:
#line 293 "grammar.y"
{ yyval.i = Q_DECNET; ;
    break;}
case 74:
#line 294 "grammar.y"
{ yyval.i = Q_LAT; ;
    break;}
case 75:
#line 295 "grammar.y"
{ yyval.i = Q_SCA; ;
    break;}
case 76:
#line 296 "grammar.y"
{ yyval.i = Q_MOPDL; ;
    break;}
case 77:
#line 297 "grammar.y"
{ yyval.i = Q_MOPRC; ;
    break;}
case 78:
#line 298 "grammar.y"
{ yyval.i = Q_IPV6; ;
    break;}
case 79:
#line 299 "grammar.y"
{ yyval.i = Q_ICMPV6; ;
    break;}
case 80:
#line 300 "grammar.y"
{ yyval.i = Q_AH; ;
    break;}
case 81:
#line 301 "grammar.y"
{ yyval.i = Q_ESP; ;
    break;}
case 82:
#line 302 "grammar.y"
{ yyval.i = Q_ISO; ;
    break;}
case 83:
#line 303 "grammar.y"
{ yyval.i = Q_ESIS; ;
    break;}
case 84:
#line 304 "grammar.y"
{ yyval.i = Q_ISIS; ;
    break;}
case 85:
#line 305 "grammar.y"
{ yyval.i = Q_ISIS_L1; ;
    break;}
case 86:
#line 306 "grammar.y"
{ yyval.i = Q_ISIS_L2; ;
    break;}
case 87:
#line 307 "grammar.y"
{ yyval.i = Q_ISIS_IIH; ;
    break;}
case 88:
#line 308 "grammar.y"
{ yyval.i = Q_ISIS_LSP; ;
    break;}
case 89:
#line 309 "grammar.y"
{ yyval.i = Q_ISIS_SNP; ;
    break;}
case 90:
#line 310 "grammar.y"
{ yyval.i = Q_ISIS_PSNP; ;
    break;}
case 91:
#line 311 "grammar.y"
{ yyval.i = Q_ISIS_CSNP; ;
    break;}
case 92:
#line 312 "grammar.y"
{ yyval.i = Q_CLNP; ;
    break;}
case 93:
#line 313 "grammar.y"
{ yyval.i = Q_STP; ;
    break;}
case 94:
#line 314 "grammar.y"
{ yyval.i = Q_IPX; ;
    break;}
case 95:
#line 315 "grammar.y"
{ yyval.i = Q_NETBEUI; ;
    break;}
case 96:
#line 317 "grammar.y"
{ yyval.rblk = gen_broadcast(yyvsp[-1].i); ;
    break;}
case 97:
#line 318 "grammar.y"
{ yyval.rblk = gen_multicast(yyvsp[-1].i); ;
    break;}
case 98:
#line 319 "grammar.y"
{ yyval.rblk = gen_less(yyvsp[0].i); ;
    break;}
case 99:
#line 320 "grammar.y"
{ yyval.rblk = gen_greater(yyvsp[0].i); ;
    break;}
case 100:
#line 321 "grammar.y"
{ yyval.rblk = gen_byteop(yyvsp[-1].i, yyvsp[-2].i, yyvsp[0].i); ;
    break;}
case 101:
#line 322 "grammar.y"
{ yyval.rblk = gen_inbound(0); ;
    break;}
case 102:
#line 323 "grammar.y"
{ yyval.rblk = gen_inbound(1); ;
    break;}
case 103:
#line 324 "grammar.y"
{ yyval.rblk = gen_vlan(yyvsp[0].i); ;
    break;}
case 104:
#line 325 "grammar.y"
{ yyval.rblk = gen_vlan(-1); ;
    break;}
case 105:
#line 326 "grammar.y"
{ yyval.rblk = yyvsp[0].rblk; ;
    break;}
case 106:
#line 329 "grammar.y"
{ yyval.rblk = gen_pf_ifname(yyvsp[0].s); ;
    break;}
case 107:
#line 330 "grammar.y"
{ yyval.rblk = gen_pf_rnr(yyvsp[0].i); ;
    break;}
case 108:
#line 331 "grammar.y"
{ yyval.rblk = gen_pf_reason(yyvsp[0].i); ;
    break;}
case 109:
#line 332 "grammar.y"
{ yyval.rblk = gen_pf_action(yyvsp[0].i); ;
    break;}
case 110:
#line 335 "grammar.y"
{ yyval.i = yyvsp[0].i; ;
    break;}
case 111:
#line 336 "grammar.y"
{ const char *reasons[] = PFRES_NAMES;
				  int i;
				  for (i = 0; reasons[i]; i++) {
					  if (pcap_strcasecmp(yyvsp[0].s, reasons[i]) == 0) {
						  yyval.i = i;
						  break;
					  }
				  }
				  if (reasons[i] == NULL)
					  bpf_error("unknown PF reason");
				;
    break;}
case 112:
#line 349 "grammar.y"
{ if (pcap_strcasecmp(yyvsp[0].s, "pass") == 0 ||
				      pcap_strcasecmp(yyvsp[0].s, "accept") == 0)
					yyval.i = PF_PASS;
				  else if (pcap_strcasecmp(yyvsp[0].s, "drop") == 0 ||
				      pcap_strcasecmp(yyvsp[0].s, "block") == 0)
					yyval.i = PF_DROP;
				  else
					  bpf_error("unknown PF action");
				;
    break;}
case 113:
#line 360 "grammar.y"
{ yyval.i = BPF_JGT; ;
    break;}
case 114:
#line 361 "grammar.y"
{ yyval.i = BPF_JGE; ;
    break;}
case 115:
#line 362 "grammar.y"
{ yyval.i = BPF_JEQ; ;
    break;}
case 116:
#line 364 "grammar.y"
{ yyval.i = BPF_JGT; ;
    break;}
case 117:
#line 365 "grammar.y"
{ yyval.i = BPF_JGE; ;
    break;}
case 118:
#line 366 "grammar.y"
{ yyval.i = BPF_JEQ; ;
    break;}
case 119:
#line 368 "grammar.y"
{ yyval.a = gen_loadi(yyvsp[0].i); ;
    break;}
case 121:
#line 371 "grammar.y"
{ yyval.a = gen_load(yyvsp[-3].i, yyvsp[-1].a, 1); ;
    break;}
case 122:
#line 372 "grammar.y"
{ yyval.a = gen_load(yyvsp[-5].i, yyvsp[-3].a, yyvsp[-1].i); ;
    break;}
case 123:
#line 373 "grammar.y"
{ yyval.a = gen_arth(BPF_ADD, yyvsp[-2].a, yyvsp[0].a); ;
    break;}
case 124:
#line 374 "grammar.y"
{ yyval.a = gen_arth(BPF_SUB, yyvsp[-2].a, yyvsp[0].a); ;
    break;}
case 125:
#line 375 "grammar.y"
{ yyval.a = gen_arth(BPF_MUL, yyvsp[-2].a, yyvsp[0].a); ;
    break;}
case 126:
#line 376 "grammar.y"
{ yyval.a = gen_arth(BPF_DIV, yyvsp[-2].a, yyvsp[0].a); ;
    break;}
case 127:
#line 377 "grammar.y"
{ yyval.a = gen_arth(BPF_AND, yyvsp[-2].a, yyvsp[0].a); ;
    break;}
case 128:
#line 378 "grammar.y"
{ yyval.a = gen_arth(BPF_OR, yyvsp[-2].a, yyvsp[0].a); ;
    break;}
case 129:
#line 379 "grammar.y"
{ yyval.a = gen_arth(BPF_LSH, yyvsp[-2].a, yyvsp[0].a); ;
    break;}
case 130:
#line 380 "grammar.y"
{ yyval.a = gen_arth(BPF_RSH, yyvsp[-2].a, yyvsp[0].a); ;
    break;}
case 131:
#line 381 "grammar.y"
{ yyval.a = gen_neg(yyvsp[0].a); ;
    break;}
case 132:
#line 382 "grammar.y"
{ yyval.a = yyvsp[-1].a; ;
    break;}
case 133:
#line 383 "grammar.y"
{ yyval.a = gen_loadlen(); ;
    break;}
case 134:
#line 385 "grammar.y"
{ yyval.i = '&'; ;
    break;}
case 135:
#line 386 "grammar.y"
{ yyval.i = '|'; ;
    break;}
case 136:
#line 387 "grammar.y"
{ yyval.i = '<'; ;
    break;}
case 137:
#line 388 "grammar.y"
{ yyval.i = '>'; ;
    break;}
case 138:
#line 389 "grammar.y"
{ yyval.i = '='; ;
    break;}
case 140:
#line 392 "grammar.y"
{ yyval.i = yyvsp[-1].i; ;
    break;}
case 141:
#line 394 "grammar.y"
{ yyval.i = A_LANE; ;
    break;}
case 142:
#line 395 "grammar.y"
{ yyval.i = A_LLC; ;
    break;}
case 143:
#line 396 "grammar.y"
{ yyval.i = A_METAC;	;
    break;}
case 144:
#line 397 "grammar.y"
{ yyval.i = A_BCC; ;
    break;}
case 145:
#line 398 "grammar.y"
{ yyval.i = A_OAMF4EC; ;
    break;}
case 146:
#line 399 "grammar.y"
{ yyval.i = A_OAMF4SC; ;
    break;}
case 147:
#line 400 "grammar.y"
{ yyval.i = A_SC; ;
    break;}
case 148:
#line 401 "grammar.y"
{ yyval.i = A_ILMIC; ;
    break;}
case 149:
#line 403 "grammar.y"
{ yyval.i = A_OAM; ;
    break;}
case 150:
#line 404 "grammar.y"
{ yyval.i = A_OAMF4; ;
    break;}
case 151:
#line 405 "grammar.y"
{ yyval.i = A_CONNECTMSG; ;
    break;}
case 152:
#line 406 "grammar.y"
{ yyval.i = A_METACONNECT; ;
    break;}
case 153:
#line 409 "grammar.y"
{ yyval.blk.atmfieldtype = A_VPI; ;
    break;}
case 154:
#line 410 "grammar.y"
{ yyval.blk.atmfieldtype = A_VCI; ;
    break;}
case 156:
#line 413 "grammar.y"
{ yyval.blk.b = gen_atmfield_code(yyvsp[-2].blk.atmfieldtype, (u_int)yyvsp[0].i, (u_int)yyvsp[-1].i, 0); ;
    break;}
case 157:
#line 414 "grammar.y"
{ yyval.blk.b = gen_atmfield_code(yyvsp[-2].blk.atmfieldtype, (u_int)yyvsp[0].i, (u_int)yyvsp[-1].i, 1); ;
    break;}
case 158:
#line 415 "grammar.y"
{ yyval.blk.b = yyvsp[-1].blk.b; yyval.blk.q = qerr; ;
    break;}
case 159:
#line 417 "grammar.y"
{
	yyval.blk.atmfieldtype = yyvsp[-1].blk.atmfieldtype;
	if (yyval.blk.atmfieldtype == A_VPI ||
	    yyval.blk.atmfieldtype == A_VCI)
		yyval.blk.b = gen_atmfield_code(yyval.blk.atmfieldtype, (u_int) yyvsp[0].i, BPF_JEQ, 0);
	;
    break;}
case 161:
#line 425 "grammar.y"
{ gen_or(yyvsp[-2].blk.b, yyvsp[0].blk.b); yyval.blk = yyvsp[0].blk; ;
    break;}
}
   /* the action file gets copied in in place of this dollarsign */
#line 542 "/gg/share/bison.simple"

  yyvsp -= yylen;
  yyssp -= yylen;
#ifdef YYLSP_NEEDED
  yylsp -= yylen;
#endif

#if YYDEBUG != 0
  if (yydebug)
    {
      short *ssp1 = yyss - 1;
      fprintf (stderr, "state stack now");
      while (ssp1 != yyssp)
	fprintf (stderr, " %d", *++ssp1);
      fprintf (stderr, "\n");
    }
#endif

  *++yyvsp = yyval;

#ifdef YYLSP_NEEDED
  yylsp++;
  if (yylen == 0)
    {
      yylsp->first_line = yylloc.first_line;
      yylsp->first_column = yylloc.first_column;
      yylsp->last_line = (yylsp-1)->last_line;
      yylsp->last_column = (yylsp-1)->last_column;
      yylsp->text = 0;
    }
  else
    {
      yylsp->last_line = (yylsp+yylen-1)->last_line;
      yylsp->last_column = (yylsp+yylen-1)->last_column;
    }
#endif

  /* Now "shift" the result of the reduction.
     Determine what state that goes to,
     based on the state we popped back to
     and the rule number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTBASE] + *yyssp;
  if (yystate >= 0 && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTBASE];

  goto yynewstate;

yyerrlab:   /* here on detecting error */

  if (! yyerrstatus)
    /* If not already recovering from an error, report this error.  */
    {
      ++yynerrs;

#ifdef YYERROR_VERBOSE
      yyn = yypact[yystate];

      if (yyn > YYFLAG && yyn < YYLAST)
	{
	  int size = 0;
	  char *msg;
	  int x, count;

	  count = 0;
	  /* Start X at -yyn if nec to avoid negative indexes in yycheck.  */
	  for (x = (yyn < 0 ? -yyn : 0);
	       x < (sizeof(yytname) / sizeof(char *)); x++)
	    if (yycheck[x + yyn] == x)
	      size += strlen(yytname[x]) + 15, count++;
	  msg = (char *) malloc(size + 15);
	  if (msg != 0)
	    {
	      strcpy(msg, "parse error");

	      if (count < 5)
		{
		  count = 0;
		  for (x = (yyn < 0 ? -yyn : 0);
		       x < (sizeof(yytname) / sizeof(char *)); x++)
		    if (yycheck[x + yyn] == x)
		      {
			strcat(msg, count == 0 ? ", expecting `" : " or `");
			strcat(msg, yytname[x]);
			strcat(msg, "'");
			count++;
		      }
		}
	      yyerror(msg);
	      free(msg);
	    }
	  else
	    yyerror ("parse error; also virtual memory exceeded");
	}
      else
#endif /* YYERROR_VERBOSE */
	yyerror("parse error");
    }

  goto yyerrlab1;
yyerrlab1:   /* here on error raised explicitly by an action */

  if (yyerrstatus == 3)
    {
      /* if just tried and failed to reuse lookahead token after an error, discard it.  */

      /* return failure if at end of input */
      if (yychar == YYEOF)
	YYABORT;

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Discarding token %d (%s).\n", yychar, yytname[yychar1]);
#endif

      yychar = YYEMPTY;
    }

  /* Else will try to reuse lookahead token
     after shifting the error token.  */

  yyerrstatus = 3;		/* Each real token shifted decrements this */

  goto yyerrhandle;

yyerrdefault:  /* current state does not do anything special for the error token. */

#if 0
  /* This is wrong; only states that explicitly want error tokens
     should shift them.  */
  yyn = yydefact[yystate];  /* If its default is to accept any token, ok.  Otherwise pop it.*/
  if (yyn) goto yydefault;
#endif

yyerrpop:   /* pop the current state because it cannot handle the error token */

  if (yyssp == yyss) YYABORT;
  yyvsp--;
  yystate = *--yyssp;
#ifdef YYLSP_NEEDED
  yylsp--;
#endif

#if YYDEBUG != 0
  if (yydebug)
    {
      short *ssp1 = yyss - 1;
      fprintf (stderr, "Error: state stack now");
      while (ssp1 != yyssp)
	fprintf (stderr, " %d", *++ssp1);
      fprintf (stderr, "\n");
    }
#endif

yyerrhandle:

  yyn = yypact[yystate];
  if (yyn == YYFLAG)
    goto yyerrdefault;

  yyn += YYTERROR;
  if (yyn < 0 || yyn > YYLAST || yycheck[yyn] != YYTERROR)
    goto yyerrdefault;

  yyn = yytable[yyn];
  if (yyn < 0)
    {
      if (yyn == YYFLAG)
	goto yyerrpop;
      yyn = -yyn;
      goto yyreduce;
    }
  else if (yyn == 0)
    goto yyerrpop;

  if (yyn == YYFINAL)
    YYACCEPT;

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Shifting error token, ");
#endif

  *++yyvsp = yylval;
#ifdef YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

  yystate = yyn;
  goto yynewstate;

 yyacceptlab:
  /* YYACCEPT comes here.  */
  if (yyfree_stacks)
    {
      free (yyss);
      free (yyvs);
#ifdef YYLSP_NEEDED
      free (yyls);
#endif
    }
  return 0;

 yyabortlab:
  /* YYABORT comes here.  */
  if (yyfree_stacks)
    {
      free (yyss);
      free (yyvs);
#ifdef YYLSP_NEEDED
      free (yyls);
#endif
    }
  return 1;
}
#line 427 "grammar.y"

