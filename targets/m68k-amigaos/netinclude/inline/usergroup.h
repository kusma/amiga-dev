#ifndef _INLINE_USERGROUP_H
#define _INLINE_USERGROUP_H \
	"$Id: usergroup.h,v 4.3 1996/02/26 17:29:56 too Exp $"

#ifndef _CDEFS_H_
#include <sys/cdefs.h>
#endif
#ifndef _INLINE_STUBS_H_
#include <inline/stubs.h>
#endif

__BEGIN_DECLS

#ifndef BASE_EXT_DECL
#define BASE_EXT_DECL
#define BASE_EXT_DECL0 extern struct Library * UserGroupBase;
#endif
#ifndef BASE_PAR_DECL
#define BASE_PAR_DECL
#define BASE_PAR_DECL0 void
#endif
#ifndef BASE_NAME
#define BASE_NAME UserGroupBase
#endif

BASE_EXT_DECL0

extern __inline char *
crypt (BASE_PAR_DECL const char * key, const char * salt)
{
	BASE_EXT_DECL
	register char * res __asm("d0");
	register struct Library *a6 __asm("a6") = BASE_NAME;
	register const char * a0 __asm("a0") = key;
	register const char * a1 __asm("a1") = salt;
	__asm __volatile ("jsr a6@(-0xae)"
	: "=r" (res)
	: "r" (a6), "r" (a0), "r" (a1)
	: "d0", "d1", "a0", "a1");
	return res;
}
extern __inline void
endgrent (BASE_PAR_DECL0)
{
	BASE_EXT_DECL
	register struct Library *a6 __asm("a6") = BASE_NAME;
	__asm __volatile ("jsr a6@(-0xa8)"
	: /* No Output */
	: "r" (a6)
	: "d0", "d1", "a0", "a1");
}
extern __inline void
endpwent (BASE_PAR_DECL0)
{
	BASE_EXT_DECL
	register struct Library *a6 __asm("a6") = BASE_NAME;
	__asm __volatile ("jsr a6@(-0x8a)"
	: /* No Output */
	: "r" (a6)
	: "d0", "d1", "a0", "a1");
}
extern __inline void
endutent (BASE_PAR_DECL0)
{
	BASE_EXT_DECL
	register struct Library *a6 __asm("a6") = BASE_NAME;
	__asm __volatile ("jsr a6@(-0xf0)"
	: /* No Output */
	: "r" (a6)
	: "d0", "d1", "a0", "a1");
}
extern __inline struct UserGroupCredentials *
getcredentials (BASE_PAR_DECL struct Task * task)
{
	BASE_EXT_DECL
	register struct UserGroupCredentials * res __asm("d0");
	register struct Library *a6 __asm("a6") = BASE_NAME;
	register struct Task * a0 __asm("a0") = task;
	__asm __volatile ("jsr a6@(-0x102)"
	: "=r" (res)
	: "r" (a6), "r" (a0)
	: "d0", "d1", "a0", "a1");
	return res;
}
extern __inline gid_t
getegid (BASE_PAR_DECL0)
{
	BASE_EXT_DECL
	register res __asm("d0");
	register struct Library *a6 __asm("a6") = BASE_NAME;
	__asm __volatile ("jsr a6@(-0x4e)"
	: "=r" (res)
	: "r" (a6)
	: "d0", "d1", "a0", "a1");
	return res;
}
extern __inline uid_t
geteuid (BASE_PAR_DECL0)
{
	BASE_EXT_DECL
	register res __asm("d0");
	register struct Library *a6 __asm("a6") = BASE_NAME;
	__asm __volatile ("jsr a6@(-0x36)"
	: "=r" (res)
	: "r" (a6)
	: "d0", "d1", "a0", "a1");
	return res;
}
extern __inline gid_t
getgid (BASE_PAR_DECL0)
{
	BASE_EXT_DECL
	register res __asm("d0");
	register struct Library *a6 __asm("a6") = BASE_NAME;
	__asm __volatile ("jsr a6@(-0x48)"
	: "=r" (res)
	: "r" (a6)
	: "d0", "d1", "a0", "a1");
	return res;
}
extern __inline struct group *
getgrent (BASE_PAR_DECL0)
{
	BASE_EXT_DECL
	register struct group * res __asm("d0");
	register struct Library *a6 __asm("a6") = BASE_NAME;
	__asm __volatile ("jsr a6@(-0xa2)"
	: "=r" (res)
	: "r" (a6)
	: "d0", "d1", "a0", "a1");
	return res;
}
extern __inline struct group *
getgrgid (BASE_PAR_DECL gid_t gid)
{
	BASE_EXT_DECL
	register struct group * res __asm("d0");
	register struct Library *a6 __asm("a6") = BASE_NAME;
	register gid_t d0 __asm("d0") = gid;
	__asm __volatile ("jsr a6@(-0x96)"
	: "=r" (res)
	: "r" (a6), "r" (d0)
	: "d0", "d1", "a0", "a1");
	return res;
}
extern __inline struct group *
getgrnam (BASE_PAR_DECL const char * name)
{
	BASE_EXT_DECL
	register struct group * res __asm("d0");
	register struct Library *a6 __asm("a6") = BASE_NAME;
	register const char * a1 __asm("a1") = name;
	__asm __volatile ("jsr a6@(-0x90)"
	: "=r" (res)
	: "r" (a6), "r" (a1)
	: "d0", "d1", "a0", "a1");
	return res;
}
extern __inline int
getgroups (BASE_PAR_DECL int ngroups, gid_t * groups)
{
	BASE_EXT_DECL
	register res __asm("d0");
	register struct Library *a6 __asm("a6") = BASE_NAME;
	register int d0 __asm("d0") = ngroups;
	register gid_t * a1 __asm("a1") = groups;
	__asm __volatile ("jsr a6@(-0x60)"
	: "=r" (res)
	: "r" (a6), "r" (d0), "r" (a1)
	: "d0", "d1", "a0", "a1");
	return res;
}
extern __inline struct lastlog *
getlastlog (BASE_PAR_DECL uid_t uid)
{
	BASE_EXT_DECL
	register struct lastlog * res __asm("d0");
	register struct Library *a6 __asm("a6") = BASE_NAME;
	register uid_t d0 __asm("d0") = uid;
	__asm __volatile ("jsr a6@(-0xf6)"
	: "=r" (res)
	: "r" (a6), "r" (d0)
	: "d0", "d1", "a0", "a1");
	return res;
}
extern __inline char *
getlogin (BASE_PAR_DECL0)
{
	BASE_EXT_DECL
	register char * res __asm("d0");
	register struct Library *a6 __asm("a6") = BASE_NAME;
	__asm __volatile ("jsr a6@(-0xd8)"
	: "=r" (res)
	: "r" (a6)
	: "d0", "d1", "a0", "a1");
	return res;
}
extern __inline char *
getpass (BASE_PAR_DECL const char * prompt)
{
	BASE_EXT_DECL
	register char * res __asm("d0");
	register struct Library *a6 __asm("a6") = BASE_NAME;
	register const char * a1 __asm("a1") = prompt;
	__asm __volatile ("jsr a6@(-0xba)"
	: "=r" (res)
	: "r" (a6), "r" (a1)
	: "d0", "d1", "a0", "a1");
	return res;
}
extern __inline pid_t
getpgrp (BASE_PAR_DECL0)
{
	BASE_EXT_DECL
	register pid_t res __asm("d0");
	register struct Library *a6 __asm("a6") = BASE_NAME;
	__asm __volatile ("jsr a6@(-0xd2)"
	: "=r" (res)
	: "r" (a6)
	: "d0", "d1", "a0", "a1");
	return res;
}
extern __inline struct passwd *
getpwent (BASE_PAR_DECL0)
{
	BASE_EXT_DECL
	register struct passwd * res __asm("d0");
	register struct Library *a6 __asm("a6") = BASE_NAME;
	__asm __volatile ("jsr a6@(-0x84)"
	: "=r" (res)
	: "r" (a6)
	: "d0", "d1", "a0", "a1");
	return res;
}
extern __inline struct passwd *
getpwnam (BASE_PAR_DECL const char * name)
{
	BASE_EXT_DECL
	register struct passwd * res __asm("d0");
	register struct Library *a6 __asm("a6") = BASE_NAME;
	register const char * a1 __asm("a1") = name;
	__asm __volatile ("jsr a6@(-0x72)"
	: "=r" (res)
	: "r" (a6), "r" (a1)
	: "d0", "d1", "a0", "a1");
	return res;
}
extern __inline struct passwd *
getpwuid (BASE_PAR_DECL uid_t uid)
{
	BASE_EXT_DECL
	register struct passwd * res __asm("d0");
	register struct Library *a6 __asm("a6") = BASE_NAME;
	register uid_t d0 __asm("d0") = uid;
	__asm __volatile ("jsr a6@(-0x78)"
	: "=r" (res)
	: "r" (a6), "r" (d0)
	: "d0", "d1", "a0", "a1");
	return res;
}
extern __inline uid_t
getuid (BASE_PAR_DECL0)
{
	BASE_EXT_DECL
	register uid_t res __asm("d0");
	register struct Library *a6 __asm("a6") = BASE_NAME;
	__asm __volatile ("jsr a6@(-0x30)"
	: "=r" (res)
	: "r" (a6)
	: "d0", "d1", "a0", "a1");
	return res;
}
extern __inline mode_t
getumask (BASE_PAR_DECL0)
{
	BASE_EXT_DECL
	register res __asm("d0");
	register struct Library *a6 __asm("a6") = BASE_NAME;
	__asm __volatile ("jsr a6@(-0xc6)"
	: "=r" (res)
	: "r" (a6)
	: "d0", "d1", "a0", "a1");
	return res;
}
extern __inline struct utmp *
getutent (BASE_PAR_DECL0)
{
	BASE_EXT_DECL
	register struct utmp * res __asm("d0");
	register struct Library *a6 __asm("a6") = BASE_NAME;
	__asm __volatile ("jsr a6@(-0xea)"
	: "=r" (res)
	: "r" (a6)
	: "d0", "d1", "a0", "a1");
	return res;
}
extern __inline int
initgroups (BASE_PAR_DECL const char * name, gid_t basegroup)
{
	BASE_EXT_DECL
	register res __asm("d0");
	register struct Library *a6 __asm("a6") = BASE_NAME;
	register const char * a1 __asm("a1") = name;
	register gid_t d0 __asm("d0") = basegroup;
	__asm __volatile ("jsr a6@(-0x6c)"
	: "=r" (res)
	: "r" (a6), "r" (a1), "r" (d0)
	: "d0", "d1", "a0", "a1");
	return res;
}
extern __inline int
setgid (BASE_PAR_DECL gid_t id)
{
	BASE_EXT_DECL
	register res __asm("d0");
	register struct Library *a6 __asm("a6") = BASE_NAME;
	register gid_t d0 __asm("d0") = id;
	__asm __volatile ("jsr a6@(-0x5a)"
	: "=r" (res)
	: "r" (a6), "r" (d0)
	: "d0", "d1", "a0", "a1");
	return res;
}
extern __inline void
setgrent (BASE_PAR_DECL0)
{
	BASE_EXT_DECL
	register struct Library *a6 __asm("a6") = BASE_NAME;
	__asm __volatile ("jsr a6@(-0x9c)"
	: /* No Output */
	: "r" (a6)
	: "d0", "d1", "a0", "a1");
}
extern __inline int
setgroups (BASE_PAR_DECL int ngroups, const gid_t * groups)
{
	BASE_EXT_DECL
	register res __asm("d0");
	register struct Library *a6 __asm("a6") = BASE_NAME;
	register int d0 __asm("d0") = ngroups;
	register const gid_t * a1 __asm("a1") = groups;
	__asm __volatile ("jsr a6@(-0x66)"
	: "=r" (res)
	: "r" (a6), "r" (d0), "r" (a1)
	: "d0", "d1", "a0", "a1");
	return res;
}
extern __inline int
setlastlog (BASE_PAR_DECL uid_t uid, char * name, char * host)
{
	BASE_EXT_DECL
	register res __asm("d0");
	register struct Library *a6 __asm("a6") = BASE_NAME;
	register uid_t d0 __asm("d0") = uid;
	register char * a0 __asm("a0") = name;
	register char * a1 __asm("a1") = host;
	__asm __volatile ("jsr a6@(-0xfc)"
	: "=r" (res)
	: "r" (a6), "r" (d0), "r" (a0), "r" (a1)
	: "d0", "d1", "a0", "a1");
	return res;
}
extern __inline int
setlogin (BASE_PAR_DECL const char * buffer)
{
	BASE_EXT_DECL
	register res __asm("d0");
	register struct Library *a6 __asm("a6") = BASE_NAME;
	register const char * a1 __asm("a1") = buffer;
	__asm __volatile ("jsr a6@(-0xde)"
	: "=r" (res)
	: "r" (a6), "r" (a1)
	: "d0", "d1", "a0", "a1");
	return res;
}
extern __inline void
setpwent (BASE_PAR_DECL0)
{
	BASE_EXT_DECL
	register struct Library *a6 __asm("a6") = BASE_NAME;
	__asm __volatile ("jsr a6@(-0x7e)"
	: /* No Output */
	: "r" (a6)
	: "d0", "d1", "a0", "a1");
}
extern __inline int
setregid (BASE_PAR_DECL gid_t real, gid_t eff)
{
	BASE_EXT_DECL
	register res __asm("d0");
	register struct Library *a6 __asm("a6") = BASE_NAME;
	register gid_t d0 __asm("d0") = real;
	register gid_t d1 __asm("d1") = eff;
	__asm __volatile ("jsr a6@(-0x54)"
	: "=r" (res)
	: "r" (a6), "r" (d0), "r" (d1)
	: "d0", "d1", "a0", "a1");
	return res;
}
extern __inline int
setreuid (BASE_PAR_DECL uid_t real, uid_t eff)
{
	BASE_EXT_DECL
	register res __asm("d0");
	register struct Library *a6 __asm("a6") = BASE_NAME;
	register uid_t d0 __asm("d0") = real;
	register uid_t d1 __asm("d1") = eff;
	__asm __volatile ("jsr a6@(-0x3c)"
	: "=r" (res)
	: "r" (a6), "r" (d0), "r" (d1)
	: "d0", "d1", "a0", "a1");
	return res;
}
extern __inline pid_t
setsid (BASE_PAR_DECL0)
{
	BASE_EXT_DECL
	register pid_t res __asm("d0");
	register struct Library *a6 __asm("a6") = BASE_NAME;
	__asm __volatile ("jsr a6@(-0xcc)"
	: "=r" (res)
	: "r" (a6)
	: "d0", "d1", "a0", "a1");
	return res;
}
extern __inline int
setuid (BASE_PAR_DECL uid_t id)
{
	BASE_EXT_DECL
	register res __asm("d0");
	register struct Library *a6 __asm("a6") = BASE_NAME;
	register uid_t d0 __asm("d0") = id;
	__asm __volatile ("jsr a6@(-0x42)"
	: "=r" (res)
	: "r" (a6), "r" (d0)
	: "d0", "d1", "a0", "a1");
	return res;
}
extern __inline void
setutent (BASE_PAR_DECL0)
{
	BASE_EXT_DECL
	register struct Library *a6 __asm("a6") = BASE_NAME;
	__asm __volatile ("jsr a6@(-0xe4)"
	: /* No Output */
	: "r" (a6)
	: "d0", "d1", "a0", "a1");
}
extern __inline int
ug_GetErr (BASE_PAR_DECL0)
{
	BASE_EXT_DECL
	register res __asm("d0");
	register struct Library *a6 __asm("a6") = BASE_NAME;
	__asm __volatile ("jsr a6@(-0x24)"
	: "=r" (res)
	: "r" (a6)
	: "d0", "d1", "a0", "a1");
	return res;
}
extern __inline char *
ug_GetSalt (BASE_PAR_DECL const struct passwd * user, char * buffer, ULONG size)
{
	BASE_EXT_DECL
	register char * res __asm("d0");
	register struct Library *a6 __asm("a6") = BASE_NAME;
	register const struct passwd * a0 __asm("a0") = user;
	register char * a1 __asm("a1") = buffer;
	register ULONG d0 __asm("d0") = size;
	__asm __volatile ("jsr a6@(-0xb4)"
	: "=r" (res)
	: "r" (a6), "r" (a0), "r" (a1), "r" (d0)
	: "d0", "d1", "a0", "a1");
	return res;
}
extern __inline int
ug_SetupContextTagList (BASE_PAR_DECL const UBYTE* pname, struct TagItem * taglist)
{
	BASE_EXT_DECL
	register res __asm("d0");
	register struct Library *a6 __asm("a6") = BASE_NAME;
	register const UBYTE* a0 __asm("a0") = pname;
	register struct TagItem * a1 __asm("a1") = taglist;
	__asm __volatile ("jsr a6@(-0x1e)"
	: "=r" (res)
	: "r" (a6), "r" (a0), "r" (a1)
	: "d0", "d1", "a0", "a1");
	return res;
}
extern __inline const char *
ug_StrError (BASE_PAR_DECL LONG code)
{
	BASE_EXT_DECL
	register char * res __asm("d0");
	register struct Library *a6 __asm("a6") = BASE_NAME;
	register LONG d1 __asm("d1") = code;
	__asm __volatile ("jsr a6@(-0x2a)"
	: "=r" (res)
	: "r" (a6), "r" (d1)
	: "d0", "d1", "a0", "a1");
	return res;
}
extern __inline mode_t
umask (BASE_PAR_DECL mode_t mask)
{
	BASE_EXT_DECL
	register res __asm("d0");
	register struct Library *a6 __asm("a6") = BASE_NAME;
	register mode_t d0 __asm("d0") = mask;
	__asm __volatile ("jsr a6@(-0xc0)"
	: "=r" (res)
	: "r" (a6), "r" (d0)
	: "d0", "d1", "a0", "a1");
	return res;
}
#undef BASE_EXT_DECL
#undef BASE_EXT_DECL0
#undef BASE_PAR_DECL
#undef BASE_PAR_DECL0
#undef BASE_NAME

__END_DECLS

#endif /* _INLINE_USERGROUP_H */
