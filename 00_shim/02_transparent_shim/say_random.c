#include <stdlib.h>
#include <stdio.h>

int main(int argc, char* argv[]) {

   FILE *fh = fopen("/dev/urandom","r");
   
   unsigned int seed;

   fgets((char *) &seed, 4, fh);

   srandom(seed);

   int i;
   for (i=0; i<10; ++i) {
      long int value = random();
      printf("Random %i %ld\n", i,value);
   }

   return 0;
}
