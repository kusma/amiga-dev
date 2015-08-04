/* mswindows.c -- Windows-specific support
   Copyright (C) 1995, 1996, 1997, 1998, 2004
   Free Software Foundation, Inc.

This file is part of GNU Wget.

GNU Wget is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

GNU Wget is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Wget; if not, write to the Free Software
Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

In addition, as a special exception, the Free Software Foundation
gives permission to link the code of its release of Wget with the
OpenSSL project's "OpenSSL" library (or with modified versions of it
that use the same license as the "OpenSSL" library), and distribute
the linked executables.  You must obey the GNU General Public License
in all respects for all of the code used other than "OpenSSL".  If you
modify this file, you may extend this exception to your version of the
file, but you are not obligated to do so.  If you do not wish to do
so, delete this exception statement from your version.  */

#include <config.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <errno.h>
#include <math.h>

#ifdef HACK_BCC_UTIME_BUG
# include <io.h>
# include <fcntl.h>
# ifdef HAVE_UTIME_H
#  include <utime.h>
# endif
# ifdef HAVE_SYS_UTIME_H
#  include <sys/utime.h>
# endif
#endif

#define INHIBIT_WRAP /* avoid wrapping of socket, bind, ... */

#include "wget.h"
#include "utils.h"
#include "url.h"

#ifndef errno
extern int errno;
#endif

#ifndef ES_SYSTEM_REQUIRED
#define ES_SYSTEM_REQUIRED  0x00000001
#endif

#ifndef ES_CONTINUOUS
#define ES_CONTINUOUS       0x80000000
#endif


/* Defined in log.c.  */
void log_request_redirect_output PARAMS ((const char *));

/* Windows version of xsleep in utils.c.  */

void
xsleep (double seconds)
{
#ifdef HAVE_USLEEP
  if (seconds > 1000)
    {
      /* Explained in utils.c. */
      sleep (seconds);
      seconds -= (long) seconds;
    }
  usleep (seconds * 1000000L);
#else  /* not HAVE_USLEEP */
  SleepEx ((DWORD) (seconds * 1000 + .5), FALSE);
#endif /* not HAVE_USLEEP */
}

void
windows_main (int *argc, char **argv, char **exec_name)
{
  char *p;

  /* Remove .EXE from filename if it has one.  */
  *exec_name = xstrdup (*exec_name);
  p = strrchr (*exec_name, '.');
  if (p)
    *p = '\0';
}

static void
ws_cleanup (void)
{
  WSACleanup ();
}

static void
ws_hangup (const char *reason)
{
  fprintf (stderr, _("Continuing in background.\n"));
  log_request_redirect_output (reason);

  /* Detach process from the current console.  Under Windows 9x, if we
     were launched from a 16-bit process (which is usually the case;
     command.com is 16-bit) the parent process should resume right away.
     Under NT or if launched from a 32-process under 9x, this is a futile
     gesture as the parent will wait for us to terminate before resuming.  */
  FreeConsole ();
}

/* Construct the name for a named section (a.k.a. `file mapping') object.
   The returned string is dynamically allocated and needs to be xfree()'d.  */
static char *
make_section_name (DWORD pid)
{
  return aprintf ("gnu_wget_fake_fork_%lu", pid);
}

/* This structure is used to hold all the data that is exchanged between
   parent and child.  */
struct fake_fork_info
{
  HANDLE event;
  int logfile_changed;
  char lfilename[MAX_PATH + 1];
};

/* Determines if we are the child and if so performs the child logic.
   Return values:
     < 0  error
       0  parent
     > 0  child
*/
static int
fake_fork_child (void)
{
  HANDLE section, event;
  struct fake_fork_info *info;
  char *name;

  name = make_section_name (GetCurrentProcessId ());
  section = OpenFileMapping (FILE_MAP_WRITE, FALSE, name);
  xfree (name);
  /* It seems that Windows 9x and NT set last-error inconsistently when
     OpenFileMapping() fails; so we assume it failed because the section
     object does not exist.  */
  if (!section)
    return 0;                   /* We are the parent.  */

  info = MapViewOfFile (section, FILE_MAP_WRITE, 0, 0, 0);
  if (!info)
    {
      CloseHandle (section);
      return -1;
    }

  event = info->event;

  info->logfile_changed = 0;
  if (!opt.lfilename)
    {
      /* See utils:fork_to_background for explanation. */
      FILE *new_log_fp = unique_create (DEFAULT_LOGFILE, 0, &opt.lfilename);
      if (new_log_fp)
	{
	  info->logfile_changed = 1;
	  strncpy (info->lfilename, opt.lfilename, sizeof (info->lfilename));
	  info->lfilename[sizeof (info->lfilename) - 1] = '\0';
	  fclose (new_log_fp);
	}
    }

  UnmapViewOfFile (info);
  CloseHandle (section);

  /* Inform the parent that we've done our part.  */
  if (!SetEvent (event))
    return -1;

  CloseHandle (event);
  return 1;                     /* We are the child.  */
}

/* Windows doesn't support the fork() call; so we fake it by invoking
   another copy of Wget with the same arguments with which we were
   invoked.  The child copy of Wget should perform the same initialization
   sequence as the parent; so we should have two processes that are
   essentially identical.  We create a specially named section object that
   allows the child to distinguish itself from the parent and is used to
   exchange information between the two processes.  We use an event object
   for synchronization.  */
static void
fake_fork (void)
{
  char exe[MAX_PATH + 1];
  DWORD exe_len, le;
  SECURITY_ATTRIBUTES sa;
  HANDLE section, event, h[2];
  STARTUPINFO si;
  PROCESS_INFORMATION pi;
  struct fake_fork_info *info;
  char *name;
  BOOL rv;

  section = pi.hProcess = pi.hThread = NULL;

  /* Get the fully qualified name of our executable.  This is more reliable
     than using argv[0].  */
  exe_len = GetModuleFileName (GetModuleHandle (NULL), exe, sizeof (exe));
  if (!exe_len || (exe_len >= sizeof (exe)))
    return;

  sa.nLength = sizeof (sa);
  sa.lpSecurityDescriptor = NULL;
  sa.bInheritHandle = TRUE;

  /* Create an anonymous inheritable event object that starts out
     non-signaled.  */
  event = CreateEvent (&sa, FALSE, FALSE, NULL);
  if (!event)
    return;

  /* Create the child process detached form the current console and in a
     suspended state.  */
  xzero (si);
  si.cb = sizeof (si);
  rv = CreateProcess (exe, GetCommandLine (), NULL, NULL, TRUE,
                      CREATE_SUSPENDED | DETACHED_PROCESS,
                      NULL, NULL, &si, &pi);
  if (!rv)
    goto cleanup;

  /* Create a named section object with a name based on the process id of
     the child.  */
  name = make_section_name (pi.dwProcessId);
  section =
      CreateFileMapping (INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0,
                         sizeof (struct fake_fork_info), name);
  le = GetLastError();
  xfree (name);
  /* Fail if the section object already exists (should not happen).  */
  if (!section || (le == ERROR_ALREADY_EXISTS))
    {
      rv = FALSE;
      goto cleanup;
    }

  /* Copy the event handle into the section object.  */
  info = MapViewOfFile (section, FILE_MAP_WRITE, 0, 0, 0);
  if (!info)
    {
      rv = FALSE;
      goto cleanup;
    }

  info->event = event;

  UnmapViewOfFile (info);

  /* Start the child process.  */
  rv = ResumeThread (pi.hThread);
  if (!rv)
    {
      TerminateProcess (pi.hProcess, (DWORD) -1);
      goto cleanup;
    }

  /* Wait for the child to signal to us that it has done its part.  If it
     terminates before signaling us it's an error.  */

  h[0] = event;
  h[1] = pi.hProcess;
  rv = WAIT_OBJECT_0 == WaitForMultipleObjects (2, h, FALSE, 5 * 60 * 1000);
  if (!rv)
    goto cleanup;

  info = MapViewOfFile (section, FILE_MAP_READ, 0, 0, 0);
  if (!info)
    {
      rv = FALSE;
      goto cleanup;
    }

  /* Ensure string is properly terminated.  */
  if (info->logfile_changed &&
      !memchr (info->lfilename, '\0', sizeof (info->lfilename)))
    {
      rv = FALSE;
      goto cleanup;
    }

  printf (_("Continuing in background, pid %lu.\n"), pi.dwProcessId);
  if (info->logfile_changed)
    printf (_("Output will be written to `%s'.\n"), info->lfilename);

  UnmapViewOfFile (info);

cleanup:

  if (event)
    CloseHandle (event);
  if (section)
    CloseHandle (section);
  if (pi.hThread)
    CloseHandle (pi.hThread);
  if (pi.hProcess)
    CloseHandle (pi.hProcess);

  /* We're the parent.  If all is well, terminate.  */
  if (rv)
    exit (0);

  /* We failed, return.  */
}

/* This is the corresponding Windows implementation of the
   fork_to_background() function in utils.c.  */
void
fork_to_background (void)
{
  int rv;

  rv = fake_fork_child ();
  if (rv < 0)
    {
      fprintf (stderr, "fake_fork_child() failed\n");
      abort ();
    }
  else if (rv == 0)
    {
      /* We're the parent.  */
      fake_fork ();
      /* If fake_fork() returns, it failed.  */
      fprintf (stderr, "fake_fork() failed\n");
      abort ();
    }
  /* If we get here, we're the child.  */
}

static BOOL WINAPI
ws_handler (DWORD dwEvent)
{
  switch (dwEvent)
    {
#ifdef CTRLC_BACKGND
    case CTRL_C_EVENT:
      ws_hangup ("CTRL+C");
      return TRUE;
#endif
#ifdef CTRLBREAK_BACKGND
    case CTRL_BREAK_EVENT:
      ws_hangup ("CTRL+Break");
      return TRUE;
#endif
    default:
      return FALSE;
    }
}

static char *title_buf = NULL;
static char *curr_url  = NULL;
static int old_percentage = -1;

/* Updates the console title with the URL of the current file being
   transferred.  */
void
ws_changetitle (const char *url)
{
  xfree_null (title_buf);
  xfree_null (curr_url);
  title_buf = (char *)xmalloc (strlen (url) + 20);
  curr_url = xstrdup (url);
  old_percentage = -1;
  sprintf (title_buf, "Wget %s", curr_url);
  SetConsoleTitle (title_buf);
}

/* Updates the console title with the percentage of the current file
   transferred.  */
void
ws_percenttitle (double percentage_float)
{
  int percentage;

  if (!title_buf || !curr_url)
    return;

  percentage = (int) percentage_float;

  /* Clamp percentage value.  */
  if (percentage < 0)
    percentage = 0;
  if (percentage > 100)
    percentage = 100;

  /* Only update the title when the percentage has changed.  */
  if (percentage == old_percentage)
    return;

  old_percentage = percentage;

  sprintf (title_buf, "Wget [%d%%] %s", percentage, curr_url);
  SetConsoleTitle (title_buf);
}

/* Returns a pointer to the fully qualified name of the directory that
   contains the Wget binary (wget.exe).  The returned path does not have a
   trailing path separator.  Returns NULL on failure.  */
char *
ws_mypath (void)
{
  static char *wspathsave = NULL;

  if (!wspathsave)
    {
      char buf[MAX_PATH + 1];
      char *p;
      DWORD len;

      len = GetModuleFileName (GetModuleHandle (NULL), buf, sizeof (buf));
      if (!len || (len >= sizeof (buf)))
        return NULL;

      p = strrchr (buf, PATH_SEPARATOR);
      if (!p)
        return NULL;

      *p = '\0';
      wspathsave = xstrdup (buf);
    }

  return wspathsave;
}

/* Prevent Windows entering sleep/hibernation-mode while Wget is doing
   a lengthy transfer.  Windows does not, by default, consider network
   activity in console-programs as activity!  Works on Win-98/ME/2K
   and up.  */
static void
set_sleep_mode (void)
{
  typedef DWORD (WINAPI *func_t) (DWORD);
  func_t set_exec_state;

  set_exec_state =
      (func_t) GetProcAddress (GetModuleHandle ("KERNEL32.DLL"),
                               "SetThreadExecutionState");

  if (set_exec_state)
    set_exec_state (ES_SYSTEM_REQUIRED | ES_CONTINUOUS);
}

/* Perform Windows specific initialization.  */
void
ws_startup (void)
{
  WSADATA data;
  WORD requested = MAKEWORD (1, 1);
  int err = WSAStartup (requested, &data);
  if (err != 0)
    {
      fprintf (stderr, _("%s: Couldn't find usable socket driver.\n"),
	       exec_name);
      exit (1);
    }

  if (data.wVersion < requested)
    {
      fprintf (stderr, _("%s: Couldn't find usable socket driver.\n"),
	       exec_name);
      WSACleanup ();
      exit (1);
    }

  atexit (ws_cleanup);
  set_sleep_mode ();
  SetConsoleCtrlHandler (ws_handler, TRUE);
}

/* Replacement utime function for buggy Borland C++Builder 5.5 compiler.
   (The Borland utime function only works on Windows NT.)  */

#ifdef HACK_BCC_UTIME_BUG
int
borland_utime (const char *path, const struct utimbuf *times)
{
  int fd;
  int res;
  struct ftime ft;
  struct tm *ptr_tm;

  if ((fd = open (path, O_RDWR)) < 0)
    return -1;

  ptr_tm = localtime (&times->modtime);
  ft.ft_tsec = ptr_tm->tm_sec >> 1;
  ft.ft_min = ptr_tm->tm_min;
  ft.ft_hour = ptr_tm->tm_hour;
  ft.ft_day = ptr_tm->tm_mday;
  ft.ft_month = ptr_tm->tm_mon + 1;
  ft.ft_year = ptr_tm->tm_year - 80;
  res = setftime (fd, &ft);
  close (fd);
  return res;
}
#endif

/* run_with_timeout Windows implementation.  */

/* Stack size 0 uses default thread stack-size (reserve+commit).
   Determined by what's in the PE header.  */
#define THREAD_STACK_SIZE  0

struct thread_data
{
  void (*fun) (void *);
  void *arg;
  DWORD ws_error;
};

/* The callback that runs FUN(ARG) in a separate thread.  This
   function exists for two reasons: a) to not require FUN to be
   declared WINAPI/__stdcall[1], and b) to retrieve Winsock errors,
   which are per-thread.  The latter is useful when FUN calls Winsock
   functions, which is how run_with_timeout is used in Wget.

   [1] MSVC can use __fastcall globally (cl /Gr) and on Watcom this is
   the default (wcc386 -3r).  */

static DWORD WINAPI
thread_helper (void *arg)
{
  struct thread_data *td = (struct thread_data *) arg;

  /* Initialize Winsock error to what it was in the parent.  That way
     the subsequent call to WSAGetLastError will return the same value
     if td->fun doesn't change Winsock error state.  */
  WSASetLastError (td->ws_error);

  td->fun (td->arg);

  /* Return Winsock error to the caller, in case FUN ran Winsock
     code.  */
  td->ws_error = WSAGetLastError ();
  return 0;
}

/* Call FUN(ARG), but don't allow it to run for more than TIMEOUT
   seconds.  Returns non-zero if the function was interrupted with a
   timeout, zero otherwise.

   This works by running FUN in a separate thread and terminating the
   thread if it doesn't finish in the specified time.  */

int
run_with_timeout (double seconds, void (*fun) (void *), void *arg)
{
  static HANDLE thread_hnd = NULL;
  struct thread_data thread_arg;
  DWORD thread_id;
  int rc;

  DEBUGP (("seconds %.2f, ", seconds));

  if (seconds == 0)
    {
    blocking_fallback:
      fun (arg);
      return 0;
    }

  /* Should never happen, but test for recursivety anyway.  */
  assert (thread_hnd == NULL);

  thread_arg.fun = fun;
  thread_arg.arg = arg;
  thread_arg.ws_error = WSAGetLastError ();
  thread_hnd = CreateThread (NULL, THREAD_STACK_SIZE, thread_helper,
			     &thread_arg, 0, &thread_id);
  if (!thread_hnd)
    {
      DEBUGP (("CreateThread() failed; %s\n", strerror (GetLastError ())));
      goto blocking_fallback;
    }

  if (WaitForSingleObject (thread_hnd, (DWORD)(1000 * seconds))
      == WAIT_OBJECT_0)
    {
      /* Propagate error state (which is per-thread) to this thread,
	 so the caller can inspect it.  */
      WSASetLastError (thread_arg.ws_error);
      DEBUGP (("Winsock error: %d\n", WSAGetLastError ()));
      rc = 0;
    }
  else
    {
      TerminateThread (thread_hnd, 1);
      rc = 1;
    }

  CloseHandle (thread_hnd);	/* Clear-up after TerminateThread().  */
  thread_hnd = NULL;
  return rc;
}

/* Wget expects network calls such as connect, recv, send, etc., to set
   errno on failure.  To achieve that, Winsock calls are wrapped with code
   that, in case of error, sets errno to the value of WSAGetLastError().
   In addition, we provide a wrapper around strerror, which recognizes
   Winsock errors and prints the appropriate error message. */

/* Define a macro that creates a function definition that wraps FUN into
   a function that sets errno the way the rest of the code expects. */

#define WRAP(fun, decl, call) int wrapped_##fun decl {	\
  int retval = fun call;				\
  if (retval < 0)					\
    errno = WSAGetLastError ();				\
  return retval;					\
}

WRAP (socket, (int domain, int type, int protocol), (domain, type, protocol))
WRAP (bind, (int s, struct sockaddr *a, int alen), (s, a, alen))
WRAP (connect, (int s, const struct sockaddr *a, int alen), (s, a, alen))
WRAP (listen, (int s, int backlog), (s, backlog))
WRAP (accept, (int s, struct sockaddr *a, int *alen), (s, a, alen))
WRAP (recv, (int s, void *buf, int len, int flags), (s, buf, len, flags))
WRAP (send, (int s, const void *buf, int len, int flags), (s, buf, len, flags))
WRAP (select, (int n, fd_set *r, fd_set *w, fd_set *e, const struct timeval *tm),
              (n, r, w, e, tm))
WRAP (getsockname, (int s, struct sockaddr *n, int *nlen), (s, n, nlen))
WRAP (getpeername, (int s, struct sockaddr *n, int *nlen), (s, n, nlen))
WRAP (setsockopt, (int s, int level, int opt, const void *val, int len),
                  (s, level, opt, val, len))
WRAP (closesocket, (int s), (s))

/* Return the text of the error message for Winsock error WSERR. */

static const char *
get_winsock_error (int wserr)
{
  switch (wserr) {
  case WSAEINTR:           return "Interrupted system call";
  case WSAEBADF:           return "Bad file number";
  case WSAEACCES:          return "Permission denied";
  case WSAEFAULT:          return "Bad address";
  case WSAEINVAL:          return "Invalid argument";
  case WSAEMFILE:          return "Too many open files";
  case WSAEWOULDBLOCK:     return "Resource temporarily unavailable";
  case WSAEINPROGRESS:     return "Operation now in progress";
  case WSAEALREADY:        return "Operation already in progress";
  case WSAENOTSOCK:        return "Socket operation on nonsocket";
  case WSAEDESTADDRREQ:    return "Destination address required";
  case WSAEMSGSIZE:        return "Message too long";
  case WSAEPROTOTYPE:      return "Protocol wrong type for socket";
  case WSAENOPROTOOPT:     return "Bad protocol option";
  case WSAEPROTONOSUPPORT: return "Protocol not supported";
  case WSAESOCKTNOSUPPORT: return "Socket type not supported";
  case WSAEOPNOTSUPP:      return "Operation not supported";
  case WSAEPFNOSUPPORT:    return "Protocol family not supported";
  case WSAEAFNOSUPPORT:    return "Address family not supported by protocol family";
  case WSAEADDRINUSE:      return "Address already in use";
  case WSAEADDRNOTAVAIL:   return "Cannot assign requested address";
  case WSAENETDOWN:        return "Network is down";
  case WSAENETUNREACH:     return "Network is unreachable";
  case WSAENETRESET:       return "Network dropped connection on reset";
  case WSAECONNABORTED:    return "Software caused connection abort";
  case WSAECONNRESET:      return "Connection reset by peer";
  case WSAENOBUFS:         return "No buffer space available";
  case WSAEISCONN:         return "Socket is already connected";
  case WSAENOTCONN:        return "Socket is not connected";
  case WSAESHUTDOWN:       return "Cannot send after socket shutdown";
  case WSAETOOMANYREFS:    return "Too many references";
  case WSAETIMEDOUT:       return "Connection timed out";
  case WSAECONNREFUSED:    return "Connection refused";
  case WSAELOOP:           return "Too many levels of symbolic links";
  case WSAENAMETOOLONG:    return "File name too long";
  case WSAEHOSTDOWN:       return "Host is down";
  case WSAEHOSTUNREACH:    return "No route to host";
  case WSAENOTEMPTY:       return "Not empty";
  case WSAEPROCLIM:        return "Too many processes";
  case WSAEUSERS:          return "Too many users";
  case WSAEDQUOT:          return "Bad quota";
  case WSAESTALE:          return "Something is stale";
  case WSAEREMOTE:         return "Remote error";
  case WSAEDISCON:         return "Disconnected";

  /* Extended Winsock errors */
  case WSASYSNOTREADY:     return "Winsock library is not ready";
  case WSANOTINITIALISED:  return "Winsock library not initalised";
  case WSAVERNOTSUPPORTED: return "Winsock version not supported";

  case WSAHOST_NOT_FOUND: return "Host not found";
  case WSATRY_AGAIN:      return "Host not found, try again";
  case WSANO_RECOVERY:    return "Unrecoverable error in call to nameserver";
  case WSANO_DATA:        return "No data record of requested type";

  default:
    return NULL;
  }
}

/* Return the error message corresponding to ERR.  This is different
   from Windows libc strerror() in that it handles Winsock errors
   correctly.  */

const char *
windows_strerror (int err)
{
  const char *p;
  if (err >= 0 && err < sys_nerr)
    return strerror (err);
  else if ((p = get_winsock_error (err)) != NULL)
    return p;
  else
    {
      static char buf[32];
      snprintf (buf, sizeof (buf), "Unknown error %d (%#x)", err, err);
      return buf;
    }
}
