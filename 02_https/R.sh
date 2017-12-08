echo
read -p "R a falso $1 con cada shim"
ping "$1" -c 1 | grep from
echo

R -e 'library(RCurl); getURLContent("https://'$1'")'

read -p "............................................................................................"
gcc -Wall -fpic -shared -o shim.so shim_ssl_get_verify_result.c common.c -ldl -lssl
LD_PRELOAD=./shim.so R -e 'library(RCurl); getURLContent("https://'$1'")'

read -p "............................................................................................"
gcc -Wall -fpic -shared -o shim.so shim_ssl_ctx_set_verify.c common.c -ldl -lssl
LD_PRELOAD=./shim.so R -e 'library(RCurl); getURLContent("https://'$1'")'

read -p "............................................................................................"
gcc -Wall -fpic -shared -o shim.so shim_ssl_ctx_set_verify_short.c common.c -ldl -lssl
LD_PRELOAD=./shim.so R -e 'library(RCurl); getURLContent("https://'$1'")'

