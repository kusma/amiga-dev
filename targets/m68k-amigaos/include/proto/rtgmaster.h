#ifndef _PROTO_RTGMASTER_H
#define _PROTO_RTGMASTER_H

#ifndef EXEC_TYPES_H
#include <exec/types.h>
#endif
#if !defined(CLIB_RTGMASTER_PROTOS_H) && !defined(__GNUC__)
#include <clib/rtgmaster_protos.h>
#endif

#ifndef __NOLIBBASE__
extern struct Library *RTGMasterBase;
#endif

#ifdef __GNUC__
#include <inline/rtgmaster.h>
#elif defined(__VBCC__)
#include <inline/rtgmaster_protos.h>
#else
#include <pragma/rtgmaster_lib.h>
#endif

#endif	/*  _PROTO_RTGMASTER_H  */
