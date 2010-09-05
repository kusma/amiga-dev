#ifndef _PROTO_COMMODITIES_H
#define _PROTO_COMMODITIES_H

#ifndef EXEC_TYPES_H
#include <exec/types.h>
#endif
#if !defined(CLIB_COMMODITIES_PROTOS_H) && !defined(__GNUC__)
#include <clib/commodities_protos.h>
#endif

#ifndef __NOLIBBASE__
extern struct Library *CxBase;
#endif

#ifdef __GNUC__
#include <inline/commodities.h>
#elif defined(__VBCC__)
#include <inline/commodities_protos.h>
#else
#include <pragma/commodities_lib.h>
#endif

#endif	/*  _PROTO_COMMODITIES_H  */
