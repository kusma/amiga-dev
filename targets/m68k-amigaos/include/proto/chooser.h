#ifndef _PROTO_CHOOSER_H
#define _PROTO_CHOOSER_H

#ifndef EXEC_TYPES_H
#include <exec/types.h>
#endif
#if !defined(CLIB_CHOOSER_PROTOS_H) && !defined(__GNUC__)
#include <clib/chooser_protos.h>
#endif

#ifndef __NOLIBBASE__
extern struct Library *ChooserBase;
#endif

#ifdef __GNUC__
#include <inline/chooser.h>
#elif defined(__VBCC__)
#include <inline/chooser_protos.h>
#else
#include <pragma/chooser_lib.h>
#endif

#endif	/*  _PROTO_CHOOSER_H  */
