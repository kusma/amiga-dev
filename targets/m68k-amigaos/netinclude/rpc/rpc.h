#ifndef RPC_RPC_H
#define RPC_RPC_H
/*
 * $Id: rpc.h,v 4.1 1994/09/26 08:09:11 jraja Exp jraja $
 *
 * Include the billions of rpc header files necessary to
 * do remote procedure calling.
 *
 * Copyright © 1994 AmiTCP/IP Group,
 *                  Network Solutions Development Inc.
 *                  All rights reserved.
 *
 */
/* @(#)rpc.h	2.4 89/07/11 4.0 RPCSRC; from 1.9 88/02/08 SMI */

/*
 * Copyright (C) 1984, Sun Microsystems, Inc.
 */

#include <rpc/types.h>		/* some typedefs */
#include <netinet/in.h>

/* external data representation interfaces */
#include <rpc/xdr.h>		/* generic (de)serializer */

/* Client side only authentication */
#include <rpc/auth.h>		/* generic authenticator (client side) */

/* Client side (mostly) remote procedure call */
#include <rpc/clnt.h>		/* generic rpc stuff */

/* semi-private protocol headers */
#include <rpc/rpc_msg.h>	/* protocol for rpc messages */
#include <rpc/auth_unix.h>	/* protocol for unix style cred */
/*
 *  Uncomment-out the next line if you are building the rpc library with    
 *  DES Authentication (see the README file in the secure_rpc/ directory).
 */
/*#include <rpc/auth_des.h>	/* protocol for des style cred */

/* Server side only remote procedure callee */
#include <rpc/svc.h>		/* service manager and multiplexer */
#include <rpc/svc_auth.h>	/* service side authenticator */

/*
 * COMMENT OUT THE NEXT INCLUDE (or add to the #ifndef) IF RUNNING ON
 * A VERSION OF UNIX THAT USES SUN'S NFS SOURCE.  These systems will
 * already have the structures defined by <rpc/netdb.h> included in <netdb.h>.
 */
/* routines for parsing /etc/rpc */

struct rpcent {
      char    *r_name;        /* name of server for this rpc program */
      char    **r_aliases;    /* alias list */
      int     r_number;       /* rpc program number */
};

struct rpcent *getrpcbyname(char * name),
  *getrpcbynumber(int number)
#if 0 /* getrpcent() not supported in AmiTCP/IP version */
  ,*getrpcent(void)
#endif
;

#endif /* !RPC_RPC_H */
