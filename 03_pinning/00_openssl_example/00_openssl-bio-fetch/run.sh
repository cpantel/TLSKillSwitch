HOSTS="/home/carlos/Desktop/TLSKillSwitch/mount/etc/hosts"

sed '5d' -i "$HOSTS" ; sed '5i\127.0.0.1       seguro.com' -i "$HOSTS"
read -p "========== Sitio legitimo seguro =========="
./openssl-bio-fetch.exe seguro.com:443


sed '5d' -i "$HOSTS" ; sed '5i\127.0.0.2       seguro.com' -i "$HOSTS"
read -p "========== Falso certificado, falsa CA =========="
./openssl-bio-fetch.exe seguro.com:443


read -p "========== Con shim v1 =========="
LD_PRELOAD=../../../tls_shims/shim_v1.so ./openssl-bio-fetch.exe seguro.com:443

read -p "========== Con shim v2 =========="
LD_PRELOAD=../../../tls_shims/shim_v2.so ./openssl-bio-fetch.exe seguro.com:443

read -p "========== Con shim v3 =========="
LD_PRELOAD=../../../tls_shims/shim_v3.so ./openssl-bio-fetch.exe seguro.com:443



sed '5d' -i "$HOSTS" ; sed '5i\127.0.0.3       seguro.com' -i "$HOSTS"
read -p "========== Certificado robado, falsa CA =========="
./openssl-bio-fetch.exe seguro.com:443

read -p "========== Con shim v1 =========="
LD_PRELOAD=../../../tls_shims/shim_v1.so ./openssl-bio-fetch.exe seguro.com:443

read -p "========== Con shim v2 =========="
LD_PRELOAD=../../../tls_shims/shim_v2.so ./openssl-bio-fetch.exe seguro.com:443

read -p "========== Con shim v3 =========="
LD_PRELOAD=../../../tls_shims/shim_v3.so ./openssl-bio-fetch.exe seguro.com:443


#sed '5d' -i "$HOSTS" ; sed '5i\127.0.0.4       seguro.com' -i "$HOSTS"
#read -p "certificado robado, root CA robada"
#./openssl-bio-fetch.exe seguro.com:443





