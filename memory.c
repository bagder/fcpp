#ifdef RCS
static char rcsid[]="$Id: memory.c,v 1.2 1994/01/24 09:36:46 start Exp $";
#endif
/******************************************************************************
Copyright (c) 1999 Daniel Stenberg

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to
deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE
SOFTWARE.
******************************************************************************/
/******************************************************************************
 *                               FREXXWARE
 * ----------------------------------------------------------------------------
 *
 * Project: Frexx C Preprocessor
 * $Source: /home/user/start/cpp/RCS/memory.c,v $
 * $Revision: 1.2 $
 * $Date: 1994/01/24 09:36:46 $
 * $Author: start $
 * $State: Exp $
 * $Locker:  $
 *
 * ----------------------------------------------------------------------------
 * $Log: memory.c,v $
 * Revision 1.2  1994/01/24  09:36:46  start
 * Made it run with OS9 properly.
 *
 * Revision 1.1  1993/11/03  09:13:08  start
 * Initial revision
 *
 *
 *****************************************************************************/

#include <string.h>
#include <stdlib.h>
#if defined(OS9)
#include <types.h>
#elif defined(UNIX)
#include <sys/types.h>
#elif defined(AMIGA)
#include <exec/types.h>
#include <proto/exec.h>
#endif

#include <stdio.h>

#include "memory.h"
#ifdef DEBUG
int mem;
int maxmem;
int malloc_count=0;
#endif

/*
 * We have two mallockey pointers because we have two different kinds of
 * Malloc()s! One for each execution and one for each fplInit().
 */
static struct MemInfo *MallocKey=NULL;

void *Realloc(void *ptr, int size)
{
  struct MemInfo *point;
  void *new;
  /* `point' points to the MemInfo structure: */
  point=(struct MemInfo *)((char *)ptr-sizeof(struct MemInfo));
 
  if(size<=point->size)
    /*
     * To increase performance, don't care about reallocing
     * to smaller sizes!
     */
    return(ptr);
  new=Malloc(size); /* allocate new storage */
  if(!new)
    return(NULL); /* fail! */
  
  memcpy(new, ptr, point->size); /* copy contents */

  Free(ptr); /* free old area */

  return(new); /* return new pointer */
}


void *Malloc(int size)
{
  char *alloc;
  struct MemInfo *point;

#ifdef DEBUG
  size+=MEMORY_COOKIE; /* add extra bytes after the block! */
#endif

#ifdef AMIGA
  alloc=(char *)AllocMem(size+sizeof(struct MemInfo), 0L);
#elif defined(UNIX)
  alloc=(char *)malloc(size+sizeof(struct MemInfo));
#endif

  if(!alloc)
    return(NULL);

  point=(struct MemInfo *)alloc;
  point->prev=MallocKey;	 /* previous */
  point->next=NULL;		 /* next */
  point->size=size;		 /* size */
#ifdef DEBUG
  malloc_count++;
  mem+=size+sizeof(struct MemInfo);
  if(mem>maxmem)
    maxmem=mem;

  memset((void *)((char *)point+sizeof(struct MemInfo)), 0xbb, size);
#endif
  if(MallocKey)
    point->prev->next=point;

  MallocKey = (void *)point;

  alloc = ((char *)point+sizeof(struct MemInfo));
  return ((void *)alloc);
}

void Free(void *ptr)
{
  struct MemInfo *point;
  /* `point' points to the MemInfo structure: */
  point=(struct MemInfo *)((char *)ptr-sizeof(struct MemInfo));

  if(MallocKey==point) {
    /* if this is the last Malloc, set `last' to `prev' */
    MallocKey=point->prev;
    if(MallocKey)
      MallocKey->next=NULL;
  } else {
    /* point the previous' `next' to our `next', and our next `previous'
       to our `previous'. Unlink us from the chain */
    if(point->prev)
      /* only if we aren't the _first_ Malloc() ! */
      point->prev->next=point->next;
    if(point->next)
      /* only if there is a next! */
      point->next->prev=point->prev;
  }
#ifdef DEBUG
  mem-=point->size+sizeof(struct MemInfo);

  CheckMem(ptr);
#endif

#ifdef AMIGA  
  FreeMem(point, point->size+sizeof(struct MemInfo));
#elif UNIX
  free(point);
#endif
}

#ifdef DEBUG
void CheckMem(void *ptr)
{
  int i;
  int b=0;
  struct MemInfo *point;
  /* `point' points to the MemInfo structure: */
  point=(struct MemInfo *)((char *)ptr-sizeof(struct MemInfo));
  for(i=0;i<MEMORY_COOKIE; i++)
    if(*((unsigned char *)point+sizeof(struct MemInfo)+i+point->size-MEMORY_COOKIE)!= 0xbb)
      b++;

  if(b) {
#if defined(UNIX)
    fprintf(stderr, "Memory violation: malloc(%d) was abused %d bytes!\n",
	    point->size-MEMORY_COOKIE-sizeof(struct MemInfo), b);
#elif defined(AMIGA)
    /* ERROR */;
#endif
  }
}
#endif

void FreeAll(void)
{
  struct MemInfo *point;
  void *prev;

  if(!MallocKey)
    return;
  do {
    point=MallocKey;
    /* `point' points to the MemInfo structure! */

    prev=(void *)point->prev;

#ifdef DEBUG
    mem-=point->size+sizeof(struct MemInfo);
#endif
#ifdef AMIGA
    FreeMem(MallocKey, point->size+sizeof(struct MemInfo));
#elif UNIX
    free((char *)MallocKey);
#endif
  } while(MallocKey=prev);
}
