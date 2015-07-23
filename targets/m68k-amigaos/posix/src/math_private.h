/*
 * POSIX Compatibility Library for AmigaOS
 *
 * Written by Frank Wille <frank@phoenix.owl.de> in 2005
 *
 * $Id: math_private.h,v 1.1 2005/06/09 20:05:52 phx Exp $
 */

/* inline functions to access high and low word of a double prec. value */

#ifdef __PPC__

unsigned long __get_high_word(__reg("r3")double *d) = 
    "\tlwz\t3,0(3)";

unsigned long __get_low_word(__reg("r3")double *d) = 
    "\tlwz\t3,4(3)";

void __set_high_word(__reg("r3")double *d,__reg("r4")unsigned long ix) =
    "\tstw\t4,0(3)";

void __set_low_word(__reg("r3")double *d,__reg("r4")unsigned long ix) =
    "\tstw\t4,4(3)";

#else /* M68k */

unsigned long __get_high_word(__reg("a0")double *d) =
    "\tmove.l\t(a0),d0";

unsigned long __get_low_word(__reg("a0")double *d) =
    "\tmove.l\t4(a0),d0";

void __set_high_word(__reg("a0")double *d,__reg("d0")unsigned long ix) =
    "\tmove.l\td0,(a0)";

void __set_low_word(__reg("a0")double *d,__reg("d0")unsigned long ix) =
    "\tmove.l\td0,4(a0)";

#endif
