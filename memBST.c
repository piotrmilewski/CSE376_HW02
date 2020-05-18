// includes
#include "memBST.h"
#include "recordsLL.h"

static memoryNode *root = NULL;

memoryNode *createNewNode(size_t ptrAddr, size_t leftBound, size_t rightBound, 
                          record *currRecord){
  memoryNode *new = (memoryNode *)malloc(sizeof(memoryNode));
  new->ptrAddr = ptrAddr;
  new->allocated = 1;
  new->leftBound = leftBound;
  new->rightBound = rightBound;
  new->currRecord = currRecord;
  new->smallerNode = NULL;
  new->largerNode = NULL;
  return new;
}

void memInsert(void **ptr, u_int size, int leftPadding, int rightPadding, 
                      record *currRecord){
  size_t ptrAddr = (size_t)(*ptr);
  size_t leftBound = ptrAddr - leftPadding;
  size_t rightBound = ptrAddr + size + rightPadding;
  memoryNode *newNode = createNewNode(ptrAddr, leftBound, rightBound, currRecord);
  // if BST is currently empty initialize it with a newNode
  if (root == NULL){
    root = newNode;
    return;
  }
  // otherwise find an according empty node
  memoryNode *ptrToNode = root;
  memoryNode *trailer = NULL;
  while (ptrToNode != NULL){
    trailer = ptrToNode;
    // iterate down the tree until you reach an empty node
    if (ptrAddr <= ptrToNode->rightBound && ptrAddr >= ptrToNode->leftBound){
      if (ptrToNode->allocated == 1){
        fprintf(stderr, "Warning: attempting to overwrite nonfree memory.\n");
        fprintf(stderr, "This warning should not be seen by user.\n");
        return;
      }
      else{
        // found free memory that was malloced in the past, give it new credentials
        ptrToNode->ptrAddr = ptrAddr;
        ptrToNode->allocated = 1;
        ptrToNode->leftBound = leftBound;
        ptrToNode->rightBound = rightBound;
        ptrToNode->currRecord = currRecord; // provide it a new record
        free(newNode); // don't need a new node anymore so get rid of it
        return; // exit since dont need to find an empty node anymore
      }
    }
    else if (ptrAddr < ptrToNode->ptrAddr){
      ptrToNode = ptrToNode->smallerNode;
    }
    else{
      ptrToNode = ptrToNode->largerNode;
    }
  }
  // only perform if ptrToNode loop has finished
  if (ptrAddr < trailer->ptrAddr)
    trailer->smallerNode = newNode;
  else
    trailer->largerNode = newNode;
  return;
}

// returns -1 when found ptr that is malloced, 0 when not found
int memDoubleMalloc(void **ptr){
  size_t ptrAddr = (size_t)(*ptr);
  memoryNode *ptrToNode = root;
  while(ptrToNode != NULL){
    if (ptrAddr <= ptrToNode->rightBound && ptrAddr >= ptrToNode->leftBound){
      if (ptrToNode->allocated == 1){
        fprintf(stderr, "Warning: Double Malloc\n");
        fprintf(stderr, "You are overwriting an existing ptr with a new malloc call.\n");
        return -1;
      }
      else{
        return 0;
      }
    }
    else if (ptrAddr < ptrToNode->ptrAddr)
      ptrToNode = ptrToNode->smallerNode;
    else
      ptrToNode = ptrToNode->largerNode;
  }
  return 0;
}

// returns -1 when no exact ptr is found, 0 if found and sets it free
int memFreeExact(void **ptr, u_int flags, const char *filename, const char *fxname, int line_num){
  size_t ptrAddr = (size_t)(*ptr);
  memoryNode *ptrToNode = root;
  while(ptrToNode != NULL){
    if (ptrAddr == ptrToNode->ptrAddr){
      if (ptrToNode->allocated == 0){
        fprintf(stderr, "Warning: Double Free\n");
        fprintf(stderr, "You are attempting to free a pointer that has already been freed.\n");
        recordInsert(1, filename, fxname, line_num, ptr, 0, flags, NULL, -1, 2, 0);
        return -1;
      }
      else{
        ptrToNode->allocated = 0;
        ptrToNode->currRecord->allocated = 0;
        ptrToNode->currRecord->freeType = 0;
        ptrToNode->currRecord = NULL;
        free((void *)(ptrToNode->leftBound));
        recordInsert(1, filename, fxname, line_num, ptr, 0, flags, NULL, -1, 0, 0);
        return 0;
      }
    }
    else if (ptrAddr < ptrToNode->ptrAddr)
      ptrToNode = ptrToNode->smallerNode;
    else
      ptrToNode = ptrToNode->largerNode;
  }
  recordInsert(1, filename, fxname, line_num, ptr, 0, flags, NULL, -1, 1, 0);
  return -1;
}

// returns -1 when no approx ptr is found, 0 if found and sets it free
int memFreeApprox(void **ptr, u_int flags, const char *filename, const char *fxname, int line_num){
  size_t ptrAddr = (size_t)(*ptr);
  memoryNode *ptrToNode = root;
  while(ptrToNode != NULL){
    if (ptrAddr <= ptrToNode->rightBound && ptrAddr >= ptrToNode->leftBound){
      if (ptrToNode->allocated == 0){
        fprintf(stderr, "Warning: Double Free\n");
        fprintf(stderr, "You are attempting to free a pointer that has already been freed.\n");
        recordInsert(1, filename, fxname, line_num, ptr, 0, flags, NULL, -1, 2, 1);
        return -1;
      }
      else{
        ptrToNode->allocated = 0;
        ptrToNode->currRecord->allocated = 0;
        ptrToNode->currRecord->freeType = 1;
        ptrToNode->currRecord = NULL;
        free((void *)(ptrToNode->leftBound));
        recordInsert(1, filename, fxname, line_num, ptr, 0, flags, NULL, -1, 0, 1);
        return 0;
      }
    }
    else if (ptrAddr < ptrToNode->ptrAddr)
      ptrToNode = ptrToNode->smallerNode;
    else
      ptrToNode = ptrToNode->largerNode;
  }
  recordInsert(1, filename, fxname, line_num, ptr, 0, flags, NULL, -1, 1, 1);
  return -1;
}

static void freeCurrentNode(memoryNode *ptrToNode){
  
  if (ptrToNode->smallerNode != NULL)
    freeCurrentNode(ptrToNode->smallerNode);
  if (ptrToNode->largerNode != NULL)
    freeCurrentNode(ptrToNode->largerNode);
  free(ptrToNode);
}

void freeAllNodes(){
  if (root != NULL)
    freeCurrentNode(root);
}
