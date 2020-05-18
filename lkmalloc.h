#ifndef LKMALLOC
#define LKMALLOC

#include <stdint.h>

#define lkmalloc(size, ptr, flags) lkmallocI(size, ptr, flags, __FILE__, __func__, __LINE__)
#define lkfree(ptr, flags) lkfreeI(ptr, flags, __FILE__, __func__, __LINE__)

void onExitFunc(int exitStatus, void *arg);
int lkmallocI(u_int size, void **ptr, u_int flags, const char *filename, 
              const char *fxname, int line_num);
int lkfreeI(void **ptr, u_int flags, const char *filename,
           const char *fxname, int line_num);
int lkreport(int fd, u_int flags);

#endif
