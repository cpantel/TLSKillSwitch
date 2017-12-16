echo
read -p "wget a falso $1 con cada shim"
ping "$1" -c 1 | grep from
echo

wget "https://$1" --ca-certificate /home/carlos/Desktop/TLSKillSwitch/certificates/rootCA.crt -O -

read -p "............................................................................................"
LD_PRELOAD=../tls_shims/shim_v1.so wget "https://$1" --ca-certificate /home/carlos/Desktop/TLSKillSwitch/certificates/rootCA.crt -O -

read -p "............................................................................................"
LD_PRELOAD=../tls_shims/shim_v2.so wget "https://$1" --ca-certificate /home/carlos/Desktop/TLSKillSwitch/certificates/rootCA.crt -O -

read -p "............................................................................................"
LD_PRELOAD=../tls_shims/shim_v3.so wget "https://$1" --ca-certificate /home/carlos/Desktop/TLSKillSwitch/certificates/rootCA.crt -O -
