#ifndef PROTO_SERSCRIPT_H
#define PROTO_SERSCRIPT_H \
       "$Id: serscript.h,v 1.2 1996/02/26 21:28:14 too Exp $"
/*
 *	SAS C prototypes for serscript.library
 *
 *      Copyright © 1995 AmiTCP/IP Group,
 *                       Network Solutions Development, Inc.
 *                       All rights reserved.
 */

#ifndef EXEC_TYPES_H
#include <exec/types.h>
#endif

#ifndef __NOLIBBASE__       
extern struct Library *SerScriptBase;
#endif

#include <libraries/serscript.h>
#include <clib/serscript_protos.h>
#if __SASC
#include <pragmas/serscript_pragmas.h>
#elif __GNUC__
#include <inline/serscript.h>
#endif

#endif /* PROTO_SERSCRIPT_H */
