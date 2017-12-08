#include <stdlib.h>
#include <stdio.h>
#include <gnu/lib-names.h>
#include <dlfcn.h>


long int random(void) {

   static int count = 0;
   if (count > 3) {
      count = 0;
      return 66666666;
   }
   ++count;

   void * handle = dlopen(LIBC_SO, RTLD_LAZY);
   long int (*original_random) () = dlsym(handle, "random");

   return  (*original_random)(); 
}
