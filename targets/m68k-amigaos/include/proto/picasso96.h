#ifndef _PROTO_PICASSO96_H
#define _PROTO_PICASSO96_H

#ifndef EXEC_TYPES_H
#include <exec/types.h>
#endif
#if !defined(CLIB_PICASSO96_PROTOS_H) && !defined(__GNUC__)
#include <clib/picasso96_protos.h>
#endif

#ifndef __NOLIBBASE__
extern struct Library *P96Base;
#endif

#ifdef __GNUC__
#ifdef __AROS__
#include <defines/picasso96.h>
#else
#include <inline/picasso96.h>
#endif
#elif defined(__VBCC__)
#include <inline/picasso96_protos.h>
#else
#include <pragma/picasso96_lib.h>
#endif

#endif	/*  _PROTO_PICASSO96_H  */
