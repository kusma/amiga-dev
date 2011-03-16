#ifndef	SYS_CDEFS_H
#define	SYS_CDEFS_H \
       "$Id: cdefs.h,v 4.2 1995/09/08 10:56:28 jraja Exp $"
/*
 *      C compiler dependent definitions
 *
 *      Copyright © 1994 AmiTCP/IP Group,
 *                       Network Solutions Development, Inc.
 *                       All rights reserved.
 */

#if defined(__cplusplus)
#define	__BEGIN_DECLS	extern "C" {
#define	__END_DECLS	};
#else
#define	__BEGIN_DECLS
#define	__END_DECLS
#endif

/*
 * SAVEDS should be used in all function definitions which will be called 
 * from other tasks than AmiTCP/IP. Is restores the global data base pointer
 * as the first thing in the function body.
 *
 * REGARGFUN contains special keywords which should be used when functions
 * used through shared library are referenced.
 */
#if __SASC_60
#define SAVEDS __saveds
#define REGARGFUN __asm
#define STKARGFUN __stdargs
#define ALIGNED __aligned
#define ASM __asm
#define REG(x) register __##x
#define COMMON __far
#else
#define SAVEDS
#define REGARGFUN
#define STKARGFUN
#define ALIGNED
#define ASM
#define REG(x)
#endif

/*
 * The __CONCAT macro is used to concatenate parts of symbol names, e.g.
 * with "#define OLD(foo) __CONCAT(old,foo)", OLD(foo) produces oldfoo.
 * The __CONCAT macro is a bit tricky -- make sure you don't put spaces
 * in between its arguments.  __CONCAT can also concatenate double-quoted
 * strings produced by the __STRING macro, but this only works with ANSI C.
 */
#if defined(__STDC__) || defined(__cplusplus)
#define	__P(protos)	protos		/* full-blown ANSI C */
#define	__CONCAT(x,y)	x ## y
#define	__STRING(x)	#x

#if __SASC_60
#define inline          __inline
#endif

#else	/* !(__STDC__ || __cplusplus) */
#define	__P(protos)	()		/* traditional C preprocessor */
#define	__CONCAT(x,y)	x/**/y
#define	__STRING(x)	"x"

#if __GNUC__
#define	const		__const__	/* GCC: ANSI C with -traditional */
#define	inline		__inline__
#define	signed		__signed__
#define	volatile	__volatile__

#else	/* !__GNUC__ */
#define	const				/* delete ANSI C keywords */
#define	inline
#define	signed
#define	volatile
#endif	/* !__GNUC__ */
#endif	/* !(__STDC__ || __cplusplus) */

#endif /* !SYS_CDEFS_H */
