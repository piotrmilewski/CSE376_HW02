// include std library headers
#include <stdio.h>
#include <stdlib.h>

// include my library
#include "lkmalloc.h"

int main(int argc, char **argv){

  on_exit(onExitFunc, (void *)(intptr_t)1);

  printf("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");

  printf("\nRunning test script #3...\n");
  printf("This script checks lkreport and its respective flags\n");

  printf("\nPrint lkreport with all flags except 0x0 to show no current records:\n");
  lkreport(1, 0x1 | 0x2 | 0x4 | 0x8 | 0x10 | 0x20);

  printf("\nPerforming a series of random malloc and free calls that will contain:\n");
  printf("memory leaks, perfectly matching alloc/free pairs, frees where the addr is in ");
  printf("the middle of alloc addr, orphan 'free's, double 'free's, and perfectly matching ");
  printf("alloc/free pairs that were freed due to LKF_APPROX\n");
  
  printf("\ncode to check LKR_SERIOUS is between lines %d", __LINE__);
  void *ptr0 = NULL;
  lkmalloc(10, &ptr0, 0x1);
  void *ptr1 = NULL;
  lkmalloc(60, &ptr1, 0x2);
  lkmalloc(40, &ptr1, 0x1);
  printf(" and %d\n", __LINE__);

  printf("code to check LKR_MATCH is between lines %d", __LINE__);
  void *ptr2 = NULL;
  lkmalloc(10, &ptr2, 0x0);
  void *ptr3 = NULL;
  lkmalloc(230, &ptr3, 0x2);
  void *ptr4 = NULL;
  lkmalloc(2, &ptr4, 0x4);
  lkfree(&ptr2, 0x0);
  lkfree(&ptr3, 0x0);
  lkfree(&ptr4, 0x0);
  printf(" and %d\n", __LINE__);

  printf("code to check LKR_BAD_FREE and LKR_APPROX is between %d", __LINE__);
  void *ptr5 = NULL;
  lkmalloc(5, &ptr5, 0x0);
  void *ptr6 = NULL;
  lkmalloc(71, &ptr6, 0x1);
  ptr5 = ptr5+2;
  ptr6 = ptr6+50;
  lkfree(&ptr5, 0x1);
  lkfree(&ptr6, 0x1);
  printf(" and %d\n", __LINE__);

  printf("code to check LKR_ORPHAN_FREE is between %d", __LINE__);
  void *ptr7 = NULL;
  void *ptr8 = NULL;
  lkfree(&ptr7, 0x0);
  lkfree(&ptr8, 0x1);
  printf(" and %d\n", __LINE__);

  printf("code to check LKR_DOUBLE_FREE is between %d", __LINE__);
  void *ptr9 = NULL;
  lkmalloc(11, &ptr9, 0x1);
  lkfree(&ptr9, 0x0);
  lkfree(&ptr9, 0x0);
  lkfree(&ptr9, 0x0);
  printf(" and %d\n", __LINE__);
  printf("malloc on line %d will show up as a perfect malloc\n", __LINE__ - 5);
  printf("free on line %d will show up as a perfect free\n", __LINE__ - 5);

  printf("\nprinting lkreport for LKR_SERIOUS...\n");
  lkreport(1, 0x1);

  printf("\nprinting lkreport for LKR_MATCH...\n");
  lkreport(1, 0x2);

  printf("\nprinting lkreport for LKR_BAD_FREE...\n");
  lkreport(1, 0x4);

  printf("\nprinting lkreport for LKR_ORPHAN_FREE...\n");
  lkreport(1, 0x8);

  printf("\nprinting lkreport for LKR_DOUBLE_FREE...\n");
  lkreport(1, 0x10);

  printf("\nprinting lkreport for LKR_APPROX...\n");
  lkreport(1, 0x20);

  printf("\n\nThis concludes the test script\n");
  printf("Will exit normally and show lkreport with the flag LKR_SERIOUS\n");

  return 0;
}
