#ifndef RPC_CLNT_H
#define RPC_CLNT_H
/*
 * $Id: clnt.h,v 4.1 1994/09/26 08:09:11 jraja Exp jraja $
 *
 * Client side remote procedure call interface.
 *
 * Copyright © 1994 AmiTCP/IP Group,
 *                  Network Solutions Development Inc.
 *                  All rights reserved.
 *
 */
/* @(#)clnt.h	2.1 88/07/29 4.0 RPCSRC; from 1.31 88/02/08 SMI*/

/*
 * Copyright (C) 1984, Sun Microsystems, Inc.
 */

/*
 * Rpc calls return an enum clnt_stat.  This should be looked at more,
 * since each implementation is required to live with this (implementation
 * independent) list of errors.
 */
enum clnt_stat {
	RPC_SUCCESS=0,			/* call succeeded */
	/*
	 * local errors
	 */
	RPC_CANTENCODEARGS=1,		/* can't encode arguments */
	RPC_CANTDECODERES=2,		/* can't decode results */
	RPC_CANTSEND=3,			/* failure in sending call */
	RPC_CANTRECV=4,			/* failure in receiving result */
	RPC_TIMEDOUT=5,			/* call timed out */
	/*
	 * remote errors
	 */
	RPC_VERSMISMATCH=6,		/* rpc versions not compatible */
	RPC_AUTHERROR=7,		/* authentication error */
	RPC_PROGUNAVAIL=8,		/* program not available */
	RPC_PROGVERSMISMATCH=9,		/* program version mismatched */
	RPC_PROCUNAVAIL=10,		/* procedure unavailable */
	RPC_CANTDECODEARGS=11,		/* decode arguments error */
	RPC_SYSTEMERROR=12,		/* generic "other problem" */

	/*
	 * callrpc & clnt_create errors
	 */
	RPC_UNKNOWNHOST=13,		/* unknown host name */
	RPC_UNKNOWNPROTO=17,		/* unkown protocol */

	/*
	 * _ create errors
	 */
	RPC_PMAPFAILURE=14,		/* the pmapper failed in its call */
	RPC_PROGNOTREGISTERED=15,	/* remote program is not registered */
	/*
	 * unspecified error
	 */
	RPC_FAILED=16
};


/*
 * Error info.
 */
struct rpc_err {
	enum clnt_stat re_status;
	union {
		int RE_errno;		/* realated system error */
		enum auth_stat RE_why;	/* why the auth error occurred */
		struct {
			u_long low;	/* lowest verion supported */
			u_long high;	/* highest verion supported */
		} RE_vers;
		struct {		/* maybe meaningful if RPC_FAILED */
			long s1;
			long s2;
		} RE_lb;		/* life boot & debugging only */
	} ru;
#define	re_errno	ru.RE_errno
#define	re_why		ru.RE_why
#define	re_vers		ru.RE_vers
#define	re_lb		ru.RE_lb
};


/*
 * Client rpc handle.
 * Created by individual implementations, see e.g. rpc_udp.c.
 * Client is responsible for initializing auth, see e.g. auth_none.c.
 */
typedef struct CLIENT {
	AUTH	*cl_auth;			/* authenticator */
	struct clnt_ops {
		enum clnt_stat	(*cl_call)(struct CLIENT *rh, u_long proc, 
					   xdrproc_t xargs, void * argsp, 
					   xdrproc_t xres, void * resp, 
					   struct timeval timeout);	/* call remote procedure */
		void		(*cl_abort)(struct CLIENT *rh);	/* abort a call */
		void		(*cl_geterr)(struct CLIENT *rh, 
					     struct rpc_err *errp); /* get specific error code */
		bool_t		(*cl_freeres)(struct CLIENT *rh, 
					      xdrproc_t xres, void * resp); /* frees results */
		void		(*cl_destroy)(struct CLIENT *rh); /* destroy this structure */
		bool_t          (*cl_control)(struct CLIENT *rh, u_int request,
					      void * info); /* the ioctl() of rpc */
	} *cl_ops;
	caddr_t			cl_private;	/* private stuff */
} CLIENT;


/*
 * client side rpc interface ops
 *
 * Parameter types are:
 *
 */

/*
 * enum clnt_stat
 * CLNT_CALL(rh, proc, xargs, argsp, xres, resp, timeout)
 * 	CLIENT *rh;
 *	u_long proc;
 *	xdrproc_t xargs;
 *	void * argsp;
 *	xdrproc_t xres;
 *	void * resp;
 *	struct timeval timeout;
 */
#define	CLNT_CALL(rh, proc, xargs, argsp, xres, resp, secs)	\
	((*(rh)->cl_ops->cl_call)(rh, proc, xargs, argsp, xres, resp, secs))
#define	clnt_call(rh, proc, xargs, argsp, xres, resp, secs)	\
	((*(rh)->cl_ops->cl_call)(rh, proc, xargs, argsp, xres, resp, secs))

/*
 * void
 * CLNT_ABORT(rh);
 * 	CLIENT *rh;
 */
#define	CLNT_ABORT(rh)	((*(rh)->cl_ops->cl_abort)(rh))
#define	clnt_abort(rh)	((*(rh)->cl_ops->cl_abort)(rh))

/*
 * void
 * CLNT_GETERR(rh, errp);
 * 	CLIENT *rh;
 *      struct rpc_err *errp;
 */
#define	CLNT_GETERR(rh,errp)	((*(rh)->cl_ops->cl_geterr)(rh, errp))
#define	clnt_geterr(rh,errp)	((*(rh)->cl_ops->cl_geterr)(rh, errp))


/*
 * bool_t
 * CLNT_FREERES(rh, xres, resp);
 * 	CLIENT *rh;
 *	xdrproc_t xres;
 *	void * resp;
 */
#define	CLNT_FREERES(rh,xres,resp) ((*(rh)->cl_ops->cl_freeres)(rh,xres,resp))
#define	clnt_freeres(rh,xres,resp) ((*(rh)->cl_ops->cl_freeres)(rh,xres,resp))

/*
 * bool_t
 * CLNT_CONTROL(cl, request, info)
 *      CLIENT *cl;
 *      u_int request;
 *      void *info;
 */
#define	CLNT_CONTROL(cl,rq,in) ((*(cl)->cl_ops->cl_control)(cl,rq,in))
#define	clnt_control(cl,rq,in) ((*(cl)->cl_ops->cl_control)(cl,rq,in))

/*
 * control operations that apply to both udp and tcp transports
 */
#define CLSET_TIMEOUT       1   /* set timeout (timeval) */
#define CLGET_TIMEOUT       2   /* get timeout (timeval) */
#define CLGET_SERVER_ADDR   3   /* get server's address (sockaddr) */
/*
 * udp only control operations
 */
#define CLSET_RETRY_TIMEOUT 4   /* set retry timeout (timeval) */
#define CLGET_RETRY_TIMEOUT 5   /* get retry timeout (timeval) */

/*
 * void
 * CLNT_DESTROY(rh);
 * 	CLIENT *rh;
 */
#define	CLNT_DESTROY(rh)	((*(rh)->cl_ops->cl_destroy)(rh))
#define	clnt_destroy(rh)	((*(rh)->cl_ops->cl_destroy)(rh))


/*
 * RPCTEST is a test program which is accessable on every rpc
 * transport/port.  It is used for testing, performance evaluation,
 * and network administration.
 */

#define RPCTEST_PROGRAM		(1UL)
#define RPCTEST_VERSION		(1UL)
#define RPCTEST_NULL_PROC	(2UL)
#define RPCTEST_NULL_BATCH_PROC	(3UL)

/*
 * By convention, procedure 0 takes null arguments and returns them
 */

#define NULLPROC (0UL)

/*
 * Below are the client handle creation routines for the various
 * implementations of client side rpc.  They can return NULL if a 
 * creation failure occurs.
 */

/*
 * Memory based rpc (for speed check and testing)
 */
extern CLIENT *clntraw_create(u_long prog, u_long vers);


/*
 * Generic client creation routine. Supported protocols are "udp" and "tcp"
 */
extern CLIENT *
clnt_create(char *host, u_long prog, u_long vers, char *prot);


/*
 * TCP based rpc
 */
extern CLIENT *clnttcp_create(struct sockaddr_in *raddr, u_long prog, 
			      u_long version, int *sockp,
			      u_int sendsz, u_int recvsz);

/*
 * UDP based rpc.
 */
extern CLIENT *clntudp_create(struct sockaddr_in *raddr, u_long program,
			      u_long version, struct timeval wait, int *sockp);
/*
 * Same as above, but you specify max packet sizes.
 */
extern CLIENT *clntudp_bufcreate(struct sockaddr_in *raddr, u_long program,
				 u_long version, struct timeval wait, 
				 int *sockp, u_int sendsz, u_int recvsz);

/*
 * Print why creation failed
 */
extern void clnt_pcreateerror(char *msg);	/* stderr */
extern char *clnt_spcreateerror(char *msg);	/* string */

/*
 * Like clnt_perror(), but is more verbose in its output
 */ 
extern void clnt_perrno(enum clnt_stat num);	/* stderr */

/*
 * Print an English error message, given the client error code
 */
extern void clnt_perror(CLIENT *clnt, char *msg); 	/* stderr */
extern char *clnt_sperror(CLIENT *clnt, char *msg);	/* string */

/* 
 * If a creation fails, the following allows the user to figure out why.
 */
struct rpc_createerr {
	enum clnt_stat cf_stat;
	struct rpc_err cf_error; /* useful when cf_stat == RPC_PMAPFAILURE */
};

extern struct rpc_createerr rpc_createerr;



/*
 * Copy error message to buffer.
 */
extern char *clnt_sperrno(enum clnt_stat num);	/* string */



#define UDPMSGSIZE	8800	/* rpc imposed limit on udp msg size */
#define RPCSMALLMSGSIZE	400	/* a more reasonable packet size */

/*
 * simple rpc
 */
int callrpc(const char * host,
	    u_long prognum, u_long versnum, u_long procnum,
	    xdrproc_t inproc, void * in, xdrproc_t outproc, void * out);

#endif /* !RPC_CLNT_H */
