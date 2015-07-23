#ifndef __STDLIB_H
#define __STDLIB_H 1

#ifndef __SIZE_T
#define __SIZE_T 1
typedef unsigned long size_t;
#endif

#ifndef __WCHAR_T
#define __WCHAR_T 1
typedef char wchar_t;
#endif

#undef NULL
#define NULL ((void *)0)

#undef EXIT_FAILURE
#define EXIT_FAILURE 20
#undef EXIT_SUCCESS
#define EXIT_SUCCESS 0

#undef RAND_MAX
#define RAND_MAX 32767


void exit(int);
#if __STDC_VERSION__ >= 199901L
void _Exit(int);
#endif
void *malloc(size_t);
void *calloc(size_t,size_t);
void *realloc(void *,size_t);
void free(void *);
int system(const char *);
int rand(void);
void srand(unsigned int);
double atof(const char *);
int atoi(const char *);
long atol(const char *);
#if __STDC_VERSION__ >= 199901L
long long atoll(const char *);
#endif
double strtod(const char *,char **);
long strtol(const char *,char **,int);
#if __STDC_VERSION__ >= 199901L
signed long long strtoll(const char *,char **,int);
#endif
unsigned long strtoul(const char *,char **,int);
#if __STDC_VERSION__ >= 199901L
unsigned long long strtoull(const char *,char **,int);
#endif
void abort(void);
int atexit(void (*)(void));
char *getenv(const char *);
void *bsearch(const void *,const void *,size_t,size_t,int (*)(const void *,const void *));
void qsort(void *,size_t,size_t,int (*)(const void *,const void *));

typedef struct {
    int quot,rem;
} div_t;

typedef struct {
    long quot,rem;
} ldiv_t;

#if __STDC_VERSION__ >= 199901L
typedef struct {
    long long quot,rem;
} lldiv_t;
#endif

div_t div(int,int);
ldiv_t ldiv(long,long);
#if __STDC_VERSION__ >= 199901L
lldiv_t lldiv(long long,long long);
#endif

int abs(int);
long labs(long);
#if __STDC_VERSION__ >= 199901L
long long llabs(long long);
#endif

#ifndef __NOINLINE__
int __asm_abs(__reg("d0") int) =
                "\ttst.l\td0\n"
                "\tbpl.b\t*+2\n"
                "\tneg.l\td0\n"
                ";=barrier";
long __asm_labs(__reg("d0") long) =
                "\ttst.l\td0\n"
                "\tbpl.b\t*+2\n"
                "\tneg.l\td0\n"
                ";=barrier";
#if __STDC_VERSION__ >= 199901L
long long __asm_llabs(__reg("d0/d1") long long) =
                "\ttst.l\td0\n"
                "\tbpl.b\t*+4\n"
                "\tneg.l\td1\n"
                "\tnegx.l\td0\n"
                ";=barrier";
#define llabs(x) __asm_llabs(x)
#endif

#define abs(x) __asm_abs(x)
#define labs(x) __asm_labs(x)
#endif

extern size_t _nalloc;

#define atof(s) strtod((s),(char **)NULL)
#define atoi(s) (int)strtol((s),(char **)NULL,10)
#define atol(s) strtol((s),(char **)NULL,10)

struct __exitfuncs{
    struct __exitfuncs *next;
    void (*func)(void);
};

#endif
