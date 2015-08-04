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


extern YYSTYPE pcap_lval;
