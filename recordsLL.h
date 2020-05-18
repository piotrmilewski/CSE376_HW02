#ifndef RECORDSLL
#define RECORDSLL

#include <sys/time.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

typedef struct record {
  int record_type;
  const char *filename;
  const char *fxname;
  int line_num;
  struct timeval tv;
  void *ptr;
  int retval;
  u_int size_or_flags;
  void *alloc_addr_returned;
  int allocated; // 0 = not allocated, 1 = allocated
  int freeStatus; // 0 = freed, 1 = orphan, 2 = double
  int freeType; // 0 = perfect, 1 = approx
  struct record *nextRecord;
} record;

record *recordInsert(int record_type, const char *filename, const char *fxname, int line_num,
                  void *ptr, int retval, u_int size_or_flags, void *alloc_addr_returned,
                  int allocated, int freeStatus, int freeType);
int dumpReport(int fd, int serious, int match, int badFree, int orphanFree, 
               int doubleFree, int approx);
void freeAllRecords();

#endif
