/*  stdio.h header for vbcc Atari-ST version  */

#ifndef __STDIO_H
#define __STDIO_H 1

#define EOF (-1)

#define BUFSIZ 1024L            /*  Groesse des Buffers             */
#define FOPEN_MAX   1024        /*  is there an Atari limitation?   */

#define _IOFBF 1L               /*  Volles Buffering                */
#define _IOLBF 2L               /*  Zeilenbuffer                    */
#define _IONBF 3L               /*  kein Buffer                     */

#define HASBUF 1L               /*  hat einen Buffer                */
#define NOBUFMEM 2L             /*  kein Buffer belegbar            */
                                /*  Returncodes von setvbuf         */

#define SEEK_SET  0             /*  fseek relativ zu Dateibeginn    */
#define SEEK_CUR  1             /*                   akt. Pos.      */
#define SEEK_END  2             /*                   Dateiende      */

#define _READ 1                 /*  Flags fuer FILE.flags, Lesen    */
#define _WRITE 2
#define _UNBUF 4                /*  ungepuffert                     */
#define _EOF 8                  /*  Fileende                        */
#define _ERR 16                 /*  Fehler                          */
#define _READABLE 32
#define _WRITEABLE 64
#define _LINEBUF 128
#define _NOTMYBUF 256
#define _ISTTY 512
#define _CLE 16384              /*  convert line-end                */

typedef struct _iobuf           /*  FILE-Struktur                   */
{
    int filehandle;             /*  Atari-Filehandle                */
    char *pointer;              /*  Zeiger auf naechstes Zeichen    */
    char *base;                 /*  Adresse des Buffers             */
    struct _iobuf *next;        /*  Zeiger auf naechste Struktur    */
    struct _iobuf *prev;        /*  tja, was wohl...                */
    int count;                  /*  Anzahl Zeichen im Buffer        */
    int flags;                  /*  Zugriffsmode                    */
    int bufsize;                /*  Groesse des Buffers             */
} FILE;

/*  stdin/out/err muss von _main() initialisiert werden     */
extern FILE *stdin, *stdout, *stderr;

int _fillbuf(FILE *),_putbuf(int,FILE *),_flushbuf(FILE *);
void _ttyflush(void);


#define L_tmpnam        30
#define TMP_MAX FOPEN_MAX

#define FILENAME_MAX 107

#ifndef __SIZE_T
#define __SIZE_T 1
typedef unsigned long size_t;
#endif

#ifndef __FPOS_T
#define __FPOS_T 1
typedef long fpos_t;
#endif

#ifndef __STDARG_H
#include <stdarg.h>
#endif

#undef NULL
#define NULL ((void*)0)

FILE *fopen(const char *,const char *);
FILE *freopen(const char *,const char *,FILE *);
int fflush(FILE *);
int fclose(FILE *);
int rename(const char *,const char *);
int remove(const char *);
FILE *tmpfile(void);
char *tmpnam(char *);
int setvbuf(FILE *,char *,int,size_t);
void setbuf(FILE *,char *);
int fprintf(FILE *,const char *,...);
int printf(const char *,...);
int sprintf(char *,const char *,...);
int snprintf(char *,size_t,const char *,...);
int __v0fprintf(FILE *,const char *);
int __v0printf(const char *);
int __v0sprintf(char *,const char *);
int vprintf(const char *,va_list);
int vfprintf(FILE *,const char *,va_list);
int vsprintf(char *,const char *,va_list);
int vsnprintf(char *,size_t,const char *,va_list);
int fscanf(FILE *,const char *,...);
int scanf(const char *,...);
int sscanf(const char *,const char *,...);
int vscanf(const char *,va_list);
int vfscanf(FILE *,const char *,va_list);
int vsscanf(const char *,const char *,va_list);
char *fgets(char *,int,FILE *);
int fputs(const char *,FILE *);
char *gets(char *);
int puts(const char *);
int ungetc(int,FILE *);
size_t fread(void *,size_t,size_t,FILE *);
size_t fwrite(void *,size_t,size_t,FILE *);
int fseek(FILE *,long,int);
void rewind(FILE *);
long ftell(FILE *);
int fgetpos(FILE *,fpos_t *);
int fsetpos(FILE *,const fpos_t *);
void perror(const char *);
int fgetc(FILE *);
int fputc(int,FILE *);
int getchar(void);
int putchar(int);
int __putc(int,FILE *);
int __getc(FILE *);

#define __check(arg,type) (volatile size_t)sizeof((arg)==(type)0)

#define __rawputc(x,p) (((p)->flags|=_WRITE),((--((FILE*)(p))->count>=0&&((x)!='\n'||!(((FILE*)(p))->flags&_LINEBUF)))?(unsigned char)(*((FILE*)(p))->pointer++=(x)):_putbuf((x),p)))
#define putc(x,f) __putc((x),(f))
#define putchar(x) __putc((x),stdout)
#define __rawgetc(p) (__check((p),FILE*),((p)->flags|=_READ),--((FILE*)(p))->count>=0?(unsigned char)*((FILE*)(p))->pointer++:_fillbuf(p))
#define getc(f) __getc(f)
#define getchar() __getc(stdin)

/*  einige der Makros muessen wohl durch echte Funktionen ersetzt werden    */
#define feof(p)         (__check((p),FILE*),((FILE*)(p))->flags&_EOF)
#define ferror(p)       (__check((p),FILE*),((FILE*)(p))->flags&_ERR)
#define fileno(p)       ((p)->filehandle)
#define clearerr(p)     (__check((p),FILE*),((FILE*)(p))->flags&=~(_ERR|_EOF))

#define fsetpos(f,ptr)  fseek((f),*(ptr),SEEK_SET)

#pragma printflike printf
#pragma printflike fprintf
#pragma printflike sprintf
#pragma printflike snprintf
#pragma scanflike scanf
#pragma scanflike fscanf
#pragma scanflike sscanf

#ifdef __BUILD_LIB
#include <tos.h>
#define __read(x,y,z) Fread(x,(long)(z),y)
#define __write(x,y,z) Fwrite(x,(long)(z),y)
#define __seek(x,y,z) Fseek((long)(y),x,z)
extern int __isatty(int);
#endif


#endif
