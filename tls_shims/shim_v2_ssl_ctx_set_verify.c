#include <stdio.h>

#include "common.h"

int SSL_library_init(void) {
   header("Cheating SSL_library_init VERSION 2");
   void * handle = dlopen(LIBSSL_SO, RTLD_LAZY);
   int (*original_SSL_library_init) (void) = dlsym(handle, "SSL_library_init");
   return (*original_SSL_library_init)(); 
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

int my_verify_callback(int preverify, X509_STORE_CTX* x509_ctx) {
    // For error codes, see http://www.openssl.org/docs/apps/verify.html  

    int depth = X509_STORE_CTX_get_error_depth(x509_ctx);
    int err = X509_STORE_CTX_get_error(x509_ctx);

    X509* cert = X509_STORE_CTX_get_current_cert(x509_ctx);
    X509_NAME* iname = cert ? X509_get_issuer_name(cert) : NULL;
    X509_NAME* sname = cert ? X509_get_subject_name(cert) : NULL;

    buffer_t buffer;
    snprintf(buffer,BUFFER_LEN, "verify_callback (depth=%d)(preverify=%d)", depth, preverify);
    header(buffer);

    // Issuer is the authority we trust that warrants nothing useful 
    print_cn_name("Issuer (cn)", iname);

    // Subject is who the certificate is issued to by the authority 
    print_cn_name("Subject (cn)", sname);

    if(depth == 0) {
        // If depth is 0, its the server's certificate. Print the SANs
        print_san_name("Subject (san)", cert);
    }

    if(preverify == 0) {
        if(err == X509_V_ERR_UNABLE_TO_GET_ISSUER_CERT_LOCALLY)
            fprintf(stdout, "  Error = X509_V_ERR_UNABLE_TO_GET_ISSUER_CERT_LOCALLY\n");
        else if(err == X509_V_ERR_CERT_UNTRUSTED)
            fprintf(stdout, "  Error = X509_V_ERR_CERT_UNTRUSTED\n");
        else if(err == X509_V_ERR_SELF_SIGNED_CERT_IN_CHAIN)
            fprintf(stdout, "  Error = X509_V_ERR_SELF_SIGNED_CERT_IN_CHAIN\n");
        else if(err == X509_V_ERR_CERT_NOT_YET_VALID)
            fprintf(stdout, "  Error = X509_V_ERR_CERT_NOT_YET_VALID\n");
        else if(err == X509_V_ERR_CERT_HAS_EXPIRED)
            fprintf(stdout, "  Error = X509_V_ERR_CERT_HAS_EXPIRED\n");
        else if(err == X509_V_OK)
            fprintf(stdout, "  Error = X509_V_OK\n");
        else
            fprintf(stdout, "  Error = %d\n", err);
    }
    return 1;
    return preverify;

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
