#ifndef _VBCCINLINE_MATHIEEEDOUBBAS_H
#define _VBCCINLINE_MATHIEEEDOUBBAS_H

#ifndef EXEC_TYPES_H
#include <exec/types.h>
#endif

DOUBLE __IEEEDPFlt(__reg("a6") void *, __reg("d0") LONG integer)="\tjsr\t-36(a6)";
#define IEEEDPFlt(integer) __IEEEDPFlt(MathIeeeDoubBasBase, (integer))

#endif /*  _VBCCINLINE_MATHIEEEDOUBBAS_H  */
