#ifndef PRAGMAS_EXEC_SYSBASE_PRAGMAS_H
#define PRAGMAS_EXEC_SYSBASE_PRAGMAS_H

/*
**	$VER: exec_pragmas.h 45.2 (6.6.1998)
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

#ifndef CLIB_EXEC_PROTOS_H
#include <clib/exec_protos.h>
#endif /* CLIB_EXEC_PROTOS_H */

/*------ misc ---------------------------------------------------------*/
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall SysBase Supervisor 1e D01
#endif /* __CLIB_PRAGMA_LIBCALL */
/*------ special patchable hooks to internal exec activity ------------*/
/*------ module creation ----------------------------------------------*/
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall SysBase InitCode 48 1002
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall SysBase InitStruct 4e 0A903
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall SysBase MakeLibrary 54 10A9805
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall SysBase MakeFunctions 5a A9803
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall SysBase FindResident 60 901
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall SysBase InitResident 66 1902
#endif /* __CLIB_PRAGMA_LIBCALL */
/*------ diagnostics --------------------------------------------------*/
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall SysBase Alert 6c 701
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall SysBase Debug 72 001
#endif /* __CLIB_PRAGMA_LIBCALL */
/*------ interrupts ---------------------------------------------------*/
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall SysBase Disable 78 00
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall SysBase Enable 7e 00
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall SysBase Forbid 84 00
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall SysBase Permit 8a 00
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall SysBase SetSR 90 1002
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall SysBase SuperState 96 00
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall SysBase UserState 9c 001
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall SysBase SetIntVector a2 9002
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall SysBase AddIntServer a8 9002
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall SysBase RemIntServer ae 9002
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall SysBase Cause b4 901
#endif /* __CLIB_PRAGMA_LIBCALL */
/*------ memory allocation --------------------------------------------*/
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall SysBase Allocate ba 0802
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall SysBase Deallocate c0 09803
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall SysBase AllocMem c6 1002
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall SysBase AllocAbs cc 9002
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall SysBase FreeMem d2 0902
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall SysBase AvailMem d8 101
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall SysBase AllocEntry de 801
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall SysBase FreeEntry e4 801
#endif /* __CLIB_PRAGMA_LIBCALL */
/*------ lists --------------------------------------------------------*/
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall SysBase Insert ea A9803
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall SysBase AddHead f0 9802
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall SysBase AddTail f6 9802
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall SysBase Remove fc 901
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall SysBase RemHead 102 801
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall SysBase RemTail 108 801
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall SysBase Enqueue 10e 9802
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall SysBase FindName 114 9802
#endif /* __CLIB_PRAGMA_LIBCALL */
/*------ tasks --------------------------------------------------------*/
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall SysBase AddTask 11a BA903
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall SysBase RemTask 120 901
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall SysBase FindTask 126 901
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall SysBase SetTaskPri 12c 0902
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall SysBase SetSignal 132 1002
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall SysBase SetExcept 138 1002
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall SysBase Wait 13e 001
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall SysBase Signal 144 0902
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall SysBase AllocSignal 14a 001
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall SysBase FreeSignal 150 001
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall SysBase AllocTrap 156 001
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall SysBase FreeTrap 15c 001
#endif /* __CLIB_PRAGMA_LIBCALL */
/*------ messages -----------------------------------------------------*/
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall SysBase AddPort 162 901
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall SysBase RemPort 168 901
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall SysBase PutMsg 16e 9802
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall SysBase GetMsg 174 801
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall SysBase ReplyMsg 17a 901
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall SysBase WaitPort 180 801
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall SysBase FindPort 186 901
#endif /* __CLIB_PRAGMA_LIBCALL */
/*------ libraries ----------------------------------------------------*/
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall SysBase AddLibrary 18c 901
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall SysBase RemLibrary 192 901
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall SysBase OldOpenLibrary 198 901
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall SysBase CloseLibrary 19e 901
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall SysBase SetFunction 1a4 08903
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall SysBase SumLibrary 1aa 901
#endif /* __CLIB_PRAGMA_LIBCALL */
/*------ devices ------------------------------------------------------*/
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall SysBase AddDevice 1b0 901
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall SysBase RemDevice 1b6 901
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall SysBase OpenDevice 1bc 190804
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall SysBase CloseDevice 1c2 901
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall SysBase DoIO 1c8 901
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall SysBase SendIO 1ce 901
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall SysBase CheckIO 1d4 901
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall SysBase WaitIO 1da 901
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall SysBase AbortIO 1e0 901
#endif /* __CLIB_PRAGMA_LIBCALL */
/*------ resources ----------------------------------------------------*/
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall SysBase AddResource 1e6 901
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall SysBase RemResource 1ec 901
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall SysBase OpenResource 1f2 901
#endif /* __CLIB_PRAGMA_LIBCALL */
/*------ private diagnostic support -----------------------------------*/
/*------ misc ---------------------------------------------------------*/
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall SysBase RawDoFmt 20a BA9804
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall SysBase GetCC 210 00
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall SysBase TypeOfMem 216 901
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall SysBase Procure 21c 9802
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall SysBase Vacate 222 9802
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall SysBase OpenLibrary 228 0902
#endif /* __CLIB_PRAGMA_LIBCALL */
/*--- functions in V33 or higher (Release 1.2) ---*/
/*------ signal semaphores (note funny registers)----------------------*/
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall SysBase InitSemaphore 22e 801
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall SysBase ObtainSemaphore 234 801
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall SysBase ReleaseSemaphore 23a 801
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall SysBase AttemptSemaphore 240 801
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall SysBase ObtainSemaphoreList 246 801
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall SysBase ReleaseSemaphoreList 24c 801
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall SysBase FindSemaphore 252 901
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall SysBase AddSemaphore 258 901
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall SysBase RemSemaphore 25e 901
#endif /* __CLIB_PRAGMA_LIBCALL */
/*------ kickmem support ----------------------------------------------*/
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall SysBase SumKickData 264 00
#endif /* __CLIB_PRAGMA_LIBCALL */
/*------ more memory support ------------------------------------------*/
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall SysBase AddMemList 26a 9821005
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall SysBase CopyMem 270 09803
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall SysBase CopyMemQuick 276 09803
#endif /* __CLIB_PRAGMA_LIBCALL */
/*------ cache --------------------------------------------------------*/
/*--- functions in V36 or higher (Release 2.0) ---*/
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall SysBase CacheClearU 27c 00
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall SysBase CacheClearE 282 10803
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall SysBase CacheControl 288 1002
#endif /* __CLIB_PRAGMA_LIBCALL */
/*------ misc ---------------------------------------------------------*/
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall SysBase CreateIORequest 28e 0802
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall SysBase DeleteIORequest 294 801
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall SysBase CreateMsgPort 29a 00
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall SysBase DeleteMsgPort 2a0 801
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall SysBase ObtainSemaphoreShared 2a6 801
#endif /* __CLIB_PRAGMA_LIBCALL */
/*------ even more memory support -------------------------------------*/
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall SysBase AllocVec 2ac 1002
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall SysBase FreeVec 2b2 901
#endif /* __CLIB_PRAGMA_LIBCALL */
/*------ V39 Pool LVOs...*/
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall SysBase CreatePool 2b8 21003
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall SysBase DeletePool 2be 801
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall SysBase AllocPooled 2c4 0802
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall SysBase FreePooled 2ca 09803
#endif /* __CLIB_PRAGMA_LIBCALL */
/*------ misc ---------------------------------------------------------*/
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall SysBase AttemptSemaphoreShared 2d0 801
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall SysBase ColdReboot 2d6 00
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall SysBase StackSwap 2dc 801
#endif /* __CLIB_PRAGMA_LIBCALL */
/*------ future expansion ---------------------------------------------*/
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall SysBase CachePreDMA 2fa 09803
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall SysBase CachePostDMA 300 09803
#endif /* __CLIB_PRAGMA_LIBCALL */
/*------ New, for V39*/
/*--- functions in V39 or higher (Release 3) ---*/
/*------ Low memory handler functions*/
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall SysBase AddMemHandler 306 901
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall SysBase RemMemHandler 30c 901
#endif /* __CLIB_PRAGMA_LIBCALL */
/*------ Function to attempt to obtain a Quick Interrupt Vector...*/
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall SysBase ObtainQuickVector 312 801
#endif /* __CLIB_PRAGMA_LIBCALL */
/*--- functions in V45 or higher ---*/
/*------ Finally the list functions are complete*/
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall SysBase NewMinList 33c 801
#endif /* __CLIB_PRAGMA_LIBCALL */
/*------ New AVL tree support for V45. Yes, this is intentionally part of Exec!*/
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall SysBase AVL_AddNode 354 A9803
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall SysBase AVL_RemNodeByAddress 35a 9802
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall SysBase AVL_RemNodeByKey 360 A9803
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall SysBase AVL_FindNode 366 A9803
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall SysBase AVL_FindPrevNodeByAddress 36c 801
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall SysBase AVL_FindPrevNodeByKey 372 A9803
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall SysBase AVL_FindNextNodeByAddress 378 801
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall SysBase AVL_FindNextNodeByKey 37e A9803
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall SysBase AVL_FindFirstNode 384 801
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall SysBase AVL_FindLastNode 38a 801
#endif /* __CLIB_PRAGMA_LIBCALL */
/*--- (10 function slots reserved here) ---*/

#endif /* PRAGMAS_EXEC_SYSBASE_PRAGMAS_H */
