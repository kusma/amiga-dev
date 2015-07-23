/*
 * POSIX Compatibility Library for AmigaOS
 *
 * Written by Frank Wille <frank@phoenix.owl.de> in 2005
 *
 * ====================================================
 * Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved.
 *
 * Developed at SunPro, a Sun Microsystems, Inc. business.
 * Permission to use, copy, modify, and distribute this
 * software is freely granted, provided that this notice
 * is preserved.
 * ====================================================
 *
 * $Id: hypot.c,v 1.1 2005/06/09 20:05:52 phx Exp $
 */

#include <math.h>
#include "math_private.h"


double hypot(double x,double y)
{
  double a=x,b=y,t1,t2,y1,y2,w;
  long j,k,ha,hb;

  ha = (long)__get_high_word(&x);
  ha &= 0x7fffffff;
  hb = (long)__get_high_word(&y);
  hb &= 0x7fffffff;
  if(hb > ha) {
    a=y;b=x;j=ha;
    ha=hb;hb=j;
  }
  else {
    a=x;b=y;
  }
  __set_high_word(&a,ha);    /* a <- |a| */
  __set_high_word(&b,hb);    /* b <- |b| */
  if((ha-hb)>0x3c00000) {
    return a+b;
  } /* x/y > 2**60 */
  k=0;
  if(ha > 0x5f300000) {   /* a>2**500 */
    if(ha >= 0x7ff00000) {       /* Inf or NaN */
      unsigned long low;

      w = a+b;                 /* for sNaN */
      low = __get_low_word(&a);
      if(((ha&0xfffff)|low)==0)
        w = a;
      low = __get_low_word(&b);
      if(((hb^0x7ff00000)|low)==0)
        w = b;
      return w;
    }
    /* scale a and b by 2**-600 */
    ha -= 0x25800000;
    hb -= 0x25800000;
    k += 600;
    __set_high_word(&a,ha);
    __set_high_word(&b,hb);
  }
  if(hb < 0x20b00000) {   /* b < 2**-500 */
    if(hb <= 0x000fffff) {      /* subnormal b or 0 */
      unsigned long low;

      low = __get_low_word(&b);
      if((hb|low)==0)
        return a;
      t1=0;
      __set_high_word(&t1,0x7fd00000);   /* t1=2^1022 */
      b *= t1;
      a *= t1;
      k -= 1022;
    }
    else {            /* scale a and b by 2^600 */
      ha += 0x25800000;       /* a *= 2^600 */
      hb += 0x25800000;       /* b *= 2^600 */
      k -= 600;
      __set_high_word(&a,ha);
      __set_high_word(&b,hb);
    }
  }
  /* medium size a and b */
  w = a-b;
  if (w>b) {
    t1 = 0;
    __set_high_word(&t1,ha);
    t2 = a-t1;
    w  = sqrt(t1*t1-(b*(-b)-t2*(a+t1)));
  }
  else {
    a  = a+a;
    y1 = 0;
    __set_high_word(&y1,hb);
    y2 = b - y1;
    t1 = 0;
    __set_high_word(&t1,ha+0x00100000);
    t2 = a - t1;
    w  = sqrt(t1*y1-(w*(-w)-(t1*y2+t2*b)));
  }
  if(k!=0) {
    unsigned long high;

    t1 = 1.0;
    high = __get_high_word(&t1);
    __set_high_word(&t1,high+(k<<20));
    return t1*w;
  }
  else return w;
}
