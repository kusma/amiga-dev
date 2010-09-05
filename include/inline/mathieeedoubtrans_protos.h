#ifndef _VBCCINLINE_MATHIEEEDOUBTRANS_H
#define _VBCCINLINE_MATHIEEEDOUBTRANS_H

#ifndef EXEC_TYPES_H
#include <exec/types.h>
#endif

DOUBLE __IEEEDPFieee(__reg("a6") void *, __reg("d0") FLOAT single)="\tjsr\t-108(a6)";
#define IEEEDPFieee(single) __IEEEDPFieee(MathIeeeDoubTransBase, (single))

#endif /*  _VBCCINLINE_MATHIEEEDOUBTRANS_H  */
