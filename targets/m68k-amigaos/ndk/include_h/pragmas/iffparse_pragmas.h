#ifndef PRAGMAS_IFFPARSE_PRAGMAS_H
#define PRAGMAS_IFFPARSE_PRAGMAS_H

/*
**	$VER: iffparse_pragmas.h 40.2 (6.6.1998)
**
**	Direct ROM interface (pragma) definitions.
**
**	Copyright © 2001 Amiga, Inc.
**	    All Rights Reserved
*/

#if defined(LATTICE) || defined(__SASC) || defined(_DCC)
#ifndef __CLIB_PRAGMA_LIBCALL
#define __CLIB_PRAGMA_LIBCALL
#endif /* __CLIB_PRAGMA_LIBCALL */
#else /* __MAXON__, __STORM__ or AZTEC_C */
#ifndef __CLIB_PRAGMA_AMICALL
#define __CLIB_PRAGMA_AMICALL
#endif /* __CLIB_PRAGMA_AMICALL */
#endif /* */

#if defined(__SASC) || defined(__STORM__)
#ifndef __CLIB_PRAGMA_TAGCALL
#define __CLIB_PRAGMA_TAGCALL
#endif /* __CLIB_PRAGMA_TAGCALL */
#endif /* __MAXON__, __STORM__ or AZTEC_C */

#ifndef CLIB_IFFPARSE_PROTOS_H
#include <clib/iffparse_protos.h>
#endif /* CLIB_IFFPARSE_PROTOS_H */

/*--- functions in V36 or higher (Release 2.0) ---*/

/* Basic functions */

#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall IFFParseBase AllocIFF 1e 00
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall IFFParseBase OpenIFF 24 0802
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall IFFParseBase ParseIFF 2a 0802
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall IFFParseBase CloseIFF 30 801
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall IFFParseBase FreeIFF 36 801
#endif /* __CLIB_PRAGMA_LIBCALL */

/* Read/Write functions */

#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall IFFParseBase ReadChunkBytes 3c 09803
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall IFFParseBase WriteChunkBytes 42 09803
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall IFFParseBase ReadChunkRecords 48 109804
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall IFFParseBase WriteChunkRecords 4e 109804
#endif /* __CLIB_PRAGMA_LIBCALL */

/* Context entry/exit */

#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall IFFParseBase PushChunk 54 210804
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall IFFParseBase PopChunk 5a 801
#endif /* __CLIB_PRAGMA_LIBCALL */
/*--- (1 function slot reserved here) ---*/

/* Low-level handler installation */

#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall IFFParseBase EntryHandler 66 A9210806
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall IFFParseBase ExitHandler 6c A9210806
#endif /* __CLIB_PRAGMA_LIBCALL */

/* Built-in chunk/property handlers */

#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall IFFParseBase PropChunk 72 10803
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall IFFParseBase PropChunks 78 09803
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall IFFParseBase StopChunk 7e 10803
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall IFFParseBase StopChunks 84 09803
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall IFFParseBase CollectionChunk 8a 10803
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall IFFParseBase CollectionChunks 90 09803
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall IFFParseBase StopOnExit 96 10803
#endif /* __CLIB_PRAGMA_LIBCALL */

/* Context utilities */

#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall IFFParseBase FindProp 9c 10803
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall IFFParseBase FindCollection a2 10803
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall IFFParseBase FindPropContext a8 801
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall IFFParseBase CurrentChunk ae 801
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall IFFParseBase ParentChunk b4 801
#endif /* __CLIB_PRAGMA_LIBCALL */

/* LocalContextItem support functions */

#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall IFFParseBase AllocLocalItem ba 321004
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall IFFParseBase LocalItemData c0 801
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall IFFParseBase SetLocalItemPurge c6 9802
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall IFFParseBase FreeLocalItem cc 801
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall IFFParseBase FindLocalItem d2 210804
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall IFFParseBase StoreLocalItem d8 09803
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall IFFParseBase StoreItemInContext de A9803
#endif /* __CLIB_PRAGMA_LIBCALL */

/* IFFHandle initialization */

#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall IFFParseBase InitIFF e4 90803
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall IFFParseBase InitIFFasDOS ea 801
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall IFFParseBase InitIFFasClip f0 801
#endif /* __CLIB_PRAGMA_LIBCALL */

/* Internal clipboard support */

#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall IFFParseBase OpenClipboard f6 001
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall IFFParseBase CloseClipboard fc 801
#endif /* __CLIB_PRAGMA_LIBCALL */

/* Miscellaneous */

#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall IFFParseBase GoodID 102 001
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall IFFParseBase GoodType 108 001
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall IFFParseBase IDtoStr 10e 8002
#endif /* __CLIB_PRAGMA_LIBCALL */

#endif /* PRAGMAS_IFFPARSE_PRAGMAS_H */
