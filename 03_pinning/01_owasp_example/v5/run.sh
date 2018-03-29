echo "WORKS BEST WITH 127.0.0.2"

read -p "shim v1"
LD_PRELOAD=/home/carlos/Desktop/TLSKillSwitch/tls_shims/shim_v1.so ./pubkey-pin.exe seguro.com

read -p "shim v2"
LD_PRELOAD=/home/carlos/Desktop/TLSKillSwitch/tls_shims/shim_v2.so ./pubkey-pin.exe seguro.com

read -p "shim v3"
LD_PRELOAD=/home/carlos/Desktop/TLSKillSwitch/tls_shims/shim_v3.so ./pubkey-pin.exe seguro.com

read -p "shim v4"
LD_PRELOAD=/home/carlos/Desktop/TLSKillSwitch/tls_shims/shim_v4.so ./pubkey-pin.exe seguro.com

