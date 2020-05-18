#ifndef MEMBST
#define MEMBST

#include <stdlib.h>
#include <stdio.h>
#include "recordsLL.h"

typedef struct memoryNode {
  size_t ptrAddr;
  int allocated;
  size_t leftBound;
  size_t rightBound;
  record *currRecord;
  struct memoryNode *smallerNode;
  struct memoryNode *largerNode;
} memoryNode;

memoryNode *createNewNode(size_t ptrAddr, size_t leftBound, size_t rightBound, 
                          record *currRecord);
void memInsert(void **ptr, u_int size, int leftPadding, int rightPadding,
                      record *currRecord);
int memDoubleMalloc(void **ptr);
void memRemove();
int memFreeExact(void **ptr, u_int flags, const char *filename, const char *fxname, int line_num);
int memFreeApprox(void **ptr, u_int flags, const char *filename, const char *fxname, int line_num);
void freeAllNodes();

#endif
