/******************************************************************************
 *                               FREXXWARE
 * ----------------------------------------------------------------------------
 *
 * Project: Frexx C Preprocessor
 * $Source: /home/user/start/cpp/RCS/memory.h,v $
 * $Revision: 1.3 $
 * $Date: 1993/12/06 13:51:20 $
 * $Author: start $
 * $State: Exp $
 * $Locker: start $
 *
 * ----------------------------------------------------------------------------
 * $Log: memory.h,v $
 * Revision 1.3  1993/12/06  13:51:20  start
 * A lot of new stuff (too much to mention)
 *
 * Revision 1.2  1993/11/11  07:16:39  start
 * New stuff
 *
 * Revision 1.2  1993/11/11  07:16:39  start
 * New stuff
 *
 * Revision 1.1  1993/11/03  09:15:59  start
 * Initial revision
 *
 *
 *****************************************************************************/
/******************************************************************************

 memory.h

 Structures and defines for memory functions.

 *****************************************************************************/

typedef struct MemInfo {
  struct MemInfo *prev;
  struct MemInfo *next;
  int size;
} MemInfo;

void Free(void *);
void FreeAll(void);
void *Malloc(int);
void *Realloc(void *, int);
#ifdef DEBUG
void CheckMem(void *);
#endif

#ifdef DEBUG
#define MEMORY_COOKIE 0		/* When using the DEBUG option, all Malloc()
				   will allocate a number of extra bytes at
				   the end of the block. These will be checked
				   to be intact when the block is freed or
				   CheckMem()'ed. This #define tells the size
				   of that block! */
#endif
