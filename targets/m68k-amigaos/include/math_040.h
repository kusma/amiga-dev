/* $VER: math_040.h 0.7 (24.08.2014)
** math.h 68040 specific support, link with -lm040
*/

/* macros */
int __asm_fpclassify(__reg("fp0")double)="\tftst.x\tfp0\n"
                                         "\tmoveq\t#7,d1\n"
                                         "\tfmove.l\tfpsr,d0\n"
                                         "\trol.l\t#8,d0\n"
                                         "\tand.l\td1,d0";
int __asm_isfinite(__reg("fp0")double)="\tftst.x\tfp0\n"
                                       "\tfmove.l\tfpsr,d0\n"
                                       "\tand.l\t#$03000000,d0\n"
                                       "\tseq\td0\n"
                                       "\textb.l\td0";
int __asm_isnormal(__reg("fp0")double)="\tftst.x\tfp0\n"
                                       "\tfmove.l\tfpsr,d0\n"
                                       "\tand.l\t#$07000000,d0\n"
                                       "\tseq\td0\n"
                                       "\textb.l\td0";
int __asm_isnan(__reg("fp0")double)="\tftst.x\tfp0\n"
                                    "\tmoveq\t#1,d0\n"
                                    "\tfbun.w\t*+4\n"
                                    "\tmoveq\t#0,d0\n"
                                    ";=barrier";
int __asm_isinf(__reg("fp0")double)="\tftst.x\tfp0\n"
                                    "\tmoveq\t#1,d0\n"
                                    "\tfmove.l\tfpsr,d1\n"
                                    "\trol.l\t#7,d1\n"
                                    "\tand.l\td1,d0";
int __asm_signbit(__reg("fp0")double)="\tfmove.s\tfp0,d0\n"
                                      "\tmoveq\t#31,d1\n"
                                      "\tasr.l\td1,d0";
int __asm_isgreater(__reg("fp0")double,__reg("fp1")double)="\tfcmp.x\tfp1,fp0\n"
                                                           "\tmoveq\t#1,d0\n"
                                                           "\tfbogt.w\t*+4\n"
                                                           "\tmoveq\t#0,d0\n"
                                                           ";=barrier";
int __asm_isgreaterequal(__reg("fp0")double,__reg("fp1")double)="\tfcmp.x\tfp1,fp0\n"
                                                                "\tmoveq\t#0,d0\n"
                                                                "\tfbult.w\t*+4\n"
                                                                "\tmoveq\t#1,d0\n"
                                                                ";=barrier";
int __asm_isless(__reg("fp0")double,__reg("fp1")double)="\tfcmp.x\tfp1,fp0\n"
                                                        "\tmoveq\t#1,d0\n"
                                                        "\tfbolt.w\t*+4\n"
                                                        "\tmoveq\t#0,d0\n"
                                                        ";=barrier";
int __asm_islessequal(__reg("fp0")double,__reg("fp1")double)="\tfcmp.x\tfp1,fp0\n"
                                                             "\tmoveq\t#0,d0\n"
                                                             "\tfbugt.w\t*+4\n"
                                                             "\tmoveq\t#1,d0\n"
                                                             ";=barrier";
int __asm_islessgreater(__reg("fp0")double,__reg("fp1")double)="\tfcmp.x\tfp1,fp0\n"
                                                               "\tmoveq\t#0,d0\n"
                                                               "\tfbueq.w\t*+4\n"
                                                               "\tmoveq\t#1,d0\n"
                                                               ";=barrier";
int __asm_isunordered(__reg("fp0")double,__reg("fp1")double)="\tfcmp.x\tfp1,fp0\n"
                                                             "\tmoveq\t#1,d0\n"
                                                             "\tfbun.w\t*+4\n"
                                                             "\tmoveq\t#0,d0\n"
                                                             ";=barrier";

#define fpclassify(x) __asm_fpclassify(x)
#define isfinite(x) __asm_isfinite(x)
#define isnormal(x) __asm_isnormal(x)
#define isnan(x) __asm_isnan(x)
#define isinf(x) __asm_isinf(x)
#define signbit(x) __asm_signbit(x)
#define isgreater(x,y) __asm_isgreater(x,y)
#define isgreaterequal(x,y) __asm_isgreaterequal(x,y)
#define isless(x,y) __asm_isless(x,y)
#define islessequal(x,y) __asm_islessequal(x,y)
#define islessgreater(x,y) __asm_islessgreater(x,y)
#define isunordered(x,y) __asm_isunordered(x,y)

#ifndef __NOINLINE__
/* faster inline functions */
double __asm_ceil(__reg("fp0")double)="\tfmove.l\tfpcr,d1\n"
                                      "\tmoveq\t#$30,d0\n"
                                      "\tor.l\td1,d0\n"
                                      "\tfmove.l\td0,fpcr\n"
                                      "\tfmove.l\tfp0,d0\n"
                                      "\tfmove.l\td0,fp0\n"
                                      "\tfmove.l\td1,fpcr";
double __asm_copysign(__reg("fp0")double,__reg("fp1")double)="\tfmove.s\tfp1,d0\n"
                                                             "\tfabs.x\tfp0\n"
                                                             "\ttst.l\td0\n"
                                                             "\tbpl.b\t*+4\n"
                                                             "\tfneg.x\tfp0\n"
                                                             ";=barrier";
double __asm_fabs(__reg("fp0")double)="\tfabs.x\tfp0";
double __asm_fdim(__reg("fp0")double,__reg("fp1")double)="\tfsub.x\tfp1,fp0\n"
                                                         "\tfabs.x\tfp0";
double __asm_floor(__reg("fp0")double)="\tfmove.l\tfpcr,d1\n"
                                       "\tmoveq\t#$20,d0\n"
                                       "\tor.l\td1,d0\n"
                                       "\tand.w\t#$ffef,d0\n"
                                       "\tfmove.l\td0,fpcr\n"
                                       "\tfmove.l\tfp0,d0\n"
                                       "\tfmove.l\td0,fp0\n"
                                       "\tfmove.l\td1,fpcr";
double __asm_fma(__reg("fp0")double,__reg("fp1")double,__reg("fp2")double)="\tfmul.x\tfp1,fp0\n"
                                                                           "\tfadd.x\tfp2,fp0";
double __asm_fmax(__reg("fp0")double,__reg("fp1")double)="\tftst.x\tfp1\n"
                                                         "\tfbun.w\t*+14\n"
                                                         "\tfcmp.x\tfp1,fp0\n"
                                                         "\tfboge.w\t*+6\n"
                                                         "\tfmove.x\tfp1,fp0\n"
                                                         ";=barrier";
double __asm_fmin(__reg("fp0")double,__reg("fp1")double)="\tftst.x\tfp1\n"
                                                         "\tfbun.w\t*+14\n"
                                                         "\tfcmp.x\tfp1,fp0\n"
                                                         "\tfbole.w\t*+6\n"
                                                         "\tfmove.x\tfp1,fp0\n"
                                                         ";=barrier";
double __asm_hypot(__reg("fp0")double,__reg("fp1")double)="\tfmul.x\tfp0,fp0\n"
                                                          "\tfmul.x\tfp1,fp1\n"
                                                          "\tfadd.x\tfp1,fp0\n"
                                                          "\tfsqrt.x\tfp0";
double __asm_nan(__reg("fp0")double)="\tfmove.s\t#$7fc00000,fp0";
double __asm_nearbyint(__reg("fp0")double)="\tfmove.l\tfp0,d0\n"
                                           "\tfmove.l\td0,fp0";
double __asm_sqrt(__reg("fp0")double)="\tfsqrt.x\tfp0";
double __asm_trunc(__reg("fp0")double)="\tmoveq\t#$10,d0\n"
                                       "\tfmove.l\tfpcr,d1\n"
                                       "\tor.l\td1,d0\n"
                                       "\tand.w\t#$ffdf,d0\n"
                                       "\tfmove.l\td0,fpcr\n"
                                       "\tfmove.l\tfp0,d0\n"
                                       "\tfmove.l\td0,fp0\n"
                                       "\tfmove.l\td1,fpcr";

/* double precision */
#define ceil(x) __asm_ceil(x)
#define copysign(x,y) __asm_copysign(x,y)
#define fabs(x) __asm_fabs(x)
#define fdim(x,y) __asm_fdim(x,y)
#define floor(x) __asm_floor(x)
#define fma(x,y) __asm_fma(x,y)
#define fmax(x,y) __asm_fmax(x,y)
#define fmin(x,y) __asm_fmin(x,y)
#define hypot(x,y) __asm_hypot(x,y)
#define nan(x) __asm_nan(x)
#define nearbyint(x) __asm_nearbyint(x)
#define remainder(x,y) __asm_remainder(x,y)
#define sqrt(x) __asm_sqrt(x)
#define trunc(x) __asm_trunc(x)

/* single precision */
#define ceilf(x) __asm_ceil(x)
#define copysignf(x,y) __asm_copysign(x,y)
#define fabsf(x) __asm_fabs(x)
#define fdimf(x,y) __asm_fdim(x,y)
#define floorf(x) __asm_floor(x)
#define fmaf(x,y) __asm_fma(x,y)
#define fmaxf(x,y) __asm_fmax(x,y)
#define fminf(x,y) __asm_fmin(x,y)
#define hypotf(x,y) __asm_hypot(x,y)
#define nanf(x) __asm_nan(x)
#define nearbyintf(x) __asm_nearbyint(x)
#define remainderf(x,y) __asm_remainder(x,y)
#define sqrtf(x) __asm_sqrt(x)
#define truncf(x) __asm_trunc(x)

#endif /* __NOINLINE__ */
