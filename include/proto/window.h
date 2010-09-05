#ifndef _PROTO_WINDOW_H
#define _PROTO_WINDOW_H

#ifndef EXEC_TYPES_H
#include <exec/types.h>
#endif
#if !defined(CLIB_WINDOW_PROTOS_H) && !defined(__GNUC__)
#include <clib/window_protos.h>
#endif

#ifndef __NOLIBBASE__
extern struct Library *WindowBase;
#endif

#ifdef __GNUC__
#include <inline/window.h>
#elif defined(__VBCC__)
#include <inline/window_protos.h>
#else
#include <pragma/window_lib.h>
#endif

#endif	/*  _PROTO_WINDOW_H  */
