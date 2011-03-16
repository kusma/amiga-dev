#ifndef LIBRARIES_IFCONFIG_H
#define LIBRARIES_IFCONFIG_H \
       "$Id: ifconfig.h,v 1.2 1996/02/25 21:17:40 jraja Exp $"
/*
 *	Definitions of AmiTCP/IP ifconfig.library
 *
 *      Copyright © 1995 AmiTCP/IP Group,
 *                       Network Solutions Development, Inc.
 *                       All rights reserved.
 */
#ifndef EXEC_TYPES_H
#include <exec/types.h>
#endif

#ifndef DOS_RDARGS_H
#include <dos/rdargs.h>
#endif

#ifndef UTILITY_TAGITEM_H
#include <utility/tagitem.h>
#endif

/*
 * Name and version of the library described by this header
 */
#define IFCONFIGNAME "AmiTCP:libs/ifconfig.library"
#define IFCONFIGVERSION 0

/*
 * Interface type definitions. Note that these are BIT positions, and only
 * one of the bits is set in ifc_type field of if_config.
 */
#define IFCT_SANA 1
#define IFCT_SLIP 2
#define IFCT_PPP  4

/*
 * Information on each configuration item. This is used internally by the
 * library, and returned by the IfConfigGetTagInfo().
 * Note that this structure is READ ONLY when returned by IfConfigGetTagInfo(),
 * and it may grow in future (in such vase the library version number will
 * grow, too).
 */
struct ifc_confitem {
  const char * ci_Names;
  const char * ci_CanonicalName;/* canonical form of the name */
  Tag          ci_TagValue;	/* data type is encoded into the tag value */
  UWORD        ci_TypesAllowed;	/* mask of allowed interface types */
  UWORD        ci_Flags;	/* misc flags, defined below */
};
/*
 * ci_Flags definitions
 */
#define CIF_REQUIRED		(1<<0)
#define CIF_NOT                 (1<<1) /* switch defaults to 1, turn to 0 */

/*
 * Tag item values for the interface configuration information
 *
 * Note that the name and type are parsed separately, and they do not
 * have corresponding tag values.
 */

/*
 * data type number bits 16 - 23
 */
#define IFTB_TYPE 16		/* starts from 16th bit */
/*
 * argument passing convention (bit 8 WITHIN type) 
 */
#define IFTF_REF 0x80		/* not set == by value */
#define IFTM_REF(tag) (((tag) >> IFTB_TYPE) & IFTF_REF)
#define IFTS_TYPE 0x7F		/* mask after shift by IFTB_TYPE */
#define IFTM_TYPE(tag) (((tag) >> IFTB_TYPE) & IFTS_TYPE)
#define IFTM_REFTYPE(tag) (((tag) >> IFTB_TYPE) & (IFTF_REF | IFTS_TYPE))
/*
 * Code (bits 0 - 15)
 */
#define IFTS_CODE 0xFFFF
#define IFTM_CODE(tag) ((tag) & SBTS_CODE)

/*
 * Defined data types (tag value type on comments)
 */

#define CI_TYPE_TYPE		            0	/* XXX - internal only */
#define CI_TYPE_STRING		(IFTF_REF | 1)	/* null terminated string */
#define CI_TYPE_LONG		            2	/* long integer */
#define CI_TYPE_SWITCH		            3	/* boolean */
#define CI_TYPE_HEXSTRING	(IFTF_REF | 4)	/* byte length + data */
#define CI_TYPE_DOTNOT		(IFTF_REF | 5)	/* struct sockaddr_in */
#define CI_LAST_TYPE			    5	/* last type value */
/*
 * Macros to define the actual tag codes - one for each type
 */
#define IFTM_STRING(code) (TAG_USER | (CI_TYPE_STRING << IFTB_TYPE) | (code))
#define IFTM_LONG(code) (TAG_USER | (CI_TYPE_LONG << IFTB_TYPE) | (code))
#define IFTM_SWITCH(code) (TAG_USER | (CI_TYPE_SWITCH << IFTB_TYPE) | (code))
#define IFTM_HEXSTRING(code) (TAG_USER | (CI_TYPE_HEXSTRING << IFTB_TYPE) | (code))
#define IFTM_DOTNOT(code) (TAG_USER | (CI_TYPE_DOTNOT << IFTB_TYPE) | (code))

/*
 * The actual tag codes
 */
#define IF_Device	IFTM_STRING(     1) /* Exec device name */
#define IF_Unit		IFTM_LONG(       2) /* Exec device unit */
#define IF_Address	IFTM_HEXSTRING(  3) /* Sana-II HW-address */
#define IF_IPType	IFTM_LONG(       4) /* IP protocol type */
#define IF_MTU		IFTM_LONG(       5) /* (Transmit) MTU */
#define IF_SerBaud	IFTM_LONG(       6) /* Serial baud rate */
#define IF_SerBufLen	IFTM_LONG(       7) /* Serial read buffer length */
#define IF_CD		IFTM_SWITCH(     8) /* Serial flag: Carrier Detect */
#define IF_7Wire	IFTM_SWITCH(     9) /* Serial flag: CTS/RTS handshake */
#define IF_EOFMode	IFTM_SWITCH(    10) /* Serial flag: EOF-mode */
#define IF_Shared	IFTM_SWITCH(    11) /* Serial flag: Shared mode */
#define IF_UseODU	IFTM_SWITCH(    12) /* Serial flag: Use OwnDevUnit */
#define IF_ARPType	IFTM_LONG(      13) /* ARP protocol type */
#define IF_IPReq	IFTM_LONG(      14) /* # of IP read requests */
#define IF_ARPReq	IFTM_LONG(      15) /* # of ARP read requests */
#define IF_WriteReq	IFTM_LONG(      16) /* # of write requests */
#define IF_Tracking	IFTM_SWITCH(    17) /* Sana2: Type tracking */
#define IF_QuickIO	IFTM_SWITCH(    18) /* Sana2: QuickIO */
#define IF_Filter	IFTM_SWITCH(    19) /* Sana2: Use Filter Hook */
#define IF_NoARP	IFTM_SWITCH(    20) /* Flag: No ARP */
#define IF_ARPHdr	IFTM_LONG(      21) /* ?? */
#define IF_P2P		IFTM_SWITCH(    22) /* Flag: Point-to-point interface */
#define IF_Simplex	IFTM_SWITCH(    23) /* Flag: iface receives own bcasts */
#define IF_Loopback	IFTM_SWITCH(    24) /* Flag: iface is of loopback type */
#define IF_Compress	IFTM_SWITCH(    25) /* Slip flag: Compress headers */
#define IF_NoICMP	IFTM_SWITCH(    26) /* Slip flag: No ICMP traffic */
#define IF_AutoComp	IFTM_SWITCH(    27) /* Slip flag: Compress detection */
#define IF_OnlineScript IFTM_STRING(    28) /* Serial: Dialing script */
#define IF_OfflineScript IFTM_STRING(   29) /* Serial: Hangup script */
#define IF_ReconnectScript IFTM_STRING( 30) /* Serial: Reconnect script */
#define IF_IP		IFTM_DOTNOT(    31) /* IP address of the interface */
#define IF_Netmask	IFTM_DOTNOT(    32) /* Netmask of the interface */
#define IF_DestIP	IFTM_DOTNOT(    33) /* Dest. IP addr of the iface */
#define IF_Gateway	IFTM_DOTNOT(    34) /* Def. gateway for this interface */
#define IF_UseBootP	IFTM_SWITCH(    35) /* Use BootP to configure iface */
#define IF_AutoConfig	IFTM_SWITCH(    36) /* Configure this iface on startup */
#define IF_ConfigFileName IFTM_STRING(  37) /* Sana2 configuration file name */
#define IF_ConfigFileContents IFTM_STRING( 38) /* Sana2 configuration contents */
#define IF_DoOnline	IFTM_SWITCH(    39) /* AmiTCP: put device online at start */
#define IF_DoOffline	IFTM_SWITCH(    40) /* AmiTCP: put device offline at end */
#define IF_MAXTAGS	                   40  /* # of defined tag codes */

/*
 * The interface configuration structure returned by the ifconfig_find(),
 * if entry for the searched interface was found.
 */
struct ifconfig {
  struct Node		ifc_node; /* read only */
  char			ifc_name[16]; /* IFNAMSIZ */
  LONG			ifc_type; /* IFCT_SANA, IFCT_SLIP or IFCT_PPP */
  struct TagItem *	ifc_taglist;
  UBYTE			ifc_databuf[0]; /* private */
};

/*
 * The ifconfig.library functions return dos error codes + some
 * extended error codes.
 * IfConfigStrError() should be used to convert these codes to 
 * english language text strings.
 * 
 * The normal error code is in the lower word of the returned error code
 * Upper half contains private information about the error.
 * When comparing the error code to the defined values, the upper half must 
 * be masked away with the following macro
 */

#define IFCONFIG_ERROR(x) ((unsigned short)x)

/* extended IoErr codes */
#define ERROR_IFCONFIG_BASE			1024
#define ERROR_TYPE_MUST_BE_FIRST		1024
#define ERROR_TYPE_NOT_KNOWN			1025
#define ERROR_OPTION_NOT_ALLOWED_FOR_TYPE	1026
#define ERROR_OPTION_NOT_KNOWN			1027
#define ERROR_IFCONFIG_MAX			1027

#endif /* LIBRARIES_IFCONFIG_H */
