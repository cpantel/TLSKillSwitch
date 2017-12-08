#ifndef __common_h__
#define __common_h__

#include <gnu/lib-names.h>
#include <dlfcn.h>
#define LIBSSL_SO "/lib/x86_64-linux-gnu/libssl.so.1.0.0"

#include <openssl/ssl.h>
#include <openssl/bio.h>
#include <openssl/err.h>
#include <openssl/pem.h>
#include <openssl/conf.h>
#include <openssl/x509.h>
#include <openssl/buffer.h>
#include <openssl/x509v3.h>
#include <openssl/opensslconf.h>



void print_cn_name(const char* label, X509_NAME* const name);
void print_san_name(const char* label, X509* const cert);
void print_error_string(unsigned long err, const char* const label);
void header(const char* label);

#endif /* __common_h__ */