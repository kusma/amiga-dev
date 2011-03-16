#ifndef __MATH_H
#define __MATH_H 1

extern const char __infinity[];
#define HUGE_VAL (*(const double *)(const void *)__infinity)

double sqrt(double);
double fabs(double);
double floor(double);
double ceil(double);
double sin(double);
double cos(double);
double tan(double);
double sinh(double);
double cosh(double);
double tanh(double);
double asin(double);
double acos(double);
double atan(double);
double exp(double);
double log(double);
double log10(double);
double pow(double,double);
double modf(double,double *);
double fmod(double,double);
double atan2(double,double);
double ldexp(double,int);
double frexp(double,int *);

#ifndef __NOINLINE__

#if defined(__M68881)||defined(__M68882)
double __asm_sqrt(__reg("fp0") double)="\tfsqrt.x\tfp0";
double __asm_fabs(__reg("fp0") double)="\tfabs.x\tfp0";
#if !defined(__M68040)
double __asm_floor(__reg("fp0") double)="\tfmove.x\tfp0,fp1\n"
                                        "\tfintrz.x\tfp1,fp0\n"
                                        "\tfcmp.x\tfp1,fp0\n"
                                        "\tfbole.w\t*+10\n"
                                        "\tfsub.s\t#$3f800000,fp0\n"
                                        ";=barrier";
double __asm_ceil(__reg("fp0") double)="\tfmove.x\tfp0,fp1\n"
                                       "\tfintrz.x\tfp1,fp0\n"
                                       "\tfcmp.x\tfp1,fp0\n"
                                       "\tfboge.w\t*+10\n"
                                       "\tfadd.s\t#$3f800000,fp0\n"
                                        ";=barrier";
#else
double __asm_floor(__reg("fp0") double)="\tfmove.l\tfpcr,d1\n\tmoveq\t#32,d0\n\tor.l\td1,d0\n\tand.b\t#255-16,d0\n\tfmove.l\td0,fpcr\n\tfint.x\tfp0\n\tfmove.l\td1,fpcr";
double __asm_ceil(__reg("fp0") double)="\tfmove.l\tfpcr,d1\n\tmoveq\t#48,d0\n\tor.l\td1,d0\n\tfmove.l\td0,fpcr\n\tfint.x\tfp0\n\tfmove.l\td1,fpcr";
#endif

#define sqrt(x) __asm_sqrt(x)
#define fabs(x) __asm_fabs(x)
#define floor(x) __asm_floor(x)
#define ceil(x) __asm_ceil(x)
#endif

#if (defined(__M68881)||defined(__M68882)) && !defined(__M68040) && !defined(__M68060)
double __asm_acos(__reg("fp0")double)="\tfacos.x\tfp0";
double __asm_asin(__reg("fp0")double)="\tfasin.x\tfp0";
double __asm_atan(__reg("fp0")double)="\tfatan.x\tfp0";
double __asm_cos(__reg("fp0")double)="\tfcos.x\tfp0";
double __asm_cosh(__reg("fp0")double)="\tfcosh.x\tfp0";
double __asm_exp(__reg("fp0")double)="\tfetox.x\tfp0";
double __asm_log(__reg("fp0")double)="\tflogn.x\tfp0";
double __asm_log10(__reg("fp0")double)="\tflog10.x\tfp0";
double __asm_modf(__reg("fp0")double,__reg("a0") double *)="\tfintrz.x\tfp0,fp1\n\tfmove.d\tfp1,(a0)\n\tfsub.x\tfp1,fp0";
double __asm_sin(__reg("fp0")double)="\tfsin.x\tfp0";
double __asm_sinh(__reg("fp0")double)="\tfsinh.x\tfp0";
double __asm_tan(__reg("fp0")double)="\tftan.x\tfp0";
double __asm_tanh(__reg("fp0")double)="\tftanh.x\tfp0";
double __asm_fmod(__reg("fp0")double,__reg("fp1")double)="\tfmod.x\tfp1,fp0";

#define acos(x) __asm_acos(x)
#define asin(x) __asm_asin(x)
#define atan(x) __asm_atan(x)
#define cos(x) __asm_cos(x)
#define cosh(x) __asm_cosh(x)
#define exp(x) __asm_exp(x)
#define log(x) __asm_log(x)
#define log10(x) __asm_log10(x)
#define modf(x,y) __asm_modf(x,y)
#define sin(x) __asm_sin(x)
#define sinh(x) __asm_sinh(x)
#define tan(x) __asm_tan(x)
#define tanh(x) __asm_tanh(x)
#define fmod(x,y) __asm_fmod(x,y)
#endif

#endif /* __NOINLINE__ */

#endif
