#ifndef DEVICES_NETINFO_H
#define DEVICES_NETINFO_H \
       "$Id$"
/*
 *	AmiTCP/IP netinfo.device interface for 32 bit C compilers
 *
 *      Copyright © 1994 AmiTCP/IP Group,
 *                       Network Solutions Development, Inc.
 *                       All rights reserved.
 */

#ifndef EXEC_TYPES_H
#include <exec/types.h>
#endif
#ifndef EXEC_IO_H
#include <exec/io.h>
#endif

#define NETINFONAME "AmiTCP:devs/netinfo.device"

/*
 * Modified IOStdReq
 */
struct NetInfoReq {
    struct  Message  io_Message;
    struct  Device  *io_Device;     /* device node pointer  */
    struct  Unit    *io_Unit;	    /* unit (driver private) */
    UWORD   io_Command;		    /* device command */
    UBYTE   io_Flags;
    BYTE    io_Error;		    /* error or warning num */
    ULONG   io_Actual;		    /* actual number of bytes transferred */
    ULONG   io_Length;		    /* requested number bytes transferred*/
    APTR    io_Data;		    /* points to data area */
    ULONG   io_Offset;		    /* search criteria */
};

/*
 * NetInfo units
 */
#define NETINFO_PASSWD_UNIT    0L
#define NETINFO_GROUP_UNIT     1L
#define NETINFO_UNITS          2L

/* 
 * Non-standard commands
 */
#define NI_GETBYID    (CMD_NONSTD+0)
#define NI_GETBYNAME  (CMD_NONSTD+1)
#define NI_MEMBERS    (CMD_NONSTD+2)
#define NI_END        (CMD_NONSTD+3)

/*
 * Non-standard error codes (same as in the BSD errno)
 */
#define NIERR_NOTFOUND      2
#define NIERR_TOOSMALL      7
#define NIERR_NOMEM         12
#define NIERR_ACCESS        13
#define NIERR_NULL_POINTER  14
#define NIERR_INVAL         22

/* The passwd structure */
struct NetInfoPasswd
{
  UBYTE *pw_name;               /* Username */
  UBYTE *pw_passwd;             /* Encrypted password */
  LONG   pw_uid;                /* User ID */
  LONG   pw_gid;                /* Group ID */
  UBYTE *pw_gecos;              /* Real name etc */
  UBYTE *pw_dir;                /* Home directory */
  UBYTE *pw_shell;              /* Shell */
};

/* The group structure */
struct NetInfoGroup
{
  UBYTE  *gr_name;              /* Group name.  */
  UBYTE  *gr_passwd;            /* Password.    */
  LONG    gr_gid;		/* Group ID.    */
  UBYTE **gr_mem;               /* Member list. */
};

#endif /* DEVICES_NETINFO_H */

