gcc -Wall -fpic -shared -o shim_v1.so shim_v1_ssl_get_verify_result.c common.c -ldl -lssl
gcc -Wall -fpic -shared -o shim_v2.so shim_v2_ssl_ctx_set_verify.c common.c -ldl -lssl
gcc -Wall -fpic -shared -o shim_v3.so shim_v3_ssl_ctx_set_verify_short.c common.c -ldl 
