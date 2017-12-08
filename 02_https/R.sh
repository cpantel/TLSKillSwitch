echo
read -p "R a falso $1 con cada shim"
ping "$1" -c 1 | grep from
echo

R -e 'library(RCurl); getURLContent("https://'$1'")'

read -p "............................................................................................"
LD_PRELOAD=../tls_shims/shim_v1.so R -e 'library(RCurl); getURLContent("https://'$1'")'

read -p "............................................................................................"
LD_PRELOAD=../tls_shims/shim_v2.so R -e 'library(RCurl); getURLContent("https://'$1'")'

read -p "............................................................................................"
LD_PRELOAD=../tls_shims/shim_v3.so R -e 'library(RCurl); getURLContent("https://'$1'")'

