/* $VER: math_881.h 0.7 (11.08.2014)
** math.h 6888x specific support, link with -lm881
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
double __asm_acos(__reg("fp0")double)="\tfacos.x\tfp0";
double __asm_asin(__reg("fp0")double)="\tfasin.x\tfp0";
double __asm_atan(__reg("fp0")double)="\tfatan.x\tfp0";
double __asm_atanh(__reg("fp0")double)="\tfatanh.x\tfp0";
double __asm_ceil(__reg("fp0")double)="\tfmove.x\tfp0,fp1\n"
                                      "\tfintrz.x\tfp0\n"
                                      "\tfcmp.x\tfp1,fp0\n"
                                      "\tfboge.w\t*+10\n"
                                      "\tfadd.s\t#$3f800000,fp0\n"
                                      ";=barrier";
double __asm_copysign(__reg("fp0")double,__reg("fp1")double)="\tfmove.s\tfp1,d0\n"
                                                             "\tfabs.x\tfp0\n"
                                                             "\ttst.l\td0\n"
                                                             "\tbpl.b\t*+4\n"
                                                             "\tfneg.x\tfp0\n"
                                                             ";=barrier";
double __asm_cos(__reg("fp0")double)="\tfcos.x\tfp0";
double __asm_cosh(__reg("fp0")double)="\tfcosh.x\tfp0";
double __asm_exp(__reg("fp0")double)="\tfetox.x\tfp0";
double __asm_exp2(__reg("fp0")double)="\tftwotox.x\tfp0";
double __asm_exp10(__reg("fp0")double)="\tftentox.x\tfp0";
double __asm_expm1(__reg("fp0")double)="\tfetoxm1.x\tfp0";
double __asm_fabs(__reg("fp0")double)="\tfabs.x\tfp0";
double __asm_fdim(__reg("fp0")double,__reg("fp1")double)="\tfsub.x\tfp1,fp0\n"
                                                         "\tfabs.x\tfp0";
double __asm_floor(__reg("fp0")double)="\tfmove.x\tfp0,fp1\n"
                                       "\tfintrz.x\tfp0\n"
                                       "\tfcmp.x\tfp1,fp0\n"
                                       "\tfbole.w\t*+10\n"
                                       "\tfsub.s\t#$3f800000,fp0\n"
                                       ";=barrier";
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
double __asm_fmod(__reg("fp0")double,__reg("fp1")double)="\tfmod.x\tfp1,fp0";
double __asm_hypot(__reg("fp0")double,__reg("fp1")double)="\tfmul.x\tfp0,fp0\n"
                                                          "\tfmul.x\tfp1,fp1\n"
                                                          "\tfadd.x\tfp1,fp0\n"
                                                          "\tfsqrt.x\tfp0";
double __asm_ldexp(__reg("fp0")double,
                   __reg("d0")int)="\tfscale.l\td0,fp0";
double __asm_log(__reg("fp0")double)="\tflogn.x\tfp0";
double __asm_log10(__reg("fp0")double)="\tflog10.x\tfp0";
double __asm_log2(__reg("fp0")double)="\tflog2.x\tfp0";
double __asm_logb(__reg("fp0")double)="\tfgetexp.x\tfp0";
double __asm_lognp1(__reg("fp0")double)="\tflognp1.x\tfp0";
long   __asm_lround(__reg("fp0")double)="\tfmove.s\tfp0,d0\n"
                                        "\tand.l\t#$80000000,d0\n"
                                        "\tor.l\t#3f000000,d0\n"
                                        "\tfadd.s\td0,fp0\n"
                                        "\tfintrz.x\tfp0\n"
                                        "\tfmove.l\tfp0,d0";
double __asm_modf(__reg("fp0")double,
                  __reg("a0")double *)="\tfintrz.x\tfp0,fp1\n"
                                       "\tfmove.d\tfp1,(a0)\n"
                                       "\tfsub.x\tfp1,fp0";
double __asm_nan(__reg("fp0")double)="\tfmove.s\t#$7fc00000,fp0";
double __asm_nearbyint(__reg("fp0")double)="\tfint.x\tfp0";
double __asm_remainder(__reg("fp0")double,__reg("fp1")double)="\tfrem.x\tfp1,fp0";
double __asm_round(__reg("fp0")double)="\tfmove.s\tfp0,d0\n"
                                       "\tand.l\t#$80000000,d0\n"
                                       "\tor.l\t#3f000000,d0\n"
                                       "\tfadd.s\td0,fp0\n"
                                       "\tfintrz.x\tfp0";
double __asm_sin(__reg("fp0")double)="\tfsin.x\tfp0";
double __asm_sinh(__reg("fp0")double)="\tfsinh.x\tfp0";
double __asm_sqrt(__reg("fp0")double)="\tfsqrt.x\tfp0";
double __asm_tan(__reg("fp0")double)="\tftan.x\tfp0";
double __asm_tanh(__reg("fp0")double)="\tftanh.x\tfp0";
double __asm_trunc(__reg("fp0")double)="\tfintrz.x\tfp0";

/* double precision */
#define acos(x) __asm_acos(x)
#define asin(x) __asm_asin(x)
#define atan(x) __asm_atan(x)
#define atanh(x) __asm_atanh(x)
#define ceil(x) __asm_ceil(x)
#define copysign(x,y) __asm_copysign(x,y)
#define cos(x) __asm_cos(x)
#define cosh(x) __asm_cosh(x)
#define exp(x) __asm_exp(x)
#define exp2(x) __asm_exp2(x)
#define exp10(x) __asm_exp10(x)
#define expm1(x) __asm_expm1(x)
#define fabs(x) __asm_fabs(x)
#define fdim(x,y) __asm_fdim(x,y)
#define floor(x) __asm_floor(x)
#define fma(x,y) __asm_fma(x,y)
#define fmax(x,y) __asm_fmax(x,y)
#define fmin(x,y) __asm_fmin(x,y)
#define fmod(x,y) __asm_fmod(x,y)
#define hypot(x,y) __asm_hypot(x,y)
#define ldexp(x,exp) __asm_ldexp(x,exp)
#define log(x) __asm_log(x)
#define log10(x) __asm_log10(x)
#define log2(x) __asm_log2(x)
#define logb(x) __asm_logb(x)
#define lognp1(x) __asm_lognp1(x)
#define lround(x) __asm_lround(x)
#define modf(x,y) __asm_modf(x,y)
#define nan(x) __asm_nan(x)
#define nearbyint(x) __asm_nearbyint(x)
#define remainder(x,y) __asm_remainder(x,y)
#define round(x) __asm_round(x)
#define sin(x) __asm_sin(x)
#define sinh(x) __asm_sinh(x)
#define sqrt(x) __asm_sqrt(x)
#define tan(x) __asm_tan(x)
#define tanh(x) __asm_tanh(x)
#define trunc(x) __asm_trunc(x)

/* single precision */
#define acosf(x) __asm_acos(x)
#define asinf(x) __asm_asin(x)
#define atanf(x) __asm_atan(x)
#define atanhf(x) __asm_atanh(x)
#define ceilf(x) __asm_ceil(x)
#define copysignf(x,y) __asm_copysign(x,y)
#define cosf(x) __asm_cos(x)
#define coshf(x) __asm_cosh(x)
#define expf(x) __asm_exp(x)
#define exp2f(x) __asm_exp2(x)
#define exp10f(x) __asm_exp10(x)
#define expm1f(x) __asm_expm1(x)
#define fabsf(x) __asm_fabs(x)
#define fdimf(x,y) __asm_fdim(x,y)
#define floorf(x) __asm_floor(x)
#define fmaf(x,y) __asm_fma(x,y)
#define fmaxf(x,y) __asm_fmax(x,y)
#define fminf(x,y) __asm_fmin(x,y)
#define fmodf(x,y) __asm_fmod(x,y)
#define hypotf(x,y) __asm_hypot(x,y)
#define ldexpf(x,exp) __asm_ldexp(x,exp)
#define logf(x) __asm_log(x)
#define log10f(x) __asm_log10(x)
#define log2f(x) __asm_log2(x)
#define logbf(x) __asm_logb(x)
#define lognp1f(x) __asm_lognp1(x)
#define lroundf(x) __asm_lround(x)
#define modff(x,y) __asm_modf(x,y)
#define nanf(x) __asm_nan(x)
#define nearbyintf(x) __asm_nearbyint(x)
#define remainderf(x,y) __asm_remainder(x,y)
#define roundf(x) __asm_round(x)
#define sinf(x) __asm_sin(x)
#define sinhf(x) __asm_sinh(x)
#define sqrtf(x) __asm_sqrt(x)
#define tanf(x) __asm_tan(x)
#define tanhf(x) __asm_tanh(x)
#define truncf(x) __asm_trunc(x)
#endif /* __NOINLINE__ */
