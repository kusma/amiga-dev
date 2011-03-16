#ifndef _PROTO_TEXTFIELD_H
#define _PROTO_TEXTFIELD_H

#ifndef EXEC_TYPES_H
#include <exec/types.h>
#endif
#if !defined(CLIB_TEXTFIELD_PROTOS_H) && !defined(__GNUC__)
#include <clib/textfield_protos.h>
#endif

#ifndef __NOLIBBASE__
extern struct Library *TextFieldBase;
#endif

#ifdef __GNUC__
#include <inline/textfield.h>
#elif defined(__VBCC__)
#include <inline/textfield_protos.h>
#else
#include <pragma/textfield_lib.h>
#endif

#endif	/*  _PROTO_TEXTFIELD_H  */
