#ifndef __STRING_H
#define __STRING_H 1

#ifndef __SIZE_T
#define __SIZE_T 1
typedef unsigned long size_t;
#endif

#undef NULL
#define NULL ((void *)0)

void *memcpy(void *,const void *,size_t);
void *memmove(void *,const void *,size_t);
char *strcpy(char *,const char *);
char *strncpy(char *,const char *,size_t);
char *strcat(char *,const char *);
char *strncat(char *,const char *,size_t);
int memcmp(const void *,const void *,size_t);
int strcmp(const char *,const char *);
int strncmp(const char *,const char *,size_t);
void *memchr(const void *,int,size_t);
char *strchr(const char *,int);
size_t strcspn(const char *,const char *);
char *strpbrk(const char *,const char *);
char *strrchr(const char *,int);
size_t strspn(const char *,const char *);
char *strstr(const char *,const char *);
void *memset(void *,int,size_t);
size_t strlen(const char *);
char *strtok(char *,const char *);
char *strerror(int);
int strcoll(const char *,const char *);
size_t strxfrm(char *,const char *,size_t);

#ifndef __NOINLINE__
#if defined(__M68000) || defined(__M68010)
void *__asm_memcpy(__reg("a0") void *, __reg("a1") const void *,
                   __reg("d2") size_t) =
        "\tmove.l\ta0,d0\n"
        "\tcmp.l\t#16,d2\n"
        "\tblo.b\t*+48\n"
        "\tmoveq\t#1,d1\n"
        "\tand.b\td0,d1\n"
        "\tbeq.b\t*+4\n"
        "\tmove.b\t(a1)+,(a0)+\n"
        "\tsubq.l\t#1,d2\n"
        ";=barrier\n"
        "\tmove.l\ta1,d1\n"
        "\tand.b\t#1,d1\n"
        "\tbeq.b\t*+16\n"
        "\tcmp.l\t#$10000,d2\n"
        "\tblo.b\t*+22\n"
        ";=barrier\n"
        "\tmove.b\t(a1)+,(a0)+\n"
        "\tsubq.l\t#1,d2\n"
        "\tbne.b\t*-6\n"
        "\tbra.b\t*+24\n"
        ";=barrier\n"
        "\tmoveq\t#3,d1\n"
        "\tand.l\td2,d1\n"
        "\tsub.l\td1,d2\n"
        ";=barrier\n"
        "\tmove.l\t(a1)+,(a0)+\n"
        "\tsubq.l\t#4,d2\n"
        "\tbne.b\t*-6\n"
        "\tmove.w\td1,d2\n"
        ";=barrier\n"
        "\tsubq.w\t#1,d2\n"
        "\tblo.b\t*+6\n"
        ";=barrier\n"
        "\tmove.b\t(a1)+,(a0)+\n"
        "\tdbf\td2,*-4\n"
        ";=barrier";

void *__asm_memcpy_desc(__reg("a0") void *, __reg("a1") const void *,
                        __reg("d2") size_t) =
        "\tcmp.l\t#16,d2\n"
        "\tblo.b\t*+48\n"
        "\tmoveq\t#1,d1\n"
        "\tmove.l\ta0,d0\n"
        "\tand.b\td1,d0\n"
        "\tbeq.b\t*+4\n"
        "\tmove.b\t-(a1),-(a0)\n"
        "\tsubq.l\t#1,d2\n"
        ";=barrier\n"
        "\tmove.l\ta1,d0\n"
        "\tand.b\td1,d0\n"
        "\tbeq.b\t*+16\n"
        "\tcmp.l\t#$10000,d2\n"
        "\tblo.b\t*+22\n"
        ";=barrier\n"
        "\tmove.b\t-(a1),-(a0)\n"
        "\tsubq.l\t#1,d2\n"
        "\tbne.b\t*-6\n"
        "\tbra.b\t*+24\n"
        ";=barrier\n"
        "\tmoveq\t#3,d1\n"
        "\tand.l\td2,d1\n"
        "\tsub.l\td1,d2\n"
        ";=barrier\n"
        "\tmove.l\t-(a1),-(a0)\n"
        "\tsubq.l\t#4,d2\n"
        "\tbne.b\t*-6\n"
        "\tmove.w\td1,d2\n"
        ";=barrier\n"
        "\tsubq.w\t#1,d2\n"
        "\tblo.b\t*+6\n"
        ";=barrier\n"
        "\tmove.b\t-(a1),-(a0)\n"
        "\tdbf\td2,*-4\n"
        ";=barrier\n"
        "\tmove.l\ta0,d0";

void *__asm_memset(__reg("a0") void *, __reg("d0") int, __reg("d2") size_t) =
        "\tmove.l\ta0,a1\n"
        "\tcmp.l\t#16,d2\n"
        "\tblo.b\t*+38\n"
        "\tmove.l\ta0,d1\n"
        "\tand.b\t#1,d1\n"
        "\tbeq.b\t*+4\n"
        "\tmove.b\td0,(a0)+\n"
        "\tsubq.l\t#1,d2\n"
        ";=barrier\n"
        "\tmove.b\td0,d1\n"
        "\tlsl.w\t#8,d0\n"
        "\tmove.b\td1,d0\n"
        "\tmove.w\td0,d1\n"
        "\tswap\td0\n"
        "\tmove.w\td1,d0\n"
        "\tmoveq\t#3,d1\n"
        "\tand.l\td2,d1\n"
        "\tsub.l\td1,d2\n"
        ";=barrier\n"
        "\tmove.l\td0,(a0)+\n"
        "\tsubq.l\t#4,d2\n"
        "\tbne.b\t*-6\n"
        "\tmove.w\td1,d2\n"
        ";=barrier\n"
        "\tsubq.w\t#1,d2\n"
        "\tblo.b\t*+6\n"
        ";=barrier\n"
        "\tmove.b\td0,(a0)+\n"
        "\tdbf\td2,*-4\n"
        ";=barrier\n"
        "\tmove.l\ta1,d0";

#else /* 68020+ */
void *__asm_memcpy(__reg("a0") void *, __reg("a1") const void *,
                   __reg("d2") size_t) =
        "\tmove.l\ta0,d0\n"
        "\tsubq.l\t#4,d2\n"
        "\tbcs.b\t*+30\n"
        "\tmove.l\td0,d1\n"
        "\tlsr.l\t#1,d1\n"
        "\tbcc.b\t*+8\n"
        "\tmove.b\t(a1)+,(a0)+\n"
        "\tsubq.l\t#1,d2\n"
        "\tbcs.b\t*+18\n"
        "\taddq.l\t#1,d1\n"
        ";=barrier\n"
        "\tlsr.l\t#1,d1\n"
        "\tbcc.b\t*+6\n"
        "\tmove.w\t(a1)+,(a0)+\n"
        "\tsubq.l\t#2,d2\n"
        "\tbcs.b\t*+6\n"
        ";=barrier\n"
        "\tmove.l\t(a1)+,(a0)+\n"
        "\tsubq.l\t#4,d2\n"
        "\tbcc.b\t*-6\n"
        ";=barrier\n"
        "\taddq.l\t#3,d2\n"
        "\tbcc.b\t*+6\n"
        ";=barrier\n"
        "\tmove.b\t(a1)+,(a0)+\n"
        "\tdbf\td2,*-4\n"
        ";=barrier";

void *__asm_memcpy_desc(__reg("a0") void *, __reg("a1") const void *,
                   __reg("d2") size_t) =
        "\tsubq.l\t#4,d2\n"
        "\tbcs.b\t*+28\n"
        "\tmove.l\ta0,d0\n"
        "\tlsr.l\t#1,d0\n"
        "\tbcc.b\t*+6\n"
        "\tmove.b\t-(a1),-(a0)\n"
        "\tsubq.l\t#1,d2\n"
        "\tbcs.b\t*+16\n"
        ";=barrier\n"
        "\tlsr.l\t#1,d0\n"
        "\tbcc.b\t*+6\n"
        "\tmove.w\t-(a1),-(a0)\n"
        "\tsubq.l\t#2,d2\n"
        "\tbcs.b\t*+6\n"
        ";=barrier\n"
        "\tmove.l\t-(a1),-(a0)\n"
        "\tsubq.l\t#4,d2\n"
        "\tbcc.b\t*-6\n"
        ";=barrier\n"
        "\taddq.l\t#3,d2\n"
        "\tbcc.b\t*+6\n"
        ";=barrier\n"
        "\tmove.b\t-(a1),-(a0)\n"
        "\tdbf\td2,*-4\n"
        ";=barrier\n"
        "\tmove.l\ta0,d0";

void *__asm_memset(__reg("a0") void *, __reg("d0") int, __reg("d2") size_t) =
        "\tmove.l\ta0,a1\n"
        "\tsubq.l\t#8,d2\n"
        "\tbcs.b\t*+46\n"
        "\tmove.l\td0,d1\n"
        "\tlsl.l\t#8,d0\n"
        "\tmove.b\td1,d0\n"
        "\taddq.l\t#4,d2\n"
        "\tmove.l\td0,d1\n"
        "\tswap\td0\n"
        "\tmove.w\td1,d0\n"
        "\tmove.l\ta1,d1\n"
        "\tlsr.l\t#1,d1\n"
        "\tbcc.b\t*+6\n"
        "\tmove.b\td0,(a0)+\n"
        "\taddq.l\t#1,d1\n"
        "\tsubq.l\t#1,d2\n"
        ";=barrier\n"
        "\tlsr.l\t#1,d1\n"
        "\tbcc.b\t*+4\n"
        "\tmove.w\td0,(a0)+\n"
        "\tsubq.l\t#2,d2\n"
        ";=barrier\n"
        "\tmove.l\td0,(a0)+\n"
        "\tsubq.l\t#4,d2\n"
        "\tbcc.b\t*-6\n"
        "\taddq.l\t#3,d2\n"
        "\tbcs.b\t*+6\n"
        "\tbra.b\t*+10\n"
        ";=barrier\n"
        "\taddq.l\t#7,d2\n"
        "\tbcc.b\t*+6\n"
        ";=barrier\n"
        "\tmove.b\td0,(a0)+\n"
        "\tdbf\td2,*-4\n"
        ";=barrier\n"
        "\tmove.l\ta1,d0";

#endif /* 68020+ */

size_t __asm_strlen(__reg("a0") const char *) =
        "\tmove.l\ta0,d0\n"
        ";=barrier\n"
        "\ttst.b\t(a0)+\n"
        "\tbne.b\t*-4\n"
        "\tsub.l\ta0,d0\n"
        "\tnot.l\td0";

char *__asm_strcpy(__reg("a0") char *, __reg("a1") const char *) =
        "\tmove.l\ta0,d0\n"
        ";=barrier\n"
        "\tmove.b\t(a1)+,(a0)+\n"
        "\tbne.b\t*-4";

char *__asm_strncpy(__reg("a0") char *, __reg("a1") const char *,
                    __reg("d1") size_t) =
        "\tmove.l\ta0,d0\n"
        "\tbra.b\t*+6\n"
        ";=barrier\n"
        "\tmove.b\t(a1),(a0)+\n"
        "\tbeq.b\t*+2\n"
        "\taddq.l\t#1,a1\n"
        ";=barrier\n"
        "\tsubq.l\t#1,d1\n"
        "\tbpl.b\t*-10";

int __asm_strcmp(__reg("a0") const char *, __reg("a1") const char *) =
        "\tmoveq\t#0,d0\n"
        "\tmoveq\t#0,d1\n"
        ";=barrier\n"
        "\tmove.b\t(a0)+,d0\n"
        "\tmove.b\t(a1)+,d1\n"
        "\tbeq.b\t*+6\n"
        "\tsub.l\td1,d0\n"
        "\tbeq.b\t*-10\n"
        "\tmoveq\t#0,d1\n"
        ";=barrier\n"
        "\tsub.l\td1,d0";

int __asm_strncmp(__reg("a0") const char *, __reg("a1") const char *,
                  __reg("d2") size_t) =
        "\tmoveq\t#0,d0\n"
        "\tmoveq\t#0,d1\n"
        ";=barrier\n"
        "\tsubq.l\t#1,d2\n"
        "\tbmi.b\t*+14\n"
        "\tmove.b\t(a0)+,d0\n"
        "\tmove.b\t(a1)+,d1\n"
        "\tbeq.b\t*+6\n"
        "\tsub.l\td1,d0\n"
        "\tbeq.b\t*-14\n"
        "\tmoveq\t#0,d1\n"
        ";=barrier\n"
        "\tsub.l\td1,d0";

char *__asm_strcat(__reg("a0") char *, __reg("a1") const char *) =
        "\tmove.l\ta0,d0\n"
        ";=barrier\n"
        "\ttst.b\t(a0)+\n"
        "\tbne.b\t*-4\n"
        "\tsubq.l\t#1,a0\n"
        ";=barrier\n"
        "\tmove.b\t(a1)+,(a0)+\n"
        "\tbne.b\t*-4";

void *__asm_strrchr(__reg("a0") char *, __reg("d1") int) =
        "\tmoveq\t#0,d0\n"
        ";=barrier\n"
        "\tcmp.b\t(a0),d1\n"
        "\tbne.b\t*+2\n"
        "\tmove.l\ta0,d0\n"
        ";=barrier\n"
        "\ttst.b\t(a0)+\n"
        "\tbne.b\t*-10";

#define memcpy(d,s,n) __asm_memcpy(d,s,n)
#define memmove(d,s,n) ((d)<=(s) ? __asm_memcpy(d,s,n) : \
                                   __asm_memcpy_desc((char *)(d)+(n),(char *)(s)+(n),n))
#define memset(p,f,n) __asm_memset(p,f,n)
#define strlen(p) __asm_strlen(p)
#define strcpy(d,s) __asm_strcpy(d,s)
#define strncpy(d,s,n) __asm_strncpy(d,s,n)
#define strcmp(s1,s2) __asm_strcmp(s1,s2)
#define strncmp(s1,s2,n) __asm_strncmp(s1,s2,n)
#define strcat(d,s) __asm_strcat(d,s)
#define strrchr(s,n) __asm_strrchr(s,n)

#endif /* __NOINLINE__ */

#endif
