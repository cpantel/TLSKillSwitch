/*********************************************************************/
/*********************************************************************/
/* HELPER FUNCTIONS, useless for teaching but needed for the program */
/*********************************************************************/
/*********************************************************************/

#include "pubkey-pin.h"

void pkp_display_error(const char* msg, long err)
{
    if(err == 0x14090086)
        fprintf(stderr, "Error: %s: SSL3_GET_SERVER_CERTIFICATE: certificate verify failed (%ld, %lx)\n", msg, err, err);
    else if (err == 0)
        fprintf(stderr, "Error: %s\n", msg);
    else if(err < 10)
        fprintf(stderr, "Error: %s: %ld\n", msg, err);
    else
        fprintf(stderr, "Error: %s: %ld (%lx)\n", msg, err, err);
}

void pkp_display_warning(const char* msg, long err)
{
    if(err < 10)
        fprintf(stdout, "Warning: %s: %ld\n", msg, err);
    else
        fprintf(stdout, "Warning: %s: %ld (%lx)\n", msg, err, err);
}

void pkp_display_info(const char* msg)
{
    fprintf(stdout, "Info: %s\n", msg);
}

BIO* pkp_create_bio(SSL* ssl, int sock)
{
#if defined(PUBKEY_PIN_VERBOSE) && (PUBKEY_PIN_VERBOSE >= 1)
    fprintf(stdout, "pkp_create_bio\n");
#endif
    
    ASSERT(NULL != ssl);
    ASSERT(sock >= 0);
    if(!((NULL != ssl) && (sock >= 0))) return NULL;
    
    unsigned long ssl_err = 0;
    BIO* bio = NULL;
    
    do
    {
        /* http://www.openssl.org/docs/crypto/BIO_s_socket.html */
        bio = BIO_new_socket(sock, BIO_NOCLOSE);
        ssl_err = (unsigned long)(long)errno;
        
        ASSERT(NULL != bio);
        if(NULL == bio)
        {
            pkp_display_error("BIO_new_socket", (long)ssl_err);
            break; /* failed */
        }
        
        /* http://www.openssl.org/docs/ssl/SSL_set_bio.html */
        SSL_set_bio(ssl,bio,bio);
        /* Cannot fail ??? */
        
    } while (0);
    
    return bio;
}

SSL* pkp_create_channel(SSL_CTX* ctx)
{
#if defined(PUBKEY_PIN_VERBOSE) && (PUBKEY_PIN_VERBOSE >= 1)
    fprintf(stdout, "pkp_create_channel\n");
#endif
    
    ASSERT(NULL != ctx);
    if(!(NULL != ctx)) return NULL;
    
    unsigned long ssl_err = 0;
    long ret = 0;
    
    /* http://www.openssl.org/docs/ssl/SSL_new.html */
    SSL* ssl = SSL_new(ctx);
    ssl_err = (unsigned long)(long)errno;
    
    ASSERT(NULL != ssl);
    if(NULL == ssl)
    {
        pkp_display_error("SSL_new", (long)ssl_err);
        return NULL;
    }
    
    /* Cipher suites, https://www.openssl.org/docs/apps/ciphers.html */
#if 0
    const char* const PREFERRED_CIPHERS = "kEECDH:kEDH:kRSA:AESGCM:AES256:AES128:3DES:SHA256:SHA84:SHA1:!aNULL:!eNULL:!EXP:!LOW:!MEDIUM!ADH:!AECDH";
#endif
    
#if 0
    const char* const PREFERRED_CIPHERS = "ALL:!aNULL:!eNULL:!EXP:!LOW";
#endif
    
#if 0
    const char* const PREFERRED_CIPHERS = NULL;
#endif
    
#if 1
    const char* PREFERRED_CIPHERS =
    
    /* TLS 1.2 only */
    "ECDHE-ECDSA-AES256-GCM-SHA384:"
    "ECDHE-RSA-AES256-GCM-SHA384:"
    "ECDHE-ECDSA-AES128-GCM-SHA256:"
    "ECDHE-RSA-AES128-GCM-SHA256:"
    
    /* TLS 1.2 only */
    "DHE-DSS-AES256-GCM-SHA384:"
    "DHE-RSA-AES256-GCM-SHA384:"
    "DHE-DSS-AES128-GCM-SHA256:"
    "DHE-RSA-AES128-GCM-SHA256:"
    
    /* TLS 1.0 only */
    "DHE-DSS-AES256-SHA:"
    "DHE-RSA-AES256-SHA:"
    "DHE-DSS-AES128-SHA:"
    "DHE-RSA-AES128-SHA:"
    
    /* SSL 3.0 and TLS 1.0 */
    "EDH-DSS-DES-CBC3-SHA:"
    "EDH-RSA-DES-CBC3-SHA:"
    "DH-DSS-DES-CBC3-SHA:"
    "DH-RSA-DES-CBC3-SHA";
#endif
    
    /* https://www.openssl.org/docs/ssl/ssl.html#DEALING_WITH_PROTOCOL_CONTEXTS */
    /* https://www.openssl.org/docs/ssl/SSL_CTX_set_cipher_list.html            */
    ret = SSL_set_cipher_list(ssl, PREFERRED_CIPHERS);
    ssl_err = ERR_get_error();
    
    ASSERT(1 == ret);
    if(!(1 == ret))
    {
        pkp_display_error("SSL_set_cipher_list", (long)ssl_err);
    }
    
    /* No documentation. Check the source code for s_client.c                  */
    ret = SSL_set_tlsext_host_name(ssl, HOST_NAME);
    ssl_err = ERR_get_error();
    
    ASSERT(1 == ret);
    if(!(1 == ret))
    {
        /* Non-fatal, but who knows what cert might be served by an SNI server */
        /* (We actually know its the default site's cert in Apache...)         */
        pkp_display_error("SSL_set_tlsext_host_name", (long)ssl_err);
    }
    
    return ssl;
}

int pkp_create_socket(const char* host, const char* port)
{
#if defined(PUBKEY_PIN_VERBOSE) && (PUBKEY_PIN_VERBOSE >= 1)
    fprintf(stdout, "pkp_create_socket\n");
#endif
    
    int ret = 0, socketfd = -1;
    long ssl_err = 0;
    
    struct addrinfo hints;
    memset(&hints, 0x00, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_ADDRCONFIG | AI_NUMERICSERV;
    hints.ai_protocol = IPPROTO_TCP;
    
    struct addrinfo* result = NULL;
    
    do
    {
        /* http://linux.die.net/man/3/gethostbyname */
        ret = getaddrinfo(host, port, &hints, &result);
        ssl_err = ret;
        
        ASSERT(0 == ret);
        if(!(0 == ret))
        {
            pkp_display_error("getaddrinfo (1)", (long)ssl_err);
            break; /* failed */
        }
        
        ASSERT(NULL != result);
        if(NULL == result)
        {
            pkp_display_error("getaddrinfo (2)", ENOMEM);
            break; /* failed */
        }
        
        /* Loop over the result set, looking for an acceptable match */
        struct addrinfo* rr = result;
        while(rr != NULL)
        {
            /* Attempt to create the socekt. This is where the rubber meets the road */
            socketfd = socket(rr->ai_family, rr->ai_socktype, rr->ai_protocol);
            ssl_err = errno;
            
            if(socketfd >= 0) break;
            
            rr = rr->ai_next;
        }
        
        ASSERT(socketfd >= 0);
        if(!(socketfd >= 0))
        {
            pkp_display_error("socket", ssl_err);
            break; /* failed */
        }
        
        /* Non fatal since carrier/device stacks often don't allow */
        /* changing settings such as buffer sizes and timeouts     */
        struct timeval tv = {10 /*seconds*/, 0 /*microseconds*/};
        ret = setsockopt(socketfd, SOL_SOCKET, SO_SNDTIMEO, (char*)&tv, sizeof(tv));
        ssl_err = errno;
        
        ASSERT(0 == ret);
        if(!(0 == ret))
        {
            pkp_display_warning("setsockopt (1)", ssl_err);
        }
        
        /* Non fatal since carrier/device stacks often don't allow */
        /* changing settings such as buffer sizes and timeouts     */
        ret = setsockopt(socketfd, SOL_SOCKET, SO_RCVTIMEO, (char*)&tv, sizeof(tv));
        ssl_err = errno;
        
        ASSERT(0 == ret);
        if(!(0 == ret))
        {
            pkp_display_warning("setsockopt (2)", ssl_err);
        }
        
        /* Attempt to connect */
        ret = connect(socketfd, rr->ai_addr, rr->ai_addrlen);
        ssl_err = errno;
        
        ASSERT(0 == ret);
        if(0 != ret)
        {
            close(socketfd);
            socketfd = -1;
            
            pkp_display_error("connect", (long)ssl_err);
            break;
        }
        
        /* If we got this far, its success */
        
#if defined(PUBKEY_PIN_VERBOSE) && (PUBKEY_PIN_VERBOSE >= 1)
        fprintf(stdout, "  success\n");
#endif
        
    } while(0);
    
    if(NULL != result)
        freeaddrinfo(result);
    
    return socketfd;
}

SSL_CTX* pkp_create_context(void)
{
#if defined(PUBKEY_PIN_VERBOSE) && (PUBKEY_PIN_VERBOSE >= 1)
    fprintf(stdout, "pkp_create_context\n");
#endif
    
    int ret = 0;
    unsigned long ssl_err = 0;
    
    SSL_CTX* ctx = NULL;
    
    do
    {
        /* http://www.openssl.org/docs/ssl/SSL_library_init.html */
        ret = SSL_library_init();
        ssl_err = ERR_get_error();
        
        ASSERT(1 == ret);
        if(!(1 == ret))
        {
            pkp_display_error("SSL_library_init", (long)ssl_err);
            break; /* failed */
        }
        
        /* SSLv23_method() is 'everything' */
        const SSL_METHOD* method = SSLv23_method();
        ssl_err = ERR_get_error();
        
        ASSERT(NULL != method);
        if(!(NULL != method))
        {
            pkp_display_error("SSLv23_method", (long)ssl_err);
            break; /* failed */
        }
        
        /* http://www.openssl.org/docs/ssl/ctx_new.html */
        ctx = SSL_CTX_new(method);
        ssl_err = ERR_get_error();
        
        ASSERT(ctx != NULL);
        if(!(ctx != NULL))
        {
            pkp_display_error("SSL_CTX_new", (long)ssl_err);
            break; /* failed */
        }
        
        /* Enable standard certificate validation and our callback */
        /* https://www.openssl.org/docs/ssl/ctx_set_verify.html */
        SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER, pkp_verify_cb);
        /* Cannot fail ??? */
        
        /* Remove most egregious */
        const long flags = SSL_OP_ALL | SSL_OP_NO_SSLv2 | SSL_OP_NO_SSLv3 | SSL_OP_NO_COMPRESSION;
        long old_opts = SSL_CTX_set_options(ctx, flags);
        UNUSED(old_opts);
        
        /* http://www.openssl.org/docs/ssl/SSL_CTX_load_verify_locations.html */
        ret = SSL_CTX_load_verify_locations(ctx, "random-org-chain.pem", NULL);
        ssl_err = ERR_get_error();
        
        /* Non-fatal */
        ASSERT(1 == ret);
        if(!(1 == ret))
        {
            pkp_display_warning("SSL_CTX_load_verify_locations", (long)ssl_err);
            // break; /* failed */
        }
        
#if defined(PUBKEY_PIN_VERBOSE) && (PUBKEY_PIN_VERBOSE >= 1)
        fprintf(stdout, "  success\n");
#endif
        
    } while(0);
    
    return ctx;
}
