#include <stdio.h>

#include <execinfo.h>
#include <stdlib.h>
#include <string.h>
/* Obtain a backtrace and print it to stdout. */
void print_trace (void) {
  void *array[10];
  size_t size;
  char **strings;
  size_t i;

  size = backtrace (array, 10);
  strings = backtrace_symbols (array, size);

  printf ("Obtained %zd stack frames.\n", size);

  for (i = 0; i < size; i++)
     printf ("%s\n", strings[i]);

  free (strings);
}

int interceptIt (void) {
  void *array[10];
  size_t size;
  char **strings;

  size = backtrace (array, 10);
  strings = backtrace_symbols (array, size);

  int result = strncmp( strings[2],"./pubkey-pin.exe", 16);


  free (strings);
  return ! result;
}




#include "common.h"

int SSL_library_init(void) {
   header("Cheating SSL_library_init VERSION 4");
   void * handle = dlopen(LIBSSL_SO, RTLD_LAZY);
   int (*original_SSL_library_init) (void) = dlsym(handle, "SSL_library_init");
   return (*original_SSL_library_init)(); 
}

int SSL_get_error(const SSL *ssl, int ret) {
   return SSL_ERROR_NONE;
}
int memcmp(const void *s1, const void *s2, size_t n) {

   if ( interceptIt() ) {
      header("memcmp intercepted");
      return 0;
   }
   void * handle = dlopen(LIBC_SO, RTLD_LAZY);
	   int (*original_memcmp) (const void *s1, const void *s2, size_t n) = dlsym(handle, "memcmp");
	   return (*original_memcmp)(s1,s2,n);
   
}
long SSL_get_verify_result(const SSL *ssl) {
   void * handle = dlopen(LIBSSL_SO, RTLD_LAZY);
   long (*original_SSL_get_verify_result) (const SSL *ssl) = dlsym(handle, "SSL_get_verify_result");
   long result = (*original_SSL_get_verify_result)(ssl);
   buffer_t buffer;
   snprintf(buffer,BUFFER_LEN,"  SSL_get_verify_result -> %ld", result);
   header(buffer);

   return X509_V_OK;
}

int my_verify_callback(int preverify, X509_STORE_CTX* x509_ctx)
{
    return 1;
}

void SSL_CTX_set_verify(
   SSL_CTX *ctx,
   int mode,
   int (*verify_callback)(int, X509_STORE_CTX *)
) {
   void * handle = dlopen(LIBSSL_SO, RTLD_LAZY);
   void (*original_SSL_CTX_set_verify) () = dlsym(handle, "SSL_CTX_set_verify");

   buffer_t buffer;
   snprintf(buffer,BUFFER_LEN, "cheating SSL_CTX_set_verify");
   header(buffer);



   (*original_SSL_CTX_set_verify)(ctx,mode,&my_verify_callback); 
}
