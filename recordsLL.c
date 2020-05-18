// includes 
#include "recordsLL.h"

static record *root = NULL;
static record *tail = NULL;

record *recordInsert(int record_type, const char *filename, const char *fxname, int line_num,
                  void *ptr, int retval, u_int size_or_flags, void *alloc_addr_returned,
                  int allocated, int freeStatus, int freeType){
  // create new record
  record *new = (record *)malloc(sizeof(record));
  new->record_type = record_type;
  new->filename = filename;
  new->fxname = fxname;
  new->line_num = line_num;
  gettimeofday(&(new->tv), NULL); // set tv to time
  new->ptr = ptr;
  new->retval = retval;
  new->size_or_flags = size_or_flags;
  new->alloc_addr_returned = alloc_addr_returned;
  new->allocated = allocated;
  new->freeStatus = freeStatus;
  new->freeType = freeType;
  new->nextRecord = NULL;
  // insert new record
  if (root == NULL){
    root = new;
    tail = new;
  }
  else{
    tail->nextRecord = new;
    tail = new;
  }
  return new;
}

// fd is a file that has already been open for writing, just need to send it info
// tags for mallocs: LKR_SERIOUS, LKR_MATCH, LKR_APPROX
// LKR_SERIOUS determined by if BST allocated is 1
// LKR_MATCH determined by if BST allocated is 0
// LKR_APPROX determined by if BST allocated is 0
// tags for frees: LKR_MATCH, LKR_BAD_FREE, LKR_ORPHAN_FREE, LKR_DOUBLE_FREE, LKR_APPROX
// LKR_MATCH determined by if record freeStatus = 0 and freeType = 0
// LKR_BAD_FREE determined by if record freeStatus = 0 and freeType = 1
// LKR_ORPHAN_FREE determined by if record freeStatus = 1
// LKR_DOUBLE_FREE determined by if record freeStatus = 2
// LKR_APPROX determined by LKR_BAD_FREE
// returns 0 if passed, -1 if some operation failed
int dumpReport(int fd, int serious, int match, int badFree, int orphanFree, 
               int doubleFree, int approx){
  int pageSize = getpagesize();
  record *node = root;
  char infoStr[pageSize];
  int retVal = 0;
  int sendInfo = 0; // if set to 1 then send info to fd
  // send out header for the CSV file
  strcpy(infoStr, "record_type,filename,fxname,line_num,timestamp,ptr_passed,retval,size_or_flags,alloc_addr_returned\n\0");
  if (write(fd, infoStr, strlen(infoStr)) < 0)
    return errno;
  // loop for each record in the linked list
  while (node != NULL){
    // determine whether to send info
    if (node->record_type == 0){ // malloc record
      if ((serious == 1 && node->allocated == 1) ||
          (match == 1 && node->allocated == 0 && node->freeType == 0) ||
          (approx == 1 && node->allocated == 0 && node->freeType == 1))
        sendInfo = 1;
    }
    else{ // free record
      if ((match == 1 && node->freeStatus == 0 && node->freeType == 0) ||
          (badFree == 1 && node->freeStatus == 0 && node->freeType == 1) ||
          (orphanFree == 1 && node->freeStatus == 1) ||
          (doubleFree == 1 && node->freeStatus == 2) ||
          (approx == 1 && node->freeStatus == 0 && node->freeType == 1))
        sendInfo = 1;
    }
    if (sendInfo == 1){
      // store the info into a string
      sprintf(infoStr, "%d,%s,%s,%d,%ld.%06ld,%p,%d,%d,%p\n%c", node->record_type,
              node->filename, node->fxname, node->line_num, node->tv.tv_sec, node->tv.tv_usec,
              node->ptr, node->retval, node->size_or_flags, node->alloc_addr_returned, '\0');
      // write the contents to a file
      if (write(fd, infoStr, strlen(infoStr)) < 0)
        return errno;
      retVal++;
    }
    sendInfo = 0; // reset sendInfo for next node
    node = node->nextRecord;
  }
  return retVal;
}

void freeAllRecords(){
  record *node;
  while (root != NULL){
    node = root; // store node to free
    root = root->nextRecord; // move to the next node
    free(node); // free the stored node
  }
}
