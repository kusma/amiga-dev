#ifndef RPC_PMAP_CLNT_H
#define RPC_PMAP_CLNT_H
/*
 * $Id: pmap_clnt.h,v 4.1 1994/09/26 08:09:11 jraja Exp jraja $
 *
 * Supplies C routines to get to portmap services.
 *
 * Copyright © 1994 AmiTCP/IP Group,
 *                  Network Solutions Development Inc.
 *                  All rights reserved.
 *
 */
/* @(#)pmap_clnt.h	2.1 88/07/29 4.0 RPCSRC; from 1.11 88/02/08 SMI */

/*
 * Copyright (C) 1984, Sun Microsystems, Inc.
 */

/*
 * Usage:
 *	success = pmap_set(program, version, protocol, port);
 *	success = pmap_unset(program, version);
 *	port = pmap_getport(address, program, version, protocol);
 *	head = pmap_getmaps(address);
 *	clnt_stat = pmap_rmtcall(address, program, version, procedure,
 *		xdrargs, argsp, xdrres, resp, tout, port_ptr)
 *		(works for udp only.) 
 * 	clnt_stat = clnt_broadcast(program, version, procedure,
 *		xdrargs, argsp,	xdrres, resp, eachresult)
 *		(like pmap_rmtcall, except the call is broadcasted to all
 *		locally connected nets.  For each valid response received,
 *		the procedure eachresult is called.  Its form is:
 *	done = eachresult(resp, raddr)
 *		bool_t done;
 *		caddr_t resp;
 *		struct sockaddr_in *raddrp;
 *		where resp points to the results of the call and raddr is the
 *		address if the responder to the broadcast.
 */

typedef bool_t (*resultproc_t)(caddr_t resp, struct sockaddr_in *raddrp);

extern bool_t		pmap_set(u_long program, u_long version,
				 int protocol, u_short port);
extern bool_t		pmap_unset(u_long program, u_long version);
extern struct pmaplist *pmap_getmaps(struct sockaddr_in * address);
extern enum clnt_stat	pmap_rmtcall(struct sockaddr_in * addr,
				     u_long prog, u_long vers, u_long proc,
				     xdrproc_t xdrargs, caddr_t argsp,
				     xdrproc_t xdrres, caddr_t resp,
				     struct compatible_timeval tout,
				     u_long * port_ptr);
extern enum clnt_stat	clnt_broadcast(u_long prog, u_long vers, u_long proc,
				       xdrproc_t xargs, caddr_t argsp,
				       xdrproc_t xresults, caddr_t resultsp,
				       resultproc_t eachresult);
extern u_short		pmap_getport(struct sockaddr_in * address,
				     u_long program, u_long version,
				     u_int protocol);

#endif /* !RPC_PMAP_CLNT_H */
