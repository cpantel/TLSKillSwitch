#ifndef __PUBKEY_PIN_INCLUDED__
#define __PUBKEY_PIN_INCLUDED__

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
#include <openssl/pem.h>
#include <openssl/conf.h>
#include <openssl/x509.h>
#include <openssl/buffer.h>
#include <openssl/x509v3.h>
#include <openssl/opensslconf.h>

#ifndef UNUSED
# define UNUSED(x) ((void)(x))
#endif

#ifndef TRUE
# define TRUE 1
#endif

#ifndef FALSE
# define FALSE 0
#endif

#ifndef HOST_NAME
# define HOST_NAME "www.random.org"
#endif

#ifndef HOST_PORT
# define HOST_PORT "443"
#endif

#ifndef HOST_RESOURCE
# define HOST_RESOURCE "/"
#endif

/*******************************************/
/* Diagnostics and ASSERT                  */
/*******************************************/

#if !defined(NDEBUG)

#include <signal.h>

static void NullTrapHandler(int unused) { UNUSED(unused); }



#  define ASSERT(x) { \
  if(!(x)) { \
    fprintf(stderr, "Assertion: %s: function %s, line %d\n", (char*)(__FILE__), (char*)(__func__), (int)__LINE__); \
    raise(SIGTRAP); \
  } \
}

#else

#  define ASSERT(x) UNUSED(x)

#endif // !defined(NDEBUG)

#endif // __PUBKEY_PIN_INCLUDED__
