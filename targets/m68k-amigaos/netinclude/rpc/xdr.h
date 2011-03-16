#ifndef RPC_XDR_H
#define RPC_XDR_H
/*
 * $Id: xdr.h,v 4.1 1994/09/26 08:07:59 jraja Exp jraja $
 *
 * External Data Representation Serialization Routines.
 *
 * Copyright © 1994 AmiTCP/IP Group,
 *                  Network Solutions Development Inc.
 *                  All rights reserved.
 *
 */
/* @(#)xdr.h	2.2 88/07/29 4.0 RPCSRC */
/*      @(#)xdr.h 1.19 87/04/22 SMI      */

/*
 * Copyright (C) 1984, Sun Microsystems, Inc.
 */

#include <stdio.h>		/* for the FILE* */
/*
 * XDR provides a conventional way for converting between C data
 * types and an external bit-string representation.  Library supplied
 * routines provide for the conversion on built-in C data types.  These
 * routines and utility routines defined here are used to help implement
 * a type encode/decode routine for each user-defined type.
 *
 * Each data type provides a single procedure which takes two arguments:
 *
 *	bool_t
 *	xdrproc(xdrs, argresp)
 *		XDR *xdrs;
 *		<type> *argresp;
 *
 * xdrs is an instance of a XDR handle, to which or from which the data
 * type is to be converted.  argresp is a pointer to the structure to be
 * converted.  The XDR handle contains an operation field which indicates
 * which of the operations (ENCODE, DECODE * or FREE) is to be performed.
 *
 * XDR_DECODE may allocate space if the pointer argresp is null.  This
 * data can be freed with the XDR_FREE operation.
 *
 * We write only one procedure per data type to make it easy
 * to keep the encode and decode procedures for a data type consistent.
 * In many cases the same code performs all operations on a user defined type,
 * because all the hard work is done in the component type routines.
 * decode as a series of calls on the nested data types.
 */

/*
 * Xdr operations.  XDR_ENCODE causes the type to be encoded into the
 * stream.  XDR_DECODE causes the type to be extracted from the stream.
 * XDR_FREE can be used to release the space allocated by an XDR_DECODE
 * request.
 */
enum xdr_op {
	XDR_ENCODE=0,
	XDR_DECODE=1,
	XDR_FREE=2
};

/*
 * This is the number of bytes per unit of external data.
 */
#define BYTES_PER_XDR_UNIT	(4)
#define RNDUP(x)  ((((x) + BYTES_PER_XDR_UNIT - 1) / BYTES_PER_XDR_UNIT) \
		    * BYTES_PER_XDR_UNIT)

/*
 * The XDR handle.
 * Contains operation which is being applied to the stream,
 * an operations vector for the paticular implementation (e.g. see xdr_mem.c),
 * and two private fields for the use of the particular impelementation.
 */
typedef struct XDR {
	enum xdr_op	x_op;		/* operation; fast additional param */
	struct xdr_ops {
		bool_t	(*x_getlong)(struct XDR *xdrs, long *longp);	/* get a long from underlying stream */
		bool_t	(*x_putlong)(struct XDR *xdrs, long *longp);	/* put a long to " */
		bool_t	(*x_getbytes)(struct XDR *xdrs, caddr_t addr, 
				      u_int len);/* get some bytes from " */
		bool_t	(*x_putbytes)(struct XDR *xdrs, caddr_t addr, 
				      u_int len);/* put some bytes to " */
		u_int	(*x_getpostn)(struct XDR *xdrs);/* returns bytes off from beginning */
		bool_t  (*x_setpostn)(struct XDR *xdrs, u_int pos);/* lets you reposition the stream */
		long *	(*x_inline)(struct XDR *xdrs, u_int len);/* buf quick ptr to buffered data */
		void	(*x_destroy)(struct XDR *xdrs);	/* free privates of this xdr_stream */
	} *x_ops;
	caddr_t 	x_public;	/* users' data */
	caddr_t		x_private;	/* pointer to private data */
	caddr_t 	x_base;		/* private used for position info */
	int		x_handy;	/* extra private word */
} XDR;

/*
 * A xdrproc_t exists for each data type which is to be encoded or decoded.
 *
 * The second argument to the xdrproc_t is a pointer to an opaque pointer.
 * The opaque pointer generally points to a structure of the data type
 * to be decoded.  If this pointer is 0, then the type routines should
 * allocate dynamic storage of the appropriate size and return it.
 * bool_t	(*xdrproc_t)(XDR *, caddr_t *);
 */
typedef	bool_t (* XDRFUN xdrproc_t)(XDR *, void *);
/*
 * xdr_string_t is for type casts in cases where a xdr function is called
 * via a pointer. If the function happens to be xdr_string(), then it uses
 * the last argument, which is passed as UINT_MAX, othervice the u_int is
 * just ignored.
 */
typedef	bool_t (* XDRFUN xdr_string_t)(XDR *, void *, u_int);

/*
 * Operations defined on a XDR handle
 *
 * XDR		*xdrs;
 * long		*longp;
 * caddr_t	 addr;
 * u_int	 len;
 * u_int	 pos;
 */
#define XDR_GETLONG(xdrs, longp)			\
	(*(xdrs)->x_ops->x_getlong)(xdrs, longp)
#define xdr_getlong(xdrs, longp)			\
	(*(xdrs)->x_ops->x_getlong)(xdrs, longp)

#define XDR_PUTLONG(xdrs, longp)			\
	(*(xdrs)->x_ops->x_putlong)(xdrs, longp)
#define xdr_putlong(xdrs, longp)			\
	(*(xdrs)->x_ops->x_putlong)(xdrs, longp)

#define XDR_GETBYTES(xdrs, addr, len)			\
	(*(xdrs)->x_ops->x_getbytes)(xdrs, addr, len)
#define xdr_getbytes(xdrs, addr, len)			\
	(*(xdrs)->x_ops->x_getbytes)(xdrs, addr, len)

#define XDR_PUTBYTES(xdrs, addr, len)			\
	(*(xdrs)->x_ops->x_putbytes)(xdrs, addr, len)
#define xdr_putbytes(xdrs, addr, len)			\
	(*(xdrs)->x_ops->x_putbytes)(xdrs, addr, len)

#define XDR_GETPOS(xdrs)				\
	(*(xdrs)->x_ops->x_getpostn)(xdrs)
#define xdr_getpos(xdrs)				\
	(*(xdrs)->x_ops->x_getpostn)(xdrs)

#define XDR_SETPOS(xdrs, pos)				\
	(*(xdrs)->x_ops->x_setpostn)(xdrs, pos)
#define xdr_setpos(xdrs, pos)				\
	(*(xdrs)->x_ops->x_setpostn)(xdrs, pos)

#define	XDR_INLINE(xdrs, len)				\
	(*(xdrs)->x_ops->x_inline)(xdrs, len)
#define	xdr_inline(xdrs, len)				\
	(*(xdrs)->x_ops->x_inline)(xdrs, len)

#define	XDR_DESTROY(xdrs)				\
	if ((xdrs)->x_ops->x_destroy) 			\
		(*(xdrs)->x_ops->x_destroy)(xdrs)
#define	xdr_destroy(xdrs)				\
	if ((xdrs)->x_ops->x_destroy) 			\
		(*(xdrs)->x_ops->x_destroy)(xdrs)

/*
 * Support struct for discriminated unions.
 * You create an array of xdrdiscrim structures, terminated with
 * a entry with a null procedure pointer.  The xdr_union routine gets
 * the discriminant value and then searches the array of structures
 * for a matching value.  If a match is found the associated xdr routine
 * is called to handle that part of the union.  If there is
 * no match, then a default routine may be called.
 * If there is no match and no default routine it is an error.
 */
#define NULL_xdrproc_t ((xdrproc_t)0)
struct xdr_discrim {
	int	value;
	xdrproc_t proc;
};

/*
 * In-line routines for fast encode/decode of primitve data types.
 * Caveat emptor: these use single memory cycles to get the
 * data from the underlying buffer, and will fail to operate
 * properly if the data is not aligned.  The standard way to use these
 * is to say:
 *	if ((buf = XDR_INLINE(xdrs, count)) == NULL)
 *		return (FALSE);
 *	<<< macro calls >>>
 * where ``count'' is the number of bytes of data occupied
 * by the primitive data types.
 *
 * N.B. and frozen for all time: each data type here uses 4 bytes
 * of external representation.
 */
#define IXDR_GET_LONG(buf)		((long)ntohl((u_long)*(buf)++))
#define IXDR_PUT_LONG(buf, v)		(*(buf)++ = (long)htonl((u_long)v))

#define IXDR_GET_BOOL(buf)		((bool_t)IXDR_GET_LONG(buf))
#define IXDR_GET_ENUM(buf, t)		((t)IXDR_GET_LONG(buf))
#define IXDR_GET_U_LONG(buf)		((u_long)IXDR_GET_LONG(buf))
#define IXDR_GET_SHORT(buf)		((short)IXDR_GET_LONG(buf))
#define IXDR_GET_U_SHORT(buf)		((u_short)IXDR_GET_LONG(buf))

#define IXDR_PUT_BOOL(buf, v)		IXDR_PUT_LONG((buf), ((long)(v)))
#define IXDR_PUT_ENUM(buf, v)		IXDR_PUT_LONG((buf), ((long)(v)))
#define IXDR_PUT_U_LONG(buf, v)		IXDR_PUT_LONG((buf), ((long)(v)))
#define IXDR_PUT_SHORT(buf, v)		IXDR_PUT_LONG((buf), ((long)(v)))
#define IXDR_PUT_U_SHORT(buf, v)	IXDR_PUT_LONG((buf), ((long)(v)))

/*
 * Function to free memory allocated by an xdr object
 */
void xdr_free(xdrproc_t proc, void * objp);

/*
 * These are the "generic" xdr routines.
 */
extern bool_t XDRFUN	xdr_void(XDR * xdrs, void * dummy);
extern bool_t XDRFUN	xdr_int(XDR * xdrs, int * ip);
extern bool_t XDRFUN	xdr_u_int(XDR * xdrs, u_int * uip);
extern bool_t XDRFUN	xdr_long(XDR * xdrs, long * lp);
extern bool_t XDRFUN	xdr_u_long(XDR * xdrs, u_long * ulp);
extern bool_t XDRFUN	xdr_short(XDR * xdrs, short * sp);
extern bool_t XDRFUN	xdr_u_short(XDR * xdrs, u_short * usp);
extern bool_t XDRFUN	xdr_bool(XDR * xdrs, bool_t * bp);
extern bool_t XDRFUN	xdr_enum(XDR * xdrs, enum_t * ep);
extern bool_t XDRFUN	xdr_array(XDR * xdrs, caddr_t * addrp, 
				  u_int * sizep, u_int maxsize, 
				  u_int elsize, xdrproc_t elproc);
extern bool_t XDRFUN	xdr_bytes(XDR * xdrs, char ** cpp, u_int * sizep, 
				  u_int maxsize);
extern bool_t XDRFUN	xdr_opaque(XDR * xdrs, caddr_t cp, u_int cnt);
extern bool_t XDRFUN	xdr_string(XDR * xdrs, char ** cpp, u_int maxsize);
extern bool_t XDRFUN	xdr_union(XDR * xdrs, int * dscmp, char * unp,
				  struct xdr_discrim * choices,
				  xdrproc_t dfault);
extern bool_t XDRFUN	xdr_char(XDR * xdrs, char * cp);
extern bool_t XDRFUN	xdr_u_char(XDR * xdrs, u_char * ucp);
extern bool_t XDRFUN	xdr_vector(XDR * xdrs, char * basep, u_int nelem, 
				   u_int elemsize, xdrproc_t xdr_elem);
extern bool_t XDRFUN	xdr_float(XDR *xdrs, float *fp);
extern bool_t XDRFUN	xdr_double(XDR *xdrs, double *dp);
extern bool_t XDRFUN	xdr_reference(XDR * xdrs, caddr_t * pp, u_int size,
				      xdrproc_t proc);
extern bool_t XDRFUN	xdr_pointer(XDR * xdrs, char ** objpp,
				    u_int obj_size, xdrproc_t xdr_obj);
extern bool_t XDRFUN	xdr_wrapstring(XDR * xdrs, char ** cpp);

/*
 * Common opaque bytes objects used by many rpc protocols;
 * declared here due to commonality.
 */
#define MAX_NETOBJ_SZ 1024 
struct netobj {
	u_int	n_len;
	char	*n_bytes;
};
typedef struct netobj netobj;
extern bool_t XDRFUN   xdr_netobj(XDR * xdrs, struct netobj * np);

/*
 * These are the public routines for the various implementations of
 * xdr streams.
 */
extern void   xdrmem_create(XDR * xdrs, caddr_t addr, u_int size,
			    enum xdr_op op);	/* XDR using memory buffers */
#ifdef USE_DOSIO
extern void   xdrstdio_create(XDR * xdrs, BPTR file, 
			      enum xdr_op op);	/* XDR using stdio library */
#else
extern void   xdrstdio_create(XDR * xdrs, FILE * file, 
			      enum xdr_op op);	/* XDR using stdio library */
#endif
extern void   xdrrec_create(XDR * xdrs, u_int sendsize, u_int recvsize,
			    void * tcp_handle,
			    int (* readit)(void *, caddr_t, int),
			    int (* writeit)(void *, caddr_t, int)); /* XDR pseudo records for tcp */
extern bool_t xdrrec_endofrecord(XDR * xdrs, int sendnow);/* make end of xdr record */
extern bool_t xdrrec_skiprecord(XDR * xdrs);/* move to beginning of next record */
extern bool_t xdrrec_eof(XDR * xdrs);	/* true if no more input */

#endif /* !RPC_XDR_H */
