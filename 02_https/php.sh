echo
read -p "PHP a falso  $1 con cada shim"
ping "$1" -c 1 | grep from
echo

php -r 'echo file_get_contents("https://www.clarin.com");'

read -p "............................................................................................"
LD_PRELOAD=../tls_shims/shim_v1.so  php -r 'echo file_get_contents("https://'$1'");'

read -p "............................................................................................"
LD_PRELOAD=../tls_shims/shim_v2.so  php -r 'echo file_get_contents("https://'$1'");'

read -p "............................................................................................"
LD_PRELOAD=../tls_shims/shim_v3.so  php -r 'echo file_get_contents("https://'$1'");'

