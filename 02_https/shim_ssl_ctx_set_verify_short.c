#include <stdio.h>

#include "common.h"

int SSL_library_init(void) {
   header("Cheating SSL_library_init VERSION 3");
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
   fputs("\n  ########## cheating SSL_CTX_set_verify \n\n", stderr);
   (*original_SSL_CTX_set_verify)(ctx,mode,&my_verify_callback); 
}
