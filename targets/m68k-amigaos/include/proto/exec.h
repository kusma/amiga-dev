#ifndef _PROTO_EXEC_H
#define _PROTO_EXEC_H

#ifndef EXEC_TYPES_H
#include <exec/types.h>
#endif
#if !defined(CLIB_EXEC_PROTOS_H) && !defined(__GNUC__)
#include <clib/exec_protos.h>
#endif

#ifndef __NOLIBBASE__
extern struct ExecBase *SysBase;
#endif

#ifdef __GNUC__
#include <inline/exec.h>
#elif defined(__VBCC__)
#include <inline/exec_protos.h>
#else
#include <pragma/exec_lib.h>
#endif

#endif	/*  _PROTO_EXEC_H  */
