/*
 * POSIX Compatibility Library for AmigaOS
 *
 * Written by Frank Wille <frank@phoenix.owl.de> in 2003
 *
 * $Id: cdefs.h,v 1.1 2005/04/11 19:24:37 phx Exp $
 */

#ifndef _SYS_CDEFS_H_
#define _SYS_CDEFS_H_

/* @@@ fix this: */
#define __IDSTRING(_n,_s) static const char __idstring[] = _s
#define __COPYRIGHT(_s) static const char __copyright[] = _s
#define __RCSID(_s) static const char __rcsid[] = _s
#define __SCCSID(_s) static const char __sccsid[] = _s
#define __SCCSID2(_s) static const char __sccsid2[] = _s

#if defined(__cplusplus)
#define __BEGIN_DECLS           extern "C" {
#define __END_DECLS             }
#define __static_cast(x,y)      static_cast<x>(y)
#else
#define __BEGIN_DECLS
#define __END_DECLS
#define __static_cast(x,y)      (x)y
#endif

#define ___STRING(x)    __STRING(x)
#define ___CONCAT(x,y)  __CONCAT(x,y)

#if __STDC__ || defined(__cplusplus)
#define __P(protos)     protos          /* full-blown ANSI C */
#define __CONCAT(x,y)   x ## y
#define __STRING(x)     #x

#define __const         const           /* define reserved names to standard */
#define __signed        signed
#define __volatile      volatile
#if defined(__cplusplus)
#define __inline        inline          /* convert to C++ keyword */
#else
#if !defined(__GNUC__) && !defined(__lint__)
#define __inline                        /* delete GCC keyword */
#endif /* !__GNUC__  && !__lint__ */
#endif /* !__cplusplus */

#else   /* !(__STDC__ || __cplusplus) */
#define __P(protos)     ()              /* traditional C preprocessor */
#define __CONCAT(x,y)   x/**/y
#define __STRING(x)     "x"
#define __const                         /* delete pseudo-ANSI C keywords */
#define __inline
#define __signed
#define __volatile

#ifndef NO_ANSI_KEYWORDS
#define const           __const         /* convert ANSI C keywords */
#define inline          __inline
#define signed          __signed
#define volatile        __volatile
#endif /* !NO_ANSI_KEYWORDS */
#endif  /* !(__STDC__ || __cplusplus) */

#if __STDC_VERSION__ >= 199901L
#define __restrict      restrict
#else
#define __restrict      /* delete __restrict when not supported */
#endif

#endif  /* _SYS_CDEFS_H_ */
