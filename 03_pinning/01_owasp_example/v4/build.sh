cc    -g3 -ggdb -O0 -DDEBUG=1 -std=c99 -Wall -Wextra -Wconversion -Wformat -Wformat=2 \
-Wformat-security -Wno-deprecated-declarations -Wno-unused-function  \
-I /usr/include/openssl/ \
-I /usr/include \
pubkey-pin.c pubkey-helper.c  ../../../tls_shims/common.c \
-o pubkey-pin.exe \
-lssl -lcrypto

