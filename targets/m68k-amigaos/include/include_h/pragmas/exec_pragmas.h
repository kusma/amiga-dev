#ifndef PRAGMAS_EXEC_PRAGMAS_H
#define PRAGMAS_EXEC_PRAGMAS_H

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
 #pragma syscall Supervisor 1e D01
#endif /* __CLIB_PRAGMA_LIBCALL */
/*------ special patchable hooks to internal exec activity ------------*/
/*------ module creation ----------------------------------------------*/
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma syscall InitCode 48 1002
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma syscall InitStruct 4e 0A903
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma syscall MakeLibrary 54 10A9805
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma syscall MakeFunctions 5a A9803
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma syscall FindResident 60 901
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma syscall InitResident 66 1902
#endif /* __CLIB_PRAGMA_LIBCALL */
/*------ diagnostics --------------------------------------------------*/
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma syscall Alert 6c 701
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma syscall Debug 72 001
#endif /* __CLIB_PRAGMA_LIBCALL */
/*------ interrupts ---------------------------------------------------*/
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma syscall Disable 78 00
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma syscall Enable 7e 00
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma syscall Forbid 84 00
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma syscall Permit 8a 00
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma syscall SetSR 90 1002
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma syscall SuperState 96 00
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma syscall UserState 9c 001
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma syscall SetIntVector a2 9002
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma syscall AddIntServer a8 9002
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma syscall RemIntServer ae 9002
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma syscall Cause b4 901
#endif /* __CLIB_PRAGMA_LIBCALL */
/*------ memory allocation --------------------------------------------*/
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma syscall Allocate ba 0802
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma syscall Deallocate c0 09803
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma syscall AllocMem c6 1002
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma syscall AllocAbs cc 9002
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma syscall FreeMem d2 0902
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma syscall AvailMem d8 101
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma syscall AllocEntry de 801
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma syscall FreeEntry e4 801
#endif /* __CLIB_PRAGMA_LIBCALL */
/*------ lists --------------------------------------------------------*/
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma syscall Insert ea A9803
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma syscall AddHead f0 9802
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma syscall AddTail f6 9802
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma syscall Remove fc 901
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma syscall RemHead 102 801
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma syscall RemTail 108 801
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma syscall Enqueue 10e 9802
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma syscall FindName 114 9802
#endif /* __CLIB_PRAGMA_LIBCALL */
/*------ tasks --------------------------------------------------------*/
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma syscall AddTask 11a BA903
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma syscall RemTask 120 901
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma syscall FindTask 126 901
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma syscall SetTaskPri 12c 0902
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma syscall SetSignal 132 1002
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma syscall SetExcept 138 1002
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma syscall Wait 13e 001
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma syscall Signal 144 0902
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma syscall AllocSignal 14a 001
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma syscall FreeSignal 150 001
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma syscall AllocTrap 156 001
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma syscall FreeTrap 15c 001
#endif /* __CLIB_PRAGMA_LIBCALL */
/*------ messages -----------------------------------------------------*/
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma syscall AddPort 162 901
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma syscall RemPort 168 901
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma syscall PutMsg 16e 9802
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma syscall GetMsg 174 801
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma syscall ReplyMsg 17a 901
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma syscall WaitPort 180 801
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma syscall FindPort 186 901
#endif /* __CLIB_PRAGMA_LIBCALL */
/*------ libraries ----------------------------------------------------*/
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma syscall AddLibrary 18c 901
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma syscall RemLibrary 192 901
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma syscall OldOpenLibrary 198 901
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma syscall CloseLibrary 19e 901
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma syscall SetFunction 1a4 08903
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma syscall SumLibrary 1aa 901
#endif /* __CLIB_PRAGMA_LIBCALL */
/*------ devices ------------------------------------------------------*/
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma syscall AddDevice 1b0 901
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma syscall RemDevice 1b6 901
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma syscall OpenDevice 1bc 190804
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma syscall CloseDevice 1c2 901
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma syscall DoIO 1c8 901
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma syscall SendIO 1ce 901
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma syscall CheckIO 1d4 901
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma syscall WaitIO 1da 901
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma syscall AbortIO 1e0 901
#endif /* __CLIB_PRAGMA_LIBCALL */
/*------ resources ----------------------------------------------------*/
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma syscall AddResource 1e6 901
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma syscall RemResource 1ec 901
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma syscall OpenResource 1f2 901
#endif /* __CLIB_PRAGMA_LIBCALL */
/*------ private diagnostic support -----------------------------------*/
/*------ misc ---------------------------------------------------------*/
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma syscall RawDoFmt 20a BA9804
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma syscall GetCC 210 00
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma syscall TypeOfMem 216 901
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma syscall Procure 21c 9802
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma syscall Vacate 222 9802
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma syscall OpenLibrary 228 0902
#endif /* __CLIB_PRAGMA_LIBCALL */
/*--- functions in V33 or higher (Release 1.2) ---*/
/*------ signal semaphores (note funny registers)----------------------*/
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma syscall InitSemaphore 22e 801
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma syscall ObtainSemaphore 234 801
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma syscall ReleaseSemaphore 23a 801
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma syscall AttemptSemaphore 240 801
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma syscall ObtainSemaphoreList 246 801
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma syscall ReleaseSemaphoreList 24c 801
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma syscall FindSemaphore 252 901
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma syscall AddSemaphore 258 901
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma syscall RemSemaphore 25e 901
#endif /* __CLIB_PRAGMA_LIBCALL */
/*------ kickmem support ----------------------------------------------*/
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma syscall SumKickData 264 00
#endif /* __CLIB_PRAGMA_LIBCALL */
/*------ more memory support ------------------------------------------*/
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma syscall AddMemList 26a 9821005
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma syscall CopyMem 270 09803
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma syscall CopyMemQuick 276 09803
#endif /* __CLIB_PRAGMA_LIBCALL */
/*------ cache --------------------------------------------------------*/
/*--- functions in V36 or higher (Release 2.0) ---*/
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma syscall CacheClearU 27c 00
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma syscall CacheClearE 282 10803
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma syscall CacheControl 288 1002
#endif /* __CLIB_PRAGMA_LIBCALL */
/*------ misc ---------------------------------------------------------*/
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma syscall CreateIORequest 28e 0802
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma syscall DeleteIORequest 294 801
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma syscall CreateMsgPort 29a 00
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma syscall DeleteMsgPort 2a0 801
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma syscall ObtainSemaphoreShared 2a6 801
#endif /* __CLIB_PRAGMA_LIBCALL */
/*------ even more memory support -------------------------------------*/
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma syscall AllocVec 2ac 1002
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma syscall FreeVec 2b2 901
#endif /* __CLIB_PRAGMA_LIBCALL */
/*------ V39 Pool LVOs...*/
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma syscall CreatePool 2b8 21003
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma syscall DeletePool 2be 801
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma syscall AllocPooled 2c4 0802
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma syscall FreePooled 2ca 09803
#endif /* __CLIB_PRAGMA_LIBCALL */
/*------ misc ---------------------------------------------------------*/
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma syscall AttemptSemaphoreShared 2d0 801
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma syscall ColdReboot 2d6 00
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma syscall StackSwap 2dc 801
#endif /* __CLIB_PRAGMA_LIBCALL */
/*------ future expansion ---------------------------------------------*/
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma syscall CachePreDMA 2fa 09803
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma syscall CachePostDMA 300 09803
#endif /* __CLIB_PRAGMA_LIBCALL */
/*------ New, for V39*/
/*--- functions in V39 or higher (Release 3) ---*/
/*------ Low memory handler functions*/
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma syscall AddMemHandler 306 901
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma syscall RemMemHandler 30c 901
#endif /* __CLIB_PRAGMA_LIBCALL */
/*------ Function to attempt to obtain a Quick Interrupt Vector...*/
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma syscall ObtainQuickVector 312 801
#endif /* __CLIB_PRAGMA_LIBCALL */
/*--- functions in V45 or higher ---*/
/*------ Finally the list functions are complete*/
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma syscall NewMinList 33c 801
#endif /* __CLIB_PRAGMA_LIBCALL */
/*------ New AVL tree support for V45. Yes, this is intentionally part of Exec!*/
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma syscall AVL_AddNode 354 A9803
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma syscall AVL_RemNodeByAddress 35a 9802
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma syscall AVL_RemNodeByKey 360 A9803
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma syscall AVL_FindNode 366 A9803
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma syscall AVL_FindPrevNodeByAddress 36c 801
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma syscall AVL_FindPrevNodeByKey 372 A9803
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma syscall AVL_FindNextNodeByAddress 378 801
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma syscall AVL_FindNextNodeByKey 37e A9803
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma syscall AVL_FindFirstNode 384 801
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma syscall AVL_FindLastNode 38a 801
#endif /* __CLIB_PRAGMA_LIBCALL */
/*--- (10 function slots reserved here) ---*/

#endif /* PRAGMAS_EXEC_PRAGMAS_H */
