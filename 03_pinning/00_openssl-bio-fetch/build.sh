cc    -g3 -ggdb -O0  -std=c99 -Wall -Wextra -Wconversion -Wformat -Wformat=2 \
-Wformat-security -Wno-deprecated-declarations -Wno-unused-function  \
-I /usr/include/openssl/ \
-I /usr/include \
openssl-bio-fetch.c ../../tls_shims/common.c \
-o openssl-bio-fetch.exe \
-lssl -lcrypto

