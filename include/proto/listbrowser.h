#ifndef _PROTO_LISTBROWSER_H
#define _PROTO_LISTBROWSER_H

#ifndef EXEC_TYPES_H
#include <exec/types.h>
#endif
#if !defined(CLIB_LISTBROWSER_PROTOS_H) && !defined(__GNUC__)
#include <clib/listbrowser_protos.h>
#endif

#ifndef __NOLIBBASE__
extern struct Library *ListBrowserBase;
#endif

#ifdef __GNUC__
#include <inline/listbrowser.h>
#elif defined(__VBCC__)
#include <inline/listbrowser_protos.h>
#else
#include <pragma/listbrowser_lib.h>
#endif

#endif	/*  _PROTO_LISTBROWSER_H  */
