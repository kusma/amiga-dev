#ifndef RPC_SVC_H
#define RPC_SVC_H
/*
 * $Id: svc.h,v 4.1 1994/09/26 08:09:11 jraja Exp jraja $
 *
 * Server-side remote procedure call interface.
 *
 * Copyright © 1994 AmiTCP/IP Group,
 *                  Network Solutions Development Inc.
 *                  All rights reserved.
 *
 */
/* @(#)svc.h	2.2 88/07/29 4.0 RPCSRC; from 1.20 88/02/08 SMI */

/*
 * Copyright (C) 1984, Sun Microsystems, Inc.
 */

/*
 * This interface must manage two items concerning remote procedure calling:
 *
 * 1) An arbitrary number of transport connections upon which rpc requests
 * are received.  The two most notable transports are TCP and UDP;  they are
 * created and registered by routines in svc_tcp.c and svc_udp.c, respectively;
 * they in turn call xprt_register and xprt_unregister.
 *
 * 2) An arbitrary number of locally registered services.  Services are
 * described by the following four data: program number, version number,
 * "service dispatch" function, a transport handle, and a boolean that
 * indicates whether or not the exported program should be registered with a
 * local binder service;  if true the program's number and version and the
 * port number from the transport handle are registered with the binder.
 * These data are registered with the rpc svc system via svc_register.
 *
 * A service's dispatch function is called whenever an rpc request comes in
 * on a transport.  The request's program and version numbers must match
 * those of the registered service.  The dispatch function is passed two
 * parameters, struct svc_req * and SVCXPRT *, defined below.
 */

enum xprt_stat {
	XPRT_DIED,
	XPRT_MOREREQS,
	XPRT_IDLE
};

/*
 * Server side transport handle
 */
typedef struct SVCXPRT {
	int		xp_sock;
	u_short		xp_port;	 /* associated port number */
	struct xp_ops {
	    bool_t	(*xp_recv)(struct SVCXPRT *xprt, struct rpc_msg *msg);	 /* receive incomming requests */
	    enum xprt_stat (*xp_stat)(struct SVCXPRT *xprt); /* get transport status */
	    bool_t	(*xp_getargs)(struct SVCXPRT *xprt, xdrproc_t xargs,
				      void * argsp); /* get arguments */
	    bool_t	(*xp_reply)(struct SVCXPRT *xprt,
				    struct rpc_msg *msg);  /* send reply */
	    bool_t	(*xp_freeargs)(struct SVCXPRT *xprt, xdrproc_t xargs, 
				      void * argsp);/* free mem allocated for args */
	    void	(*xp_destroy)(struct SVCXPRT *xprt); /* destroy this struct */
	} *xp_ops;
	long		xp_addrlen;	 /* length of remote address */
	struct sockaddr_in xp_raddr;	 /* remote address */
	struct opaque_auth xp_verf;	 /* raw response verifier */
	caddr_t		xp_p1;		 /* private */
	caddr_t		xp_p2;		 /* private */
} SVCXPRT;

/*
 *  Approved way of getting address of caller
 */
#define svc_getcaller(x) (&(x)->xp_raddr)

/*
 * Operations defined on an SVCXPRT handle
 *
 * SVCXPRT		*xprt;
 * struct rpc_msg	*msg;
 * xdrproc_t		 xargs;
 * caddr_t		 argsp;
 */
#define SVC_RECV(xprt, msg)				\
	(*(xprt)->xp_ops->xp_recv)((xprt), (msg))
#define svc_recv(xprt, msg)				\
	(*(xprt)->xp_ops->xp_recv)((xprt), (msg))

#define SVC_STAT(xprt)					\
	(*(xprt)->xp_ops->xp_stat)(xprt)
#define svc_stat(xprt)					\
	(*(xprt)->xp_ops->xp_stat)(xprt)

#define SVC_GETARGS(xprt, xargs, argsp)			\
	(*(xprt)->xp_ops->xp_getargs)((xprt), (xargs), (argsp))
#define svc_getargs(xprt, xargs, argsp)			\
	(*(xprt)->xp_ops->xp_getargs)((xprt), (xargs), (argsp))

#define SVC_REPLY(xprt, msg)				\
	(*(xprt)->xp_ops->xp_reply) ((xprt), (msg))
#define svc_reply(xprt, msg)				\
	(*(xprt)->xp_ops->xp_reply) ((xprt), (msg))

#define SVC_FREEARGS(xprt, xargs, argsp)		\
	(*(xprt)->xp_ops->xp_freeargs)((xprt), (xargs), (argsp))
#define svc_freeargs(xprt, xargs, argsp)		\
	(*(xprt)->xp_ops->xp_freeargs)((xprt), (xargs), (argsp))

#define SVC_DESTROY(xprt)				\
	(*(xprt)->xp_ops->xp_destroy)(xprt)
#define svc_destroy(xprt)				\
	(*(xprt)->xp_ops->xp_destroy)(xprt)


/*
 * Service request
 */
struct svc_req {
	u_long		rq_prog;	/* service program number */
	u_long		rq_vers;	/* service protocol version */
	u_long		rq_proc;	/* the desired procedure */
	struct opaque_auth rq_cred;	/* raw creds from the wire */
	caddr_t		rq_clntcred;	/* read only cooked cred */
	SVCXPRT	*rq_xprt;		/* associated transport */
};


/*
 * Service registration
 *
 *  protocol is like TCP or UDP, zero means do not register 
 */
extern bool_t	svc_register(SVCXPRT * xprt, u_long prog, u_long vers,
			     void (* dispatch)(struct svc_req *, SVCXPRT *),
			     int protocol);

/*
 * Service un-registration
 */
extern void	svc_unregister(u_long prog, u_long vers);

/*
 * Transport registration.
 */
extern void	xprt_register(SVCXPRT * xprt);

/*
 * Transport un-register
 */
extern void	xprt_unregister(SVCXPRT * xprt);




/*
 * When the service routine is called, it must first check to see if it
 * knows about the procedure;  if not, it should call svcerr_noproc
 * and return.  If so, it should deserialize its arguments via 
 * SVC_GETARGS (defined above).  If the deserialization does not work,
 * svcerr_decode should be called followed by a return.  Successful
 * decoding of the arguments should be followed the execution of the
 * procedure's code and a call to svc_sendreply.
 *
 * Also, if the service refuses to execute the procedure due to too-
 * weak authentication parameters, svcerr_weakauth should be called.
 * Note: do not confuse access-control failure with weak authentication!
 *
 * NB: In pure implementations of rpc, the caller always waits for a reply
 * msg.  This message is sent when svc_sendreply is called.  
 * Therefore pure service implementations should always call
 * svc_sendreply even if the function logically returns void;  use
 * xdr.h - xdr_void for the xdr routine.  HOWEVER, tcp based rpc allows
 * for the abuse of pure rpc via batched calling or pipelining.  In the
 * case of a batched call, svc_sendreply should NOT be called since
 * this would send a return message, which is what batching tries to avoid.
 * It is the service/protocol writer's responsibility to know which calls are
 * batched and which are not.  Warning: responding to batch calls may
 * deadlock the caller and server processes!
 */

extern bool_t	svc_sendreply(SVCXPRT * xprt, xdrproc_t xdr_results,
			      caddr_t xdr_location);
extern void	svcerr_decode(SVCXPRT * xprt);
extern void	svcerr_weakauth(SVCXPRT * xprt);
extern void	svcerr_noproc(SVCXPRT * xprt);
extern void	svcerr_progvers(SVCXPRT * xprt, 
				u_long low_vers, u_long high_vers);
extern void	svcerr_auth(SVCXPRT * xprt, enum auth_stat why);
extern void	svcerr_noprog(SVCXPRT * xprt);
extern void	svcerr_systemerr(SVCXPRT * xprt);
    
/*
 * Lowest level dispatching -OR- who owns this process anyway.
 * Somebody has to wait for incoming requests and then call the correct
 * service routine.  The routine svc_run does infinite waiting; i.e.,
 * svc_run never returns.
 * Since another (co-existant) package may wish to selectively wait for
 * incoming calls or other events outside of the rpc architecture, the
 * routine svc_getreq is provided.  It must be passed readfds, the
 * "in-place" results of a select system call (see select, section 2).
 */

/*
 * Global keeper of rpc service descriptors in use
 * dynamic; must be inspected before each call to select 
 */
#ifdef FD_SETSIZE
extern fd_set svc_fdset;
#define svc_fds svc_fdset.fds_bits[0]	/* compatibility */
#else
extern int svc_fds;
#endif /* def FD_SETSIZE */

/*
 * a small program implemented by the svc_rpc implementation itself;
 * also see clnt.h for protocol numbers.
 */
extern void rpctest_service();

extern void	svc_getreq(int rdfds);
extern void	svc_getreqset(fd_set * readfds); /* takes fdset instead of int */
extern void	svc_run(void); 	 /* never returns */

/*
 * Socket to use on svcxxx_create call to get default socket
 */
#define	RPC_ANYSOCK	-1

/*
 * These are the existing service side transport implementations
 */

/*
 * Memory based rpc for testing and timing.
 */
extern SVCXPRT *svcraw_create(void);

/*
 * Udp based rpc.
 */
extern SVCXPRT *svcudp_create(int sock);
extern SVCXPRT *svcudp_bufcreate(int sock, u_int sendsz, u_int recvsz);

/*
 * Tcp based rpc.
 */
extern SVCXPRT *svctcp_create(int sock, u_int sendsize, u_int recvsize);

#endif /* !RPC_SVC_H */
