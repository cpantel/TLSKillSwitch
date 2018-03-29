
#include "pubkey-pin.h"

#define PUBKEY_PIN_VERBOSE 1
int flag;

int main(int argc, char* argv[])
{
    if (argc != 2) {
      printf("No destination\n");
      abort();
    }
    char target[1000];
    strncpy(target,argv[1],1000);

    int ret = 0, err = 0;
    
    SSL_CTX* ctx = NULL;
    int sock = 0;
    SSL* ssl = NULL;
    BIO* bio = NULL;
    
    do {
        
        /***************************/
        /* Part 1: SSL/TLS Context */
        /***************************/
        
        ctx = pkp_create_context();
        if(NULL == ctx) break; /* failed */
        
        /***************************/
        /* Part 2: TCP/IP Socket   */
        /***************************/
        
        sock = pkp_create_socket(target, HOST_PORT);
        if(sock < 0)  break; /* failed */
        
        /***************************/
        /* Part 3: SSL/TLS Channel */
        /***************************/
        
        ssl = pkp_create_channel(ctx, target);
        if(NULL == ssl)  break; /* failed */
        
        /***************************/
        /* Part 4: Input/Output    */
        /***************************/
        
        bio = pkp_create_bio(ssl, sock);
        if(NULL == ssl) break; /* failed */
        
        /***************************/
        /* Part 5: Secure Connect  */
        /***************************/
        
        ret = pkp_ssl_connect(ssl);
        if(FALSE == ret) break; /* failed */
        
        /***************************/
        /* Part 6a: Verify         */
        /***************************/
        
        /* These must occur *whenever* you use OpenSSL. They are *not* */
        /* extra hardening. This is how you use the library correctly. */
        
        /* http://www.openssl.org/docs/ssl/SSL_get_verify_result.html */
        /* Error codes: http://www.openssl.org/docs/apps/verify.html  */
        
        long res = SSL_get_verify_result(ssl);
        ASSERT(X509_V_OK == res);
        if(X509_V_OK != res)
        {
            fprintf(stderr, "SSL_get_verify_results failed (%ld). Exiting.\n", res);
            /* break; failed */
        }
        
        /* SSL_get_verify_results() will return X509_V_OK even if no  */
        /* certifcate was sent by the server. We have to verify the   */
        /* existence of the certifcate now. Hence the reason for the  */
        /* fetch and immediate free.                                  */
        
        X509* cert = SSL_get_peer_certificate(ssl);
        X509_free(cert);
        
        ASSERT(NULL != cert);
        if(NULL == cert)
        {
            fprintf(stderr, "SSL_get_peer_certificate failed (no certificate). Exiting.\n");
            break; /* failed */
        }
        
        /***************************/
        /* Part 6b: Verify         */
        /***************************/
        
        /* OpenSSL prior to 1.1.0 (1.0.1, 0.9.8, etc) did not perform   */
        /* hostname validation. Left as an exercise for the reader.     */
        /* Use the cert from SSL_get_peer_certificate, extract the      */
        /* hostname with pkp_print_cn_name or pkp_print_san_name, and   */
        /* then perform the match against the hostname.                 */
        
        /***************************/
        /* Part 6c: Verify         */
        /***************************/
        
        /* Extra hardening so we can discard DNS and CA hearsay. If we   */
        /* pin, we can pretty much skip the tests in 'Part 6a: Verify'   */
        /* and 'Part 6b: Verify' since we can unequivocally identify     */
        /* the host through its public key.                              */
        
        ret = pkp_pin_peer_pubkey(ssl);
        if(FALSE == ret)
        {
            fprintf(stderr, "There is a problem with server's public key. Exiting.\n");
            break; /* failed */
        }
        
        /*************************/
        /*  Part 7: Fetch        */
        /*************************/
        if (flag == 0) {
             fprintf("Fallo la global\n");
             break;
        }
        fprintf(stdout, "The server's public key is expected. We are GO for launch.\n");
        
        /* Left as an exercise for the    */
        /* reader. Do it through a BIO.   */
        
    } while (0);
    
    // Cleaned up by SSL_{shutdown|free}
    //if(NULL != bio)
    //    BIO_free(bio);
    
    if(NULL != ssl) {
        SSL_shutdown(ssl);
        SSL_free(ssl);
    }
    
    if(sock >= 0)
        close(sock);
    
    if(NULL != ctx)
        SSL_CTX_free(ctx);
    
    return err;
}

/***************************************************************************/
/* Fetch the certificate from the website, extract the public key as a     */
/* DER encoded subjectPublicKeyInfo, and then compare it to the public key */
/* on file. The key on file is what we expect to get from the server.      */
/***************************************************************************/
int pkp_pin_peer_pubkey(SSL* ssl)
{
#if defined(PUBKEY_PIN_VERBOSE) && (PUBKEY_PIN_VERBOSE >= 1)
    fprintf(stdout, "pkp_pin_peer_pubkey\n");
#endif
    
    ASSERT(NULL != ssl);
    if(!(NULL != ssl)) return FALSE;
    
    X509* cert = NULL;
    FILE* fp = NULL;
    
    int len1 = 0, len2 = 0;
    unsigned char *buff1 = NULL, *buff2 = NULL;
    
    int ret = 0, result = FALSE;
    long ssl_err = 0;
    
    do
    {
        /* http://www.openssl.org/docs/ssl/SSL_get_peer_certificate.html */
        cert = SSL_get_peer_certificate(ssl);
        ssl_err = (long)ERR_get_error();
CHECKPOINT;
        ASSERT(cert != NULL);
        if(!(cert != NULL))
        {
            pkp_display_error("SSL_get_peer_certificate", ssl_err);
            break; /* failed */
        }
        
        /* Begin Gyrations to get the subjectPublicKeyInfo       */
        /* Thanks to Viktor Dukhovni on the OpenSSL mailing list */
        
        /* http://groups.google.com/group/mailing.openssl.users/browse_thread/thread/d61858dae102c6c7 */
        len1 = i2d_X509_PUBKEY(X509_get_X509_PUBKEY(cert), NULL);
        ssl_err = (long)ERR_get_error();
        
        ASSERT(len1 > 0);
        if(!(len1 > 0))
        {
            pkp_display_error("i2d_X509_PUBKEY (1)", ssl_err);
            break; /* failed */
        }
CHECKPOINT;        
        /* scratch */
        unsigned char* temp = NULL;
        
        /* http://www.openssl.org/docs/crypto/buffer.html */
        buff1 = temp = OPENSSL_malloc(len1);
        ssl_err = (long)ERR_get_error();
        
        ASSERT(buff1 != NULL);
        if(!(buff1 != NULL))
        {
            pkp_display_error("OpenSSL_malloc (1)", ssl_err);
            break; /* failed */
        }
 CHECKPOINT;       
        /* http://www.openssl.org/docs/crypto/d2i_X509.html */
        len2 = i2d_X509_PUBKEY(X509_get_X509_PUBKEY(cert), &temp);
        ssl_err = (long)ERR_get_error();
        
        /* These checks are making sure we got back the same values as when we sized the buffer.    */
        /* Its pretty weak since they should always be the same. But it gives us something to test. */
        ASSERT(len1 == len2);
        ASSERT(temp != NULL);
        ASSERT((temp - buff1) == len1);
        if(!((len1 == len2) && (temp != NULL) && ((temp - buff1) == len1)))
        {
            pkp_display_error("i2d_X509_PUBKEY (2)", ssl_err);
            break; /* failed */
        }
CHECKPOINT;     
        /* End Gyrations */
        
        /* We are getting ready to read (and possibly write) files. Be careful of fopen() and friends. */
        /* fopen_s() is not Posix; and "wx" and "rx" modes are C1X, and not generally available.       */
        /* You might find it best to use open(..., O_EXCL | O_RDONLY,... ) or                          */
        /*   open(..., O_CREAT | O_EXCL | O_WRONLY,... ) and friends.                                  */
        /* See, for example, http://pubs.opengroup.org/onlinepubs/7908799/xsh/open.html and            */
        /*   https://www.securecoding.cert.org/confluence/display/seccode/FIO03-C                      */
        /* You can remove this assert since it has gotten your attention, and you have been advised.   */
        // ASSERT(0);
        
        /* Write out the expected public key (useful for boot strapping during first run) */
#if defined(WRITE_REMOTE_KEY_FILE)
        BIO* file = BIO_new_file("seguro.com.xxx.der", "wx");
        if(NULL == file)
            file = BIO_new_file("seguro.com.xxx.der", "w");
        
        ASSERT(NULL != file);
        if(NULL != file) {
            ret = BIO_write(file, buff1, len1);
            ASSERT(ret == len1);
            
            BIO_free_all(file);
        }
#endif
        
        /* See the warning above!!!                                            */
        /* http://pubs.opengroup.org/onlinepubs/009696699/functions/fopen.html */
        fp = fopen("/home/carlos/Desktop/TLSKillSwitch/certificates/seguro.com.pubkey.bin", "rx");
        ssl_err = errno;
CHECKPOINT;        
        if(NULL ==fp) {
            fp = fopen("/home/carlos/Desktop/TLSKillSwitch/certificates/seguro.com.pubkey.bin", "r");
            ssl_err = errno;
        }
        
        ASSERT(NULL != fp);
        if(!(NULL != fp))
        {
            pkp_display_error("fopen", ssl_err);
            break; /* failed */
        }
        
        /* Seek to eof to determine the file's size                            */
        /* http://pubs.opengroup.org/onlinepubs/009696699/functions/fseek.html */
        ret = fseek(fp, 0, SEEK_END);
        ssl_err = errno;
CHECKPOINT;        
        ASSERT(0 == ret);
        if(!(0 == ret))
        {
            pkp_display_error("fseek (1)", ssl_err);
            break; /* failed */
        }
        
        /* Fetch the file's size                                               */
        /* http://pubs.opengroup.org/onlinepubs/009696699/functions/ftell.html */
        long size = ftell(fp);
        ssl_err = errno;
CHECKPOINT;        
        /* Arbitrary size, but should be relatively small (less than 1K or 2K) */
        ASSERT(size != -1 && size > 0 && size < 2048);
        if(!(size != -1 && size > 0 && size < 2048))
        {
            pkp_display_error("ftell", ssl_err);
            break; /* failed */
        }
        
        /* Rewind to beginning to perform the read                             */
        /* http://pubs.opengroup.org/onlinepubs/009696699/functions/fseek.html */
        ret = fseek(fp, 0, SEEK_SET);
        ssl_err = errno;
        
        ASSERT(0 == ret);
        if(!(0 == ret))
        {
            pkp_display_error("fseek (2)", ssl_err);
            break; /* failed */
        }
CHECKPOINT;        
        /* Re-use buff2 and len2 */
        buff2 = NULL; len2 = (int)size;
        
        /* http://www.openssl.org/docs/crypto/buffer.html */
        buff2 = OPENSSL_malloc(len2);
        ssl_err = (long)ERR_get_error();
        
        ASSERT(buff2 != NULL);
        if(!(buff2 != NULL))
        {
            pkp_display_error("OpenSSL_malloc (2)", ssl_err);
            break; /* failed */
        }
        
        /* http://pubs.opengroup.org/onlinepubs/009696699/functions/fread.html */
        /* Returns number of elements read, which should be 1 */
        ret = (int)fread(buff2, (size_t)len2, 1, fp);
        ssl_err = errno;
        
        ASSERT(ret == 1);
        if(!(ret == 1))
        {
            pkp_display_error("fread", ssl_err);
            break; /* failed */
        }
        
        /* Re-use size. MIN and MAX macro below... */
        size = len1 < len2 ? len1 : len2;
        
        /*****************/
        /*    PAYDIRT    */
        /*****************/
printf("len 1 vs size %d, %d\n", len1, (int)size);
printf("len 2 vs size %d, %d\n", len2, (int)size);
        ASSERT(len1 == (int)size);
        ASSERT(len2 == (int)size);
        ASSERT(0 == memcmp(buff1, buff2, (size_t)size));
 CHECKPOINT;       
        if(len1 != (int)size || len2 != (int)size || 0 != memcmp(buff1, buff2, (size_t)size))
        {
#if 0
            // This was needed on FEB 18 2013. It appears the public key changed a few
            // days earlier. And needed again in NOV 2013. So much for key continuity!
            BIO* file = BIO_new_file("random-org-received.der", "wx");
            if(NULL == file)
                file = BIO_new_file("random-org-received.der", "w");
            
            ASSERT(NULL != file);
            if(NULL != file) {
                ret = BIO_write(file, buff1, len1);
                ASSERT(ret == len1);
                
                BIO_free_all(file);
            }
#endif
            
            pkp_display_error("Public key is not expected", 0);
            break; /* failed */
        }
        
        /* The one good exit point */
        result = TRUE;
        
#if defined(PUBKEY_PIN_VERBOSE) && (PUBKEY_PIN_VERBOSE >= 1)
        fprintf(stdout, "  success\n");
#endif
        
    } while(0);
    
    if(fp != NULL)
        fclose(fp);
    
    /* http://www.openssl.org/docs/crypto/buffer.html */
    if(NULL != buff2)
        OPENSSL_free(buff2);
    
    /* http://www.openssl.org/docs/crypto/buffer.html */
    if(NULL != buff1)
        OPENSSL_free(buff1);
    
    /* http://www.openssl.org/docs/crypto/X509_new.html */
    if(NULL != cert)
        X509_free(cert);
    
    return result;
}

/***************************************************************************/
/* pkp_ssl_connect() is the first fun function. It will kick-off a         */
/* number of calls to verify_callback(). If verify_callback() returns 1,       */
/* then all verifcation errors are all ignored. In this case, verifcation  */
/* MUST occur via pkp_pin_peer_pubkey() (Part 6 above).                    */
/*                                                                         */
/* If verify_callback() returns 0, then SSL_connect() will return            */
/* SSL_ERROR_SSL and this function will fail. Extended error information   */
/* is then retireved via ERR_get_error(). If the extended information is   */
/* 0x14090086, then the error is "certificate verify failed."              */
/***************************************************************************/
int pkp_ssl_connect(SSL* ssl)
{
#if defined(PUBKEY_PIN_VERBOSE) && (PUBKEY_PIN_VERBOSE >= 1)
    fprintf(stdout, "pkp_ssl_connect\n");
#endif
    
    ASSERT(NULL != ssl);
    if(!(NULL != ssl)) return FALSE;
    
    int ret = FALSE;
    long ssl_err1 = 0, ssl_err2 = 0;
    
    /* http://www.openssl.org/docs/ssl/SSL_CTX_set_verify.html */
    SSL_set_verify_depth(ssl, 4);
    /* Cannot fail ??? */
    
    /* http://www.openssl.org/docs/ssl/SSL_connect.html */
    ret = SSL_connect(ssl);
    ssl_err1 = SSL_get_error(ssl, ret);
    ssl_err2 = (long)ERR_get_error();
    
    ASSERT(1 == ret);
    switch (ssl_err1)
    {
        case SSL_ERROR_NONE: /* success */
            // display_warning("SSL_connect: success", ssl_err);
            break;
        case SSL_ERROR_ZERO_RETURN:
            pkp_display_error("SSL_connect: SSL_ERROR_ZERO_RETURN", ssl_err1);
            break;
        case SSL_ERROR_WANT_READ:
            pkp_display_error("SSL_connect: SSL_ERROR_WANT_READ", ssl_err1);
            break;
        case SSL_ERROR_WANT_WRITE:
            pkp_display_error("SSL_connect: SSL_ERROR_WANT_WRITE", ssl_err1);
            break;
        case SSL_ERROR_WANT_CONNECT:
            pkp_display_error("SSL_connect: SSL_ERROR_WANT_CONNECT", ssl_err1);
            break;
        case SSL_ERROR_WANT_ACCEPT:
            pkp_display_error("SSL_connect: SSL_ERROR_WANT_ACCEPT", ssl_err1);
            break;
        case SSL_ERROR_WANT_X509_LOOKUP:
            pkp_display_error("SSL_connect: SSL_ERROR_WANT_X509_LOOKUP", ssl_err1);
            break;
        case SSL_ERROR_SYSCALL:
            pkp_display_error("SSL_connect: SSL_ERROR_SYSCALL", ssl_err1);
            break;
        case SSL_ERROR_SSL:
            /* If you get this error, try `openssl errstr 0xNNNN` to    */
            /* decode the OpenSSL error using the error string utility. */
            pkp_display_error("SSL_connect: SSL_ERROR_SSL", ssl_err2);
            break;
        default:
            pkp_display_error("SSL_connect: unknown failure", ssl_err1);
            break;
    }
    
    /* Fixup `ret` for TRUE/FALSE return */
    ret == 1 ? (ret = TRUE) : (ret = FALSE);
    printf("FLAG %d\n",flag);
    return ret;
}

void pkp_print_cn_name(const char* label, X509_NAME* const name, int nid)
{
    int idx = -1, success = 0;
    unsigned char *utf8 = NULL;
    
    do
    {
        if(!name) break; /* failed */
        
        idx = X509_NAME_get_index_by_NID(name, nid, -1);
        if(!(idx > -1))  break; /* failed */
        
        X509_NAME_ENTRY* entry = X509_NAME_get_entry(name, idx);
        if(!entry) break; /* failed */
        
        ASN1_STRING* data = X509_NAME_ENTRY_get_data(entry);
        if(!data) break; /* failed */
        
        int length = ASN1_STRING_to_UTF8(&utf8, data);
        if(!utf8 || !(length > 0))  break; /* failed */
        
        fprintf(stdout, "  %s: %s\n", label, utf8);
        success = 1;
        
    } while (0);
    
    if(utf8)
        OPENSSL_free(utf8);
    
    if(!success)
        fprintf(stdout, "  %s: <not available>\n", label);
}

void pkp_print_san_name(const char* label, X509* const cert, int nid)
{
    unsigned char* utf8 = NULL;
    int success = 0;
    
    do
    {
        GENERAL_NAMES* names = NULL;
        
        names = X509_get_ext_d2i(cert, nid, 0, 0 );
        if(!names) break;
        
        int i = 0, numAN = sk_GENERAL_NAME_num(names);
        if(!numAN) break; /* failed */
        
        for( i = 0; i < numAN; ++i )
        {
            GENERAL_NAME* entry = sk_GENERAL_NAME_value(names, i);
            if(!entry) continue;
            
            if(GEN_DNS == entry->type)
            {
                int len1 = 0, len2 = 0;
                
                len1 = ASN1_STRING_to_UTF8(&utf8, entry->d.dNSName);
                if(utf8) {
                    len2 = (int)strlen((const char*)utf8);
                }
                
                if(len1 != len2) {
                    /* Moxie Marlinspike is in the room .... */
                    fprintf(stderr, "  Strlen and ASN1_STRING size do not match (embedded nul?): %d vs %d\n", len2, len1);
                    
                }
                
                /* If there's a problem with string lengths, then     */
                /* we skip the candidate and move on to the next.     */
                /* Another policy would be to fails since it probably */
                /* indicates the client is under attack.              */
                if(utf8 && len1 && len2 && (len1 == len2)) {
                    fprintf(stdout, "  %s: %s\n", label, utf8);
                    success = 1;
                }
                
                if(utf8) {
                    OPENSSL_free(utf8), utf8 = NULL;
                }
            }
            else
            {
                fprintf(stderr, "  Unknown GENERAL_NAME type: %d\n", entry->type);
            }
        }
        
    } while (0);
    
    if(utf8)
        OPENSSL_free(utf8);
    
    if(!success)
        fprintf(stdout, "  %s: <not available>\n", label);
    
}

/*************************************************************************/
/* `preverify` is the result of OpenSSL's internal verification. 1 is    */
/* success, and 0 is failure. The internal tests consist of customary    */
/* X509 checks, such as signature/trust chain, not before date, and not  */
/* after date.                                                           */
/*                                                                       */
/* We can ignore it if we are using public keys for identity and         */
/* authentication of the server. If we ignore, verifcation MUST occur    */
/* via pkp_pin_peer_pubkey() (Part 6 above).                             */
/*                                                                       */
/* Or we can observe it...                                               */ 
/*************************************************************************/
int verify_callback(int preverify, X509_STORE_CTX* x509_ctx)
{
    /* For error codes, see http://www.openssl.org/docs/apps/verify.html  */
    flag = 14;
    int depth = X509_STORE_CTX_get_error_depth(x509_ctx);
    int err = X509_STORE_CTX_get_error(x509_ctx);
    
    X509* cert = X509_STORE_CTX_get_current_cert(x509_ctx);
    X509_NAME* iname = cert ? X509_get_issuer_name(cert) : NULL;
    X509_NAME* sname = cert ? X509_get_subject_name(cert) : NULL;
    
    fprintf(stdout, "verify_callback (depth=%d)(preverify=%d)\n", depth, preverify);
    
    /* Issuer is the authority we trust that warrants nothing useful */
    pkp_print_cn_name("Issuer (cn)", iname, NID_commonName);
    
    /* Subject is who the certificate is issued to by the authority  */
    pkp_print_cn_name("Subject (cn)", sname, NID_commonName);
    
    if(depth == 0)
    {
        /* If depth is 0, its the server's certifcate. Print the SANs */
        pkp_print_san_name("Subject (san)", cert, /*sname,*/ NID_subject_alt_name);
    }
    
    if(preverify == 0)
    {
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
    
    /***************************************************************************/
    /* We really don't care what DNS and CAs have to say here. We are going to */
    /* pin the public key after customary SSL/TLS validations. As far as we    */
    /* are concerned, DNS and CAs are untrusted input whose sole purpose in    */
    /* life is to make us fail in spectacular ways.                            */
    /***************************************************************************/
    
    /* If we return 1, then we base all security decisions on the public key  */
    /* of the host. The decision is made above with pkp_pin_peer_pubkey()     */
    /* return 1; */
    
    /* Or, we can return the result of `preverify`, which is the result of    */
    /* customary X509 verifcation. We should still make our final security    */
    /* decision based upon pkp_pin_peer_pubkey().                             */
    return preverify;
}
