#ifndef RPC_PMAP_RMT_H
#define RPC_PMAP_RMT_H
/*
 * $Id: pmap_rmt.h,v 4.1 1994/09/26 08:09:11 jraja Exp jraja $
 *
 * Structures and XDR routines for parameters to and replies from
 * the portmapper remote-call-service.
 *
 * Copyright © 1994 AmiTCP/IP Group,
 *                  Network Solutions Development Inc.
 *                  All rights reserved.
 *
 */
/* @(#)pmap_rmt.h	2.1 88/07/29 4.0 RPCSRC; from 1.2 88/02/08 SMI */

/*
 * Copyright (C) 1986, Sun Microsystems, Inc.
 */

struct rmtcallargs {
	u_long prog, vers, proc, arglen;
	caddr_t args_ptr;
	xdrproc_t xdr_args;
};

extern bool_t XDRFUN xdr_rmtcall_args(XDR * xdrs, struct rmtcallargs * cap);

struct rmtcallres {
	u_long *port_ptr;
	u_long resultslen;
	caddr_t results_ptr;
	xdrproc_t xdr_results;
};

extern bool_t XDRFUN xdr_rmtcallres(XDR * xdrs, register struct rmtcallres * crp);

#endif /* !RPC_PMAP_RMT_H */
