/*
 * AmiTCP/IP compatibility headers for PosixLib
 *
 * Written by Frank Wille <frank@phoenix.owl.de> in 2005
 *
 * $Id: bsdsocket.h,v 1.1 2005/04/10 19:56:41 phx Exp $
 */

#ifndef LIBRARIES_BSDSOCKET_H
#define LIBRARIES_BSDSOCKET_H

#ifndef UTILITY_TAGITEM_H
#include <utility/tagitem.h>
#endif


/*
 * Parameters for SocketBaseTagList().
 */

/* Argument passed by reference or by value. */
#define SBTF_VAL 0x0000
#define SBTF_REF 0x8000

/* Code value. */
#define SBTB_CODE 1
#define SBTS_CODE 0x3FFF
#define SBTM_CODE(td) (((td) >> SBTB_CODE) & SBTS_CODE)

/* Get/Set (read/write) selection. */
#define SBTF_GET 0
#define SBTF_SET 1

/* Macros for passing the individual tag item parameters. */
#define SBTM_GETREF(code) \
	(TAG_USER | SBTF_REF | (((code) & SBTS_CODE) << SBTB_CODE))
#define SBTM_GETVAL(code) \
	(TAG_USER | (((code) & SBTS_CODE) << SBTB_CODE))
#define SBTM_SETREF(code) \
	(TAG_USER | SBTF_REF | (((code) & SBTS_CODE) << SBTB_CODE) | SBTF_SET)
#define SBTM_SETVAL(code) \
	(TAG_USER | (((code) & SBTS_CODE) << SBTB_CODE) | SBTF_SET)

/* Masks defining the signals for Task notification */
#define SBTC_BREAKMASK		1
#define SBTC_SIGIOMASK		2
#define SBTC_SIGURGMASK		3
#define SBTC_SIGEVENTMASK	4

/* Current value of the errno and h_errno variables */
#define SBTC_ERRNO		6
#define SBTC_HERRNO		7

/* Current size of the socket descriptor table */
#define SBTC_DTABLESIZE		8

/* Link library fd allocation callback; don't use this in
 * new code!
 */
#define SBTC_FDCALLBACK		9

/* Callback actions; don't use these in new code! */
#define FDCB_FREE  0
#define FDCB_ALLOC 1
#define FDCB_CHECK 2

/* syslog variables */
#define SBTC_LOGSTAT		10
#define SBTC_LOGTAGPTR		11
#define SBTC_LOGFACILITY	12
#define SBTC_LOGMASK		13

/* Error strings */
#define SBTC_ERRNOSTRPTR	14
#define SBTC_HERRNOSTRPTR	15
#define SBTC_IOERRNOSTRPTR	16
#define SBTC_S2ERRNOSTRPTR	17
#define SBTC_S2WERRNOSTRPTR	18

/* 'errno' pointer & size */
#define SBTC_ERRNOBYTEPTR	21
#define SBTC_ERRNOWORDPTR	22
#define SBTC_ERRNOLONGPTR	24
#define SBTC_ERRNOPTR(size) \
	((size == sizeof(long))		? SBTC_ERRNOLONGPTR :	\
	((size == sizeof(short))	? SBTC_ERRNOWORDPTR :	\
	((size == sizeof(char))		? SBTC_ERRNOBYTEPTR :	\
	0)))

/* 'h_errno' pointer (with sizeof(h_errno) == sizeof(long)) */
#define SBTC_HERRNOLONGPTR	25

/* Release string pointer */
#define SBTC_RELEASESTRPTR	29


#endif /* LIBRARIES_BSDSOCKET_H */
