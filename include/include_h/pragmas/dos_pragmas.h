#ifndef PRAGMAS_DOS_PRAGMAS_H
#define PRAGMAS_DOS_PRAGMAS_H

/*
**	$VER: dos_pragmas.h 40.2 (6.6.1998)
**
**	Direct ROM interface (pragma) definitions.
**
**	Copyright © 2001 Amiga, Inc.
**	    All Rights Reserved
*/

#if defined(LATTICE) || defined(__SASC) || defined(_DCC)
#ifndef __CLIB_PRAGMA_LIBCALL
#define __CLIB_PRAGMA_LIBCALL
#endif /* __CLIB_PRAGMA_LIBCALL */
#else /* __MAXON__, __STORM__ or AZTEC_C */
#ifndef __CLIB_PRAGMA_AMICALL
#define __CLIB_PRAGMA_AMICALL
#endif /* __CLIB_PRAGMA_AMICALL */
#endif /* */

#if defined(__SASC) || defined(__STORM__)
#ifndef __CLIB_PRAGMA_TAGCALL
#define __CLIB_PRAGMA_TAGCALL
#endif /* __CLIB_PRAGMA_TAGCALL */
#endif /* __MAXON__, __STORM__ or AZTEC_C */

#ifndef CLIB_DOS_PROTOS_H
#include <clib/dos_protos.h>
#endif /* CLIB_DOS_PROTOS_H */

#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall DOSBase Open 1e 2102
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall DOSBase Close 24 101
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall DOSBase Read 2a 32103
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall DOSBase Write 30 32103
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall DOSBase Input 36 00
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall DOSBase Output 3c 00
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall DOSBase Seek 42 32103
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall DOSBase DeleteFile 48 101
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall DOSBase Rename 4e 2102
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall DOSBase Lock 54 2102
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall DOSBase UnLock 5a 101
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall DOSBase DupLock 60 101
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall DOSBase Examine 66 2102
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall DOSBase ExNext 6c 2102
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall DOSBase Info 72 2102
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall DOSBase CreateDir 78 101
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall DOSBase CurrentDir 7e 101
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall DOSBase IoErr 84 00
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall DOSBase CreateProc 8a 432104
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall DOSBase Exit 90 101
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall DOSBase LoadSeg 96 101
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall DOSBase UnLoadSeg 9c 101
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall DOSBase DeviceProc ae 101
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall DOSBase SetComment b4 2102
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall DOSBase SetProtection ba 2102
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall DOSBase DateStamp c0 101
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall DOSBase Delay c6 101
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall DOSBase WaitForChar cc 2102
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall DOSBase ParentDir d2 101
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall DOSBase IsInteractive d8 101
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall DOSBase Execute de 32103
#endif /* __CLIB_PRAGMA_LIBCALL */
/*--- functions in V36 or higher (Release 2.0) ---*/
/*	DOS Object creation/deletion */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall DOSBase AllocDosObject e4 2102
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall DOSBase AllocDosObjectTagList e4 2102
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_TAGCALL
 #ifdef __CLIB_PRAGMA_LIBCALL
  #pragma tagcall DOSBase AllocDosObjectTags e4 2102
 #endif /* __CLIB_PRAGMA_LIBCALL */
#endif /* __CLIB_PRAGMA_TAGCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall DOSBase FreeDosObject ea 2102
#endif /* __CLIB_PRAGMA_LIBCALL */
/*	Packet Level routines */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall DOSBase DoPkt f0 765432107
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall DOSBase DoPkt0 f0 2102
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall DOSBase DoPkt1 f0 32103
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall DOSBase DoPkt2 f0 432104
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall DOSBase DoPkt3 f0 5432105
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall DOSBase DoPkt4 f0 65432106
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall DOSBase SendPkt f6 32103
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall DOSBase WaitPkt fc 00
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall DOSBase ReplyPkt 102 32103
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall DOSBase AbortPkt 108 2102
#endif /* __CLIB_PRAGMA_LIBCALL */
/*	Record Locking */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall DOSBase LockRecord 10e 5432105
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall DOSBase LockRecords 114 2102
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall DOSBase UnLockRecord 11a 32103
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall DOSBase UnLockRecords 120 101
#endif /* __CLIB_PRAGMA_LIBCALL */
/*	Buffered File I/O */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall DOSBase SelectInput 126 101
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall DOSBase SelectOutput 12c 101
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall DOSBase FGetC 132 101
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall DOSBase FPutC 138 2102
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall DOSBase UnGetC 13e 2102
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall DOSBase FRead 144 432104
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall DOSBase FWrite 14a 432104
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall DOSBase FGets 150 32103
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall DOSBase FPuts 156 2102
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall DOSBase VFWritef 15c 32103
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_TAGCALL
 #ifdef __CLIB_PRAGMA_LIBCALL
  #pragma tagcall DOSBase FWritef 15c 32103
 #endif /* __CLIB_PRAGMA_LIBCALL */
#endif /* __CLIB_PRAGMA_TAGCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall DOSBase VFPrintf 162 32103
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_TAGCALL
 #ifdef __CLIB_PRAGMA_LIBCALL
  #pragma tagcall DOSBase FPrintf 162 32103
 #endif /* __CLIB_PRAGMA_LIBCALL */
#endif /* __CLIB_PRAGMA_TAGCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall DOSBase Flush 168 101
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall DOSBase SetVBuf 16e 432104
#endif /* __CLIB_PRAGMA_LIBCALL */
/*	DOS Object Management */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall DOSBase DupLockFromFH 174 101
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall DOSBase OpenFromLock 17a 101
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall DOSBase ParentOfFH 180 101
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall DOSBase ExamineFH 186 2102
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall DOSBase SetFileDate 18c 2102
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall DOSBase NameFromLock 192 32103
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall DOSBase NameFromFH 198 32103
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall DOSBase SplitName 19e 5432105
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall DOSBase SameLock 1a4 2102
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall DOSBase SetMode 1aa 2102
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall DOSBase ExAll 1b0 5432105
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall DOSBase ReadLink 1b6 5432105
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall DOSBase MakeLink 1bc 32103
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall DOSBase ChangeMode 1c2 32103
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall DOSBase SetFileSize 1c8 32103
#endif /* __CLIB_PRAGMA_LIBCALL */
/*	Error Handling */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall DOSBase SetIoErr 1ce 101
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall DOSBase Fault 1d4 432104
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall DOSBase PrintFault 1da 2102
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall DOSBase ErrorReport 1e0 432104
#endif /* __CLIB_PRAGMA_LIBCALL */
/*--- (1 function slot reserved here) ---*/
/*	Process Management */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall DOSBase Cli 1ec 00
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall DOSBase CreateNewProc 1f2 101
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall DOSBase CreateNewProcTagList 1f2 101
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_TAGCALL
 #ifdef __CLIB_PRAGMA_LIBCALL
  #pragma tagcall DOSBase CreateNewProcTags 1f2 101
 #endif /* __CLIB_PRAGMA_LIBCALL */
#endif /* __CLIB_PRAGMA_TAGCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall DOSBase RunCommand 1f8 432104
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall DOSBase GetConsoleTask 1fe 00
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall DOSBase SetConsoleTask 204 101
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall DOSBase GetFileSysTask 20a 00
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall DOSBase SetFileSysTask 210 101
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall DOSBase GetArgStr 216 00
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall DOSBase SetArgStr 21c 101
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall DOSBase FindCliProc 222 101
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall DOSBase MaxCli 228 00
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall DOSBase SetCurrentDirName 22e 101
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall DOSBase GetCurrentDirName 234 2102
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall DOSBase SetProgramName 23a 101
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall DOSBase GetProgramName 240 2102
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall DOSBase SetPrompt 246 101
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall DOSBase GetPrompt 24c 2102
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall DOSBase SetProgramDir 252 101
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall DOSBase GetProgramDir 258 00
#endif /* __CLIB_PRAGMA_LIBCALL */
/*	Device List Management */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall DOSBase SystemTagList 25e 2102
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall DOSBase System 25e 2102
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_TAGCALL
 #ifdef __CLIB_PRAGMA_LIBCALL
  #pragma tagcall DOSBase SystemTags 25e 2102
 #endif /* __CLIB_PRAGMA_LIBCALL */
#endif /* __CLIB_PRAGMA_TAGCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall DOSBase AssignLock 264 2102
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall DOSBase AssignLate 26a 2102
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall DOSBase AssignPath 270 2102
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall DOSBase AssignAdd 276 2102
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall DOSBase RemAssignList 27c 2102
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall DOSBase GetDeviceProc 282 2102
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall DOSBase FreeDeviceProc 288 101
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall DOSBase LockDosList 28e 101
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall DOSBase UnLockDosList 294 101
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall DOSBase AttemptLockDosList 29a 101
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall DOSBase RemDosEntry 2a0 101
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall DOSBase AddDosEntry 2a6 101
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall DOSBase FindDosEntry 2ac 32103
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall DOSBase NextDosEntry 2b2 2102
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall DOSBase MakeDosEntry 2b8 2102
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall DOSBase FreeDosEntry 2be 101
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall DOSBase IsFileSystem 2c4 101
#endif /* __CLIB_PRAGMA_LIBCALL */
/*	Handler Interface */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall DOSBase Format 2ca 32103
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall DOSBase Relabel 2d0 2102
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall DOSBase Inhibit 2d6 2102
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall DOSBase AddBuffers 2dc 2102
#endif /* __CLIB_PRAGMA_LIBCALL */
/*	Date, Time Routines */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall DOSBase CompareDates 2e2 2102
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall DOSBase DateToStr 2e8 101
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall DOSBase StrToDate 2ee 101
#endif /* __CLIB_PRAGMA_LIBCALL */
/*	Image Management */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall DOSBase InternalLoadSeg 2f4 A98004
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall DOSBase InternalUnLoadSeg 2fa 9102
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall DOSBase NewLoadSeg 300 2102
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall DOSBase NewLoadSegTagList 300 2102
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_TAGCALL
 #ifdef __CLIB_PRAGMA_LIBCALL
  #pragma tagcall DOSBase NewLoadSegTags 300 2102
 #endif /* __CLIB_PRAGMA_LIBCALL */
#endif /* __CLIB_PRAGMA_TAGCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall DOSBase AddSegment 306 32103
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall DOSBase FindSegment 30c 32103
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall DOSBase RemSegment 312 101
#endif /* __CLIB_PRAGMA_LIBCALL */
/*	Command Support */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall DOSBase CheckSignal 318 101
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall DOSBase ReadArgs 31e 32103
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall DOSBase FindArg 324 2102
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall DOSBase ReadItem 32a 32103
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall DOSBase StrToLong 330 2102
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall DOSBase MatchFirst 336 2102
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall DOSBase MatchNext 33c 101
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall DOSBase MatchEnd 342 101
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall DOSBase ParsePattern 348 32103
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall DOSBase MatchPattern 34e 2102
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall DOSBase FreeArgs 35a 101
#endif /* __CLIB_PRAGMA_LIBCALL */
/*--- (1 function slot reserved here) ---*/
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall DOSBase FilePart 366 101
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall DOSBase PathPart 36c 101
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall DOSBase AddPart 372 32103
#endif /* __CLIB_PRAGMA_LIBCALL */
/*	Notification */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall DOSBase StartNotify 378 101
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall DOSBase EndNotify 37e 101
#endif /* __CLIB_PRAGMA_LIBCALL */
/*	Environment Variable functions */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall DOSBase SetVar 384 432104
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall DOSBase GetVar 38a 432104
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall DOSBase DeleteVar 390 2102
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall DOSBase FindVar 396 2102
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall DOSBase CliInitNewcli 3a2 801
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall DOSBase CliInitRun 3a8 801
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall DOSBase WriteChars 3ae 2102
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall DOSBase PutStr 3b4 101
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall DOSBase VPrintf 3ba 2102
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_TAGCALL
 #ifdef __CLIB_PRAGMA_LIBCALL
  #pragma tagcall DOSBase Printf 3ba 2102
 #endif /* __CLIB_PRAGMA_LIBCALL */
#endif /* __CLIB_PRAGMA_TAGCALL */
/*--- (1 function slot reserved here) ---*/
/* these were unimplemented until dos 36.147 */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall DOSBase ParsePatternNoCase 3c6 32103
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall DOSBase MatchPatternNoCase 3cc 2102
#endif /* __CLIB_PRAGMA_LIBCALL */
/* this was added for V37 dos, returned 0 before then. */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall DOSBase SameDevice 3d8 2102
#endif /* __CLIB_PRAGMA_LIBCALL */

/* NOTE: the following entries did NOT exist before ks 36.303 (2.02) */
/* If you are going to use them, open dos.library with version 37 */

/* These calls were added for V39 dos: */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall DOSBase ExAllEnd 3de 5432105
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall DOSBase SetOwner 3e4 2102
#endif /* __CLIB_PRAGMA_LIBCALL */
/*--- (2 function slots reserved here) ---*/
/*--- (2 function slots reserved here) ---*/
/*--- (2 function slots reserved here) ---*/
/*--- (2 function slots reserved here) ---*/

#endif /* PRAGMAS_DOS_PRAGMAS_H */
