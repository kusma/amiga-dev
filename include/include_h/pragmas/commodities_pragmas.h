#ifndef PRAGMAS_COMMODITIES_PRAGMAS_H
#define PRAGMAS_COMMODITIES_PRAGMAS_H

/*
**	$VER: commodities_pragmas.h 40.1 (17.5.1996)
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

#ifndef CLIB_COMMODITIES_PROTOS_H
#include <clib/commodities_protos.h>
#endif /* CLIB_COMMODITIES_PROTOS_H */

/*--- functions in V36 or higher (Release 2.0) ---*/

/*  OBJECT UTILITIES */

#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall CxBase CreateCxObj 1e 98003
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall CxBase CxBroker 24 0802
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall CxBase ActivateCxObj 2a 0802
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall CxBase DeleteCxObj 30 801
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall CxBase DeleteCxObjAll 36 801
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall CxBase CxObjType 3c 801
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall CxBase CxObjError 42 801
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall CxBase ClearCxObjError 48 801
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall CxBase SetCxObjPri 4e 0802
#endif /* __CLIB_PRAGMA_LIBCALL */

/*  OBJECT ATTACHMENT */

#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall CxBase AttachCxObj 54 9802
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall CxBase EnqueueCxObj 5a 9802
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall CxBase InsertCxObj 60 A9803
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall CxBase RemoveCxObj 66 801
#endif /* __CLIB_PRAGMA_LIBCALL */

/*  TYPE SPECIFIC */

#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall CxBase SetTranslate 72 9802
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall CxBase SetFilter 78 9802
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall CxBase SetFilterIX 7e 9802
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall CxBase ParseIX 84 9802
#endif /* __CLIB_PRAGMA_LIBCALL */

/*  COMMON MESSAGE */

#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall CxBase CxMsgType 8a 801
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall CxBase CxMsgData 90 801
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall CxBase CxMsgID 96 801
#endif /* __CLIB_PRAGMA_LIBCALL */

/*  MESSAGE ROUTING */

#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall CxBase DivertCxMsg 9c A9803
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall CxBase RouteCxMsg a2 9802
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall CxBase DisposeCxMsg a8 801
#endif /* __CLIB_PRAGMA_LIBCALL */

/*  INPUT EVENT HANDLING */

#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall CxBase InvertKeyMap ae 98003
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall CxBase AddIEvents b4 801
#endif /* __CLIB_PRAGMA_LIBCALL */
/*--- functions in V38 or higher (Release 2.1) ---*/
/*  MORE INPUT EVENT HANDLING */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall CxBase MatchIX cc 9802
#endif /* __CLIB_PRAGMA_LIBCALL */

#endif /* PRAGMAS_COMMODITIES_PRAGMAS_H */
