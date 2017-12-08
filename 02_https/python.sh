echo
read -p "Python a falso $1  con cada shim"
ping "$1" -c 1 | grep from
echo

python -c 'import urllib 
print urllib.urlopen("https://'$1'").read()'


read -p "............................................................................................"
LD_PRELOAD=../tls_shims/shim_v1.so python -c 'import urllib 
print urllib.urlopen("https://'$1'").read()'


read -p "............................................................................................"
LD_PRELOAD=../tls_shims/shim_v2.so python -c 'import urllib 
print urllib.urlopen("https://'$1'").read()'


read -p "............................................................................................"
LD_PRELOAD=../tls_shims/shim_v3.so python -c 'import urllib 
print urllib.urlopen("https://'$1'").read()'


