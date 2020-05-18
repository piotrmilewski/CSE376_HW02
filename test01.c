// include std library headers
#include <stdio.h>
#include <stdlib.h>

// include my library
#include "lkmalloc.h"

int main(int argc, char **argv){

  on_exit(onExitFunc, (void *)(intptr_t)1);

  printf("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");

  printf("\nRunning test script #1...\n");
  printf("This script checks lkmalloc and its respective flags\n");

  printf("\nPrint lkreport with all flags except 0x0 to show no current records:\n");
  lkreport(1, 0x1 | 0x2 | 0x4 | 0x8 | 0x10 | 0x20);
  
  printf("\nRunning 2 malloc(0x0) and then 2 free(0x0) calls...\n");

  void *ptr = NULL;
  lkmalloc(10, &ptr, 0x0);
  void *ptr2 = NULL;
  lkmalloc(12, &ptr2, 0x0);

  lkfree(&ptr, 0x0);
  lkfree(&ptr2, 0x0);

  printf("Then printing out perfectly matching alloc/free pairs with lkreport(0x2):\n");
  lkreport(1, 0x2);

  printf("\nNow let's check the other malloc flags and free them with free(0x0)...\n");

  void *ptr3 = NULL;
  lkmalloc(10, &ptr3, 0x1);

  printf("\nChecking if malloc of size 10 with flag LKM_INIT set memory to 0s...\n");
  
  char *ptr3Char = (char *)ptr3;
  for (int i = 0; i < 10; i++)
    printf("%d, ", *(ptr3Char+i));

  printf("\nThe pointer was of size 10 and we can see that all 10 indices are zero'd\n");

  printf("\nNow let's check if the LKM_OVER and LKM_UNDER flags work with malloc of size 5...\n");

  void *ptr4 = NULL;
  lkmalloc(5, &ptr4, 0x2 | 0x4);

  printf("\nChecking if the appropriate patterns are seen...\n");

  char *ptr4Char = (char *)ptr4 - 8;
  for (int i = 0; i < (5+16); i++)
    printf("%x, ", *(ptr4Char+i));

  printf("\n\nThis concludes the test script\n");
  printf("Will exit normally and show that lkreport shows memory leaks since last 2 malloc");
  printf(" calls were not freed\n");

  return 0;
}
