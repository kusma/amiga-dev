#ifndef RPC_AUTH_UNIX_H
#define RPC_AUTH_UNIX_H
/*
 * $Id: auth_unix.h,v 4.1 1994/09/26 08:09:11 jraja Exp jraja $
 *
 * Protocol for UNIX style authentication parameters for RPC
 *
 * Copyright © 1994 AmiTCP/IP Group,
 *                  Network Solutions Development Inc.
 *                  All rights reserved.
 *
 */
/* @(#)auth_unix.h	2.2 88/07/29 4.0 RPCSRC; from 1.8 88/02/08 SMI */
/*      @(#)auth_unix.h 1.5 86/07/16 SMI      */

/*
 * Copyright (C) 1984, Sun Microsystems, Inc.
 */

/*
 * The system is very weak.  The client uses no encryption for  it
 * credentials and only sends null verifiers.  The server sends backs
 * null verifiers or optionally a verifier that suggests a new short hand
 * for the credentials.
 */

/* The machine name is part of a credential; it may not exceed 255 bytes */
#define MAX_MACHINE_NAME 255

/* gids compose part of a credential; there may not be more than 16 of them */
#define NGRPS 16

/*
 * Unix style credentials.
 */
struct authunix_parms {
	u_long	 aup_time;
	char	*aup_machname;
	uid_t	 aup_uid;
	gid_t	 aup_gid;
	u_int	 aup_len;
	gid_t	*aup_gids;
};

extern bool_t XDRFUN xdr_authunix_parms(XDR * xdrs, struct authunix_parms * p);

/* 
 * If a response verifier has flavor AUTH_SHORT, 
 * then the body of the response verifier encapsulates the following structure;
 * again it is serialized in the obvious fashion.
 */
struct short_hand_verf {
	struct opaque_auth new_cred;
};

#endif /* !RPC_AUTH_UNIX_H */
