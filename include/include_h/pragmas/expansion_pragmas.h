#ifndef PRAGMAS_EXPANSION_PRAGMAS_H
#define PRAGMAS_EXPANSION_PRAGMAS_H

/*
**	$VER: expansion_pragmas.h 40.2 (6.6.1998)
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

#ifndef CLIB_EXPANSION_PROTOS_H
#include <clib/expansion_protos.h>
#endif /* CLIB_EXPANSION_PROTOS_H */

/*--- functions in V33 or higher (Release 1.2) ---*/
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall ExpansionBase AddConfigDev 1e 801
#endif /* __CLIB_PRAGMA_LIBCALL */
/*--- functions in V36 or higher (Release 2.0) ---*/
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall ExpansionBase AddBootNode 24 981004
#endif /* __CLIB_PRAGMA_LIBCALL */
/*--- functions in V33 or higher (Release 1.2) ---*/
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall ExpansionBase AllocBoardMem 2a 001
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall ExpansionBase AllocConfigDev 30 00
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall ExpansionBase AllocExpansionMem 36 1002
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall ExpansionBase ConfigBoard 3c 9802
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall ExpansionBase ConfigChain 42 801
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall ExpansionBase FindConfigDev 48 10803
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall ExpansionBase FreeBoardMem 4e 1002
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall ExpansionBase FreeConfigDev 54 801
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall ExpansionBase FreeExpansionMem 5a 1002
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall ExpansionBase ReadExpansionByte 60 0802
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall ExpansionBase ReadExpansionRom 66 9802
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall ExpansionBase RemConfigDev 6c 801
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall ExpansionBase WriteExpansionByte 72 10803
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall ExpansionBase ObtainConfigBinding 78 00
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall ExpansionBase ReleaseConfigBinding 7e 00
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall ExpansionBase SetCurrentBinding 84 0802
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall ExpansionBase GetCurrentBinding 8a 0802
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall ExpansionBase MakeDosNode 90 801
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall ExpansionBase AddDosNode 96 81003
#endif /* __CLIB_PRAGMA_LIBCALL */
/*--- functions in V36 or higher (Release 2.0) ---*/

#endif /* PRAGMAS_EXPANSION_PRAGMAS_H */
