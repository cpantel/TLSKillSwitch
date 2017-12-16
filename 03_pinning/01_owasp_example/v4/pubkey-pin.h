#ifndef __PUBKEY_PIN_INCLUDED__
#define __PUBKEY_PIN_INCLUDED__

#define _POSIX_C_SOURCE 200112L

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <assert.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <netdb.h>

#include <openssl/ssl.h>
#include <openssl/bio.h>
#include <openssl/err.h>
#include <openssl/x509.h>
#include <openssl/buffer.h>
#include <openssl/pem.h>
#include <openssl/x509v3.h>

#ifndef UNUSED
# define UNUSED(x) ((void)(x))
#endif

#ifndef TRUE
# define TRUE 1
#endif

#ifndef FALSE
# define FALSE 0
#endif

#ifndef HOST_PORT
# define HOST_PORT "443"
#endif

/*******************************************/
/* Diagnostics and ASSERT                  */
/*******************************************/


//#  define ASSERT(x) UNUSED(x)

#define CHECKPOINT fprintf(stderr, "CHECKPOINT: %s: function %s, line %d\n", (char*)(__FILE__), (char*)(__func__), (int)__LINE__);

#  define ASSERT(x) { \
  if(!(x)) { \
    fprintf(stderr, "Assertion: %s: function %s, line %d\n", (char*)(__FILE__), (char*)(__func__), (int)__LINE__); \
  } \
}


/*********************************************/
/*   Functions for these compilation units   */
/*********************************************/

extern void pkp_display_info(const char* msg);
extern void pkp_display_warning(const char* msg, long err);
extern void pkp_display_error(const char* msg, long err);

extern SSL_CTX* pkp_create_context(void);
extern int pkp_create_socket(const char* host, const char* port);
extern SSL* pkp_create_channel(SSL_CTX* ctx, char * target);
extern BIO* pkp_create_bio(SSL* ssl, int sock);
extern int pkp_ssl_connect(SSL* ssl);
extern int pkp_pin_peer_pubkey(SSL* ssl);

/* https://www.openssl.org/docs/ssl/ctx_set_verify.html */
extern int verify_callback(int preverify, X509_STORE_CTX* x509_ctx);




#endif // __PUBKEY_PIN_INCLUDED__
