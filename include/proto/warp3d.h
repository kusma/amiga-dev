#ifndef _PROTO_WARP3D_H
#define _PROTO_WARP3D_H

#ifndef EXEC_TYPES_H
#include <exec/types.h>
#endif
#if !defined(CLIB_WARP3D_PROTOS_H) && !defined(__GNUC__)
#include <clib/warp3d_protos.h>
#endif

#ifndef __NOLIBBASE__
extern struct Library *Warp3DBase;
#endif

#ifdef __GNUC__
#include <inline/warp3d.h>
#elif defined(__VBCC__)
#include <inline/warp3d_protos.h>
#else
#include <pragma/warp3d_lib.h>
#endif

#endif	/*  _PROTO_WARP3D_H  */
