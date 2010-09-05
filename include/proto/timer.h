#ifndef _PROTO_TIMER_H
#define _PROTO_TIMER_H

#ifndef EXEC_TYPES_H
#include <exec/types.h>
#endif
#if !defined(CLIB_TIMER_PROTOS_H) && !defined(__GNUC__)
#include <clib/timer_protos.h>
#endif

#ifndef __NOLIBBASE__
extern struct Library *TimerBase;
#endif

#ifdef __GNUC__
#include <inline/timer.h>
#elif defined(__VBCC__)
#include <inline/timer_protos.h>
#else
#include <pragma/timer_lib.h>
#endif

#endif	/*  _PROTO_TIMER_H  */
