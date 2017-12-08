echo
read -p "wget a falso $1 con cada shim"
ping "$1" -c 1 | grep from
echo

wget "https://$1" -O -

read -p "............................................................................................"
gcc -Wall -fpic -shared -o shim.so shim_ssl_get_verify_result.c common.c -ldl -lssl
LD_PRELOAD=./shim.so wget "https://$1" -O -

read -p "............................................................................................"
gcc -Wall -fpic -shared -o shim.so shim_ssl_ctx_set_verify.c common.c -ldl -lssl
LD_PRELOAD=./shim.so wget "https://$1" -O -

read -p "............................................................................................"
gcc -Wall -fpic -shared -o shim.so shim_ssl_ctx_set_verify_short.c common.c -ldl -lssl
LD_PRELOAD=./shim.so wget "https://$1" -O -
