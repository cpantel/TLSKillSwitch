#include <stdlib.h>
#include <stdio.h>
long int fixedRandom[] = { 
   1217152472,
   7261664563,
   315960093,
   695304200
};

long int random(void) {
   static int idx=0;
   if (idx + 1 > sizeof(fixedRandom) / sizeof(long int)) {
      idx = 0;
   }
   return fixedRandom[idx++];   
}
