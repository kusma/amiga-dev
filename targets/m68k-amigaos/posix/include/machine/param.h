/*
 * POSIX Compatibility Library for AmigaOS
 *
 * Written by Frank Wille <frank@phoenix.owl.de> in 2005
 *
 * $Id: param.h,v 1.1 2005/04/23 22:10:20 phx Exp $
 */

#ifndef _MACHINE_PARAM_H_
#define _MACHINE_PARAM_H_


#ifdef __PPC__
#define _MACHINE        powerpc
#define MACHINE         "powerpc"
#define _MACHINE_ARCH   powerpc
#define MACHINE_ARCH    "powerpc"
#else
#define _MACHINE        m68k
#define MACHINE         "m68k"
#define _MACHINE_ARCH   m68k
#define MACHINE_ARCH    "m68k"
#endif

/*
 * Constants related to network buffer management.
 * MCLBYTES must be no larger than NBPG (the software page size), and,
 * on machines that exchange pages of input or output buffers with mbuf
 * clusters (MAPPED_MBUFS), MCLBYTES must also be an integral multiple
 * of the hardware page size.
 */
#define MSIZE           256             /* size of an mbuf */

#ifndef MCLSHIFT
#define MCLSHIFT        11              /* convert bytes to m_buf clusters */
                                        /* 2K cluster can hold Ether frame */
#define	MCLOFSET        (MCLBYTES - 1)
#endif  /* MCLSHIFT */


#endif /* _MACHINE_PARAM_H_ */
