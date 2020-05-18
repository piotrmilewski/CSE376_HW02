// include std library headers
#include <stdio.h>
#include <stdlib.h>

// include my library
#include "lkmalloc.h"

int main(int argc, char **argv){

  on_exit(onExitFunc, (void *)(intptr_t)1);

  printf("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");

  printf("\nRunning test script #2...\n");
  printf("This script checks lkfree and its respective flags\n");

  printf("\nPrint lkreport with all flags except 0x0 to show no current records:\n");
  lkreport(1, 0x1 | 0x2 | 0x4 | 0x8 | 0x10 | 0x20);

  printf("\nRunning a malloc call and a free call with flag LKF_REG\n");
  void *ptr = NULL;
  lkmalloc(10, &ptr, 0x2 | 0x4);
  lkfree(&ptr, 0x0);

  printf("\nPrinting lkreport with flag LKR_MATCH to check if properly freed\n");
  lkreport(1, 0x2);

  printf("\nRunning a malloc call and a free call with flag LKR_APPROX\n");
  void *ptr2 = NULL;
  lkmalloc(10, &ptr2, 0x2);
  ptr2 = ptr2+2;
  lkfree(&ptr2, 0x1);

  printf("\nPrinting lkreport with flag LKR_APPROX to check if properly freed\n");
  lkreport(1, 0x20);

  printf("\nRunning a malloc and free call to check flag LKR_WARN\n");
  void *ptr3 = NULL;
  lkmalloc(10, &ptr3, 0x2);
  ptr3 = ptr3+3;
  lkfree(&ptr3, 0x1 | 0x2);

  printf("\nPrinting lkreport with flag LKR_APPROX to check if properly freed\n");
  lkreport(1, 0x20);

  printf("\nRunning a free call w/o a malloc'd ptr to check flag LKR_UNKNOWN\n");
  void *ptr4 = NULL;
  lkfree(&ptr4, 0x4);

  printf("\nRunning a free call w/o a malloc'd ptr to check flag LKR_ERROR\n");
  printf("The test script will terminate with this free call and print out lkreport\n");
  void *ptr5 = NULL;
  lkfree(&ptr5, 0x8);

  printf("Shouldn't see this if LKR_ERROR exited\n");

  return 0;
}
