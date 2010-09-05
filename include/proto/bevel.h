#ifndef _PROTO_BEVEL_H
#define _PROTO_BEVEL_H

#ifndef EXEC_TYPES_H
#include <exec/types.h>
#endif
#if !defined(CLIB_BEVEL_PROTOS_H) && !defined(__GNUC__)
#include <clib/bevel_protos.h>
#endif

#ifndef __NOLIBBASE__
extern struct Library *BevelBase;
#endif

#ifdef __GNUC__
#include <inline/bevel.h>
#elif defined(__VBCC__)
#include <inline/bevel_protos.h>
#else
#include <pragma/bevel_lib.h>
#endif

#endif	/*  _PROTO_BEVEL_H  */
