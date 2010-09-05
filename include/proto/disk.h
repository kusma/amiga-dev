#ifndef _PROTO_DISK_H
#define _PROTO_DISK_H

#ifndef EXEC_TYPES_H
#include <exec/types.h>
#endif
#if !defined(CLIB_DISK_PROTOS_H) && !defined(__GNUC__)
#include <clib/disk_protos.h>
#endif

#ifndef __NOLIBBASE__
extern struct DiskResource *DiskBase;
#endif

#ifdef __GNUC__
#include <inline/disk.h>
#elif defined(__VBCC__)
#include <inline/disk_protos.h>
#else
#include <pragma/disk_lib.h>
#endif

#endif	/*  _PROTO_DISK_H  */
