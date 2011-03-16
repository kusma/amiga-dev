#ifndef RPC_RPC_MSG_H
#define RPC_RPC_MSG_H
/*
 * $Id: rpc_msg.h,v 4.1 1994/09/26 08:09:11 jraja Exp jraja $
 *
 * RPC message definition
 *
 * Copyright © 1994 AmiTCP/IP Group,
 *                  Network Solutions Development Inc.
 *                  All rights reserved.
 *
 */
/* @(#)rpc_msg.h	2.1 88/07/29 4.0 RPCSRC */
/*      @(#)rpc_msg.h 1.7 86/07/16 SMI      */

/*
 * Copyright (C) 1984, Sun Microsystems, Inc.
 */

#define RPC_MSG_VERSION		((u_long) 2)
#define RPC_SERVICE_PORT	((u_short) 2048)

/*
 * Bottom up definition of an rpc message.
 * NOTE: call and reply use the same overall stuct but
 * different parts of unions within it.
 */

enum msg_type {
	CALL=0,
	REPLY=1
};

enum reply_stat {
	MSG_ACCEPTED=0,
	MSG_DENIED=1
};

enum accept_stat {
	SUCCESS=0,
	PROG_UNAVAIL=1,
	PROG_MISMATCH=2,
	PROC_UNAVAIL=3,
	GARBAGE_ARGS=4,
	SYSTEM_ERR=5
};

enum reject_stat {
	RPC_MISMATCH=0,
	AUTH_ERROR=1
};

/*
 * Reply part of an rpc exchange
 */

/*
 * Reply to an rpc request that was accepted by the server.
 * Note: there could be an error even though the request was
 * accepted.
 */
struct accepted_reply {
	struct opaque_auth	ar_verf;
	enum accept_stat	ar_stat;
	union {
		struct {
			u_long	low;
			u_long	high;
		} AR_versions;
		struct {
			caddr_t	where;
			xdrproc_t proc;
		} AR_results;
		/* and many other null cases */
	} ru;
#define	ar_results	ru.AR_results
#define	ar_vers		ru.AR_versions
};
extern bool_t XDRFUN xdr_accepted_reply(XDR * xdrs, struct accepted_reply * ar);

/*
 * Reply to an rpc request that was rejected by the server.
 */
struct rejected_reply {
	enum reject_stat rj_stat;
	union {
		struct {
			u_long low;
			u_long high;
		} RJ_versions;
		enum auth_stat RJ_why;  /* why authentication did not work */
	} ru;
#define	rj_vers	ru.RJ_versions
#define	rj_why	ru.RJ_why
};
extern bool_t XDRFUN xdr_rejected_reply(XDR * xdrs, struct rejected_reply * rr);

/*
 * Body of a reply to an rpc request.
 */
struct reply_body {
	enum reply_stat rp_stat;
	union {
		struct accepted_reply RP_ar;
		struct rejected_reply RP_dr;
	} ru;
#define	rp_acpt	ru.RP_ar
#define	rp_rjct	ru.RP_dr
};

/*
 * Body of an rpc request call.
 */
struct call_body {
	u_long cb_rpcvers;	/* must be equal to two */
	u_long cb_prog;
	u_long cb_vers;
	u_long cb_proc;
	struct opaque_auth cb_cred;
	struct opaque_auth cb_verf; /* protocol specific - provided by client */
};

/*
 * The rpc message
 */
struct rpc_msg {
	u_long			rm_xid;
	enum msg_type		rm_direction;
	union {
		struct call_body RM_cmb;
		struct reply_body RM_rmb;
	} ru;
#define	rm_call		ru.RM_cmb
#define	rm_reply	ru.RM_rmb
};
#define	acpted_rply	ru.RM_rmb.ru.RP_ar
#define	rjcted_rply	ru.RM_rmb.ru.RP_dr


/*
 * XDR routine to handle a rpc message.
 */
extern bool_t XDRFUN	xdr_callmsg(XDR * xdrs, struct rpc_msg * cmsg);

/*
 * XDR routine to pre-serialize the static part of a rpc message.
 */
extern bool_t XDRFUN	xdr_callhdr(XDR *xdrs, struct rpc_msg *cmsg);

/*
 * XDR routine to handle a rpc reply.
 */
extern bool_t XDRFUN	xdr_replymsg(XDR *xdrs, struct rpc_msg *rmsg);

/*
 * Fills in the error part of a reply message.
 */
extern void	_seterr_reply(struct rpc_msg *msg, struct rpc_err *error);

#endif /* !RPC_RPC_MSG_H */
