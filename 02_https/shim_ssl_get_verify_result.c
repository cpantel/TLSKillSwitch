#include <stdio.h>

#include "common.h"

int SSL_library_init(void) {
   header("Cheating SSL_library_init VERSION 1");
   void * handle = dlopen(LIBSSL_SO, RTLD_LAZY);
   int (*original_SSL_library_init) (void) = dlsym(handle, "SSL_library_init");
   return (*original_SSL_library_init)(); 
}

long SSL_get_verify_result(const SSL *ssl) {
   void * handle = dlopen(LIBSSL_SO, RTLD_LAZY);
   long (*original_SSL_get_verify_result) (const SSL *ssl) = dlsym(handle, "SSL_get_verify_result");
   long result = (*original_SSL_get_verify_result)(ssl);
   fprintf(stderr, "\n  ########## cheating SSL_get_verify_result -> %ld\n\n", result);
   return X509_V_OK;
}
