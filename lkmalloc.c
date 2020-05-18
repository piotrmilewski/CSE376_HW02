// includes
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include "lkmalloc.h"
#include "memBST.h"
#include "recordsLL.h"

// memory patterns
#define MEM_UNDER 0x6b // pattern for lower bytes
#define MEM_OVER 0x5a // pattern for upper bytes

// lkmalloc flags
#define LKM_REG 0x0 // allocate memory without any of the special protections below
#define LKM_INIT 0x1 // initialize the memory being allocated to 0s
#define LKM_OVER 0x2 // allocate 8 more bytes of memory and write the pattern 0x5a in upper bytes
#define LKM_UNDER 0x4 // allocate 8 more bytes of memory and write the pattern 0x6b in lower bytes

// lkfree flags
#define LKF_REG 0x0 // free only if the ptr passed was exactly as was allocated
#define LKF_APPROX 0x1 // free an allocation even if what is passed is in middle of a valid alloc 
#define LKF_WARN 0x2 // print a warning if you free a ptr as per LKF_APPROX
#define LKF_UNKNOWN 0x4 // print a warning if asked to free a ptr that has never been allocated
#define LKF_ERROR 0x8 // exit the program if any condition matches LKF_WARN or LKF_UNKNOWN

// lkreport flags
#define LKR_NONE 0x0 // do not produce a report
#define LKR_SERIOUS 0x1  // print memory leaks (e.g., mallocs w/o a corresponding free)
#define LKR_MATCH 0x2 // print perfectly matching alloc/free pairs
#define LKR_BAD_FREE 0x4 // print bad ‘free's (ones where passed addr is in middle of alloc addr)
#define LKR_ORPHAN_FREE 0x8 // print orphan ‘free's (ones that had never been allocated)
#define LKR_DOUBLE_FREE 0x10 // print double free’d pointers
#define LKR_APPROX 0x20 // print matching alloc/free pairs that were freed due to LKF_APPROX above

void onExitFunc(int exitStatus, void *arg){
  lkreport((intptr_t)arg, LKR_SERIOUS);
  freeAllRecords();
  freeAllNodes();
}

// lkmalloc function
int lkmallocI(u_int size, void **ptr, u_int flags, const char *filename, 
              const char *fxname, int line_num){
  // check and set flags
  u_int adjustedSize = size;
  int init = 0;
  int over = 0;
  int under = 0;
  if (LKM_INIT & flags)
    init = 1;
  if (LKM_OVER & flags){
    over = 1;
    adjustedSize += 8;
  }
  if (LKM_UNDER & flags){
    under = 1;
    adjustedSize += 8;
  }
  // allocate memory
  memDoubleMalloc(ptr); // check if possibility of a double malloc
  *ptr = malloc(adjustedSize);
  if (NULL == *ptr){
    return ENOMEM;
  }
  // memory successfully allocated, initialize memory based on flags
  char *ptrToMem = *ptr;
  // LKM_UNDER flag
  if (under == 1){
    *ptr += 8; // need to shift user pointer to after padding
    for (int i = 0; i < 8; i++){
      *ptrToMem = MEM_UNDER;
      ptrToMem++;
    }
  }
  // LKM_INIT flag
  if (init == 1){
    for (int i = 0; i < size; i++){
      *ptrToMem = 0;
      ptrToMem++;
    }
  }
  else{
    ptrToMem += size; // need to move pointer to end for LKM_OVER
  }
  // LKM_OVER flag
  if (over == 1){
    for (int i = 0; i < 8; i++){
      *ptrToMem = MEM_OVER;
      ptrToMem++;
    }
  }
  record *passToBST = recordInsert(0, filename, fxname, line_num, ptr, 0, size, *ptr, 1, -1, -1);
  memInsert(ptr, size, under * 8, over * 8, passToBST); // insert into BST
  return 0;
}

// lkfree function
int lkfreeI(void **ptr, u_int flags, const char *filename,
           const char *fxname, int line_num){
  // check and set flags
  int approx = 0;
  int warn = 0;
  int unknown = 0;
  int error = 0;
  if (LKF_APPROX & flags)
    approx = 1;
  if (LKF_WARN & flags)
    warn = 1;
  if (LKF_UNKNOWN & flags)
    unknown = 1;
  if (LKF_ERROR & flags)
    error = 1;
  // do the actual freeing
  int retVal = 0;
  if (approx == 1){
    retVal = memFreeApprox(ptr, flags, filename, fxname, line_num);
    if (retVal == 0){ // if free was successful
      if (error == 1){
        fprintf(stderr, "Freed an allocation that was in the middle of a valid allocation\n");
        fprintf(stderr, "Exiting the program...\n\n");
        exit(0);
      }
      else if (warn == 1){
        fprintf(stderr, "Freed an allocation that was in the middle of a valid allocation\n");
      }
      return 0;
    }
  }
  else{
    retVal = memFreeExact(ptr, flags, filename, fxname, line_num);
    if (retVal == 0){ // if free was successful
      return 0;
    }
  }
  // if free was unsuccessful
  if (error == 1){
    fprintf(stderr, "Cannot free a ptr that has never been allocated\n");
    fprintf(stderr, "Exiting the program...\n\n");
    exit(0);
  }
  else if (unknown == 1){
    fprintf(stderr, "Cannot free a ptr that has never been allocated\n");
  }
  return EINVAL;
}

// lkreport function
int lkreport(int fd, u_int flags){
  // flag processing
  if (LKR_NONE & flags){
    freeAllRecords();
    freeAllNodes();
    return 0;
  }
  int serious = 0;
  int match = 0;
  int badFree = 0;
  int orphanFree = 0;
  int doubleFree = 0;
  int approx = 0;
  if (LKR_SERIOUS & flags)
    serious = 1;
  if (LKR_MATCH & flags)
    match = 1;
  if (LKR_BAD_FREE & flags)
    badFree = 1;
  if (LKR_ORPHAN_FREE & flags)
    orphanFree = 1;
  if (LKR_DOUBLE_FREE & flags)
    doubleFree = 1;
  if (LKR_APPROX & flags)
    approx = 1;
  int retVal = dumpReport(fd, serious, match, badFree, orphanFree, doubleFree, approx);
  return retVal;
}
