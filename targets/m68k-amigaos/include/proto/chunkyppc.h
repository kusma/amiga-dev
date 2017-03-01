#ifndef _PROTO_CHUNKYPPC_H
#define _PROTO_CHUNKYPPC_H

#ifndef EXEC_TYPES_H
#include <exec/types.h>
#endif
#if !defined(CLIB_CHUNKYPPC_PROTOS_H) && !defined(__GNUC__)
#include <clib/chunkyppc_protos.h>
#endif

#ifndef __NOLIBBASE__
extern struct Library *ChunkyPPCBase;
#endif

#ifdef __GNUC__
#include <inline/chunkyppc.h>
#elif defined(__VBCC__)
#include <inline/chunkyppc_protos.h>
#else
#include <pragma/chunkyppc_lib.h>
#endif

#endif	/*  _PROTO_CHUNKYPPC_H  */
