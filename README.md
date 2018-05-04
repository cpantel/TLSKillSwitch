# TLSKillSwitch

Código y texto de lo presentado en:

[OWASP LATAM Tour 2018 BsAs](https://www.owasp.org/index.php/LatamTour2018#tab=ARGENTINA_-_Buenos_Aires)

[Viernes Ñoños en CADIEEL](https://groups.google.com/forum/?hl=en#!topic/embebidos32/88bOqO63eWw)

## Instrucciones

Mejor implementar en una virtual.

Ajustar lo que hay en la carpeta mount para que funcione el servidor apache. Toqueteando /etc/hosts se apunta a los diversos virtual hosts.

Regenerar lo que haya en la carpeta de certificados que está todo vencido.

De la carpeta 03_pinning, sólo 01_owasp_example tiene algo útil.

## Ejercicios propuestos

Reemplazar srandom() en lugar de random() y meditar acerca de la utilidad de este cambio. Pista: unit testing.

Implementar un keylogger antes de mirar la implementación propuesta.

Jugar un rato con lo de TLS con el servidor configurado, cambiando a que virtual hosts apuntan los nombres.

Defenderse de 03_pinning/01_owasp_example/v4 sin mirar 03_pinning/01_owasp_example/v5.

Atacar 03_pinning/01_owasp_example/v5.

## The Googler Way: 

http://www.linuxjournal.com/article/7795

http://thomasfinch.me/blog/2015/07/24/Hooking-C-Functions-At-Runtime.html

https://0x00sec.org/t/linux-infecting-running-processes/1097

http://fluxius.handgrep.se/2011/10/31/the-magic-of-ld_preload-for-userland-rootkits/

https://datacenteroverlords.com/2012/03/01/creating-your-own-ssl-certificate-authority/

https://blog.fpmurphy.com/2012/09/all-about-ld_preload.html


## The Developer Way: RTFM

https://wiki.openssl.org/index.php/Manual:Ssl(3)

https://wiki.openssl.org/index.php/SSL/TLS_Client

http://fm4dd.com/openssl/sslconnect.htm

https://www.owasp.org/index.php/Certificate_and_Public_Key_Pinning#OpenSSL

## The Reverse Engineering Way: 

Comandos para hacer análisis estático:

```bash
which ...
ldd ...
dpkg -L ...
dpkg -S ...
nm -D /lib/x86_64-linux-gnu-libssl.so.1.0.0
objdump -T *.so
```

Ejemplos:

```bash
apt-cache depends wget
ldd /usr/bin/wget

apt-cache depends python python2.7 python2.7-stdlib

apt-cache depends php php7.0 php7.0-common

apt-cache depends R
which R
dpkg -S /usr/bin/R
apt-cache depends r-base-core
apt-cache depends r-base-core libcurl3

apt-cache depends nodejs
dpkg -L nodejs | less
ldd /usr/bin/node
locate node | grep -i ssl -> /usr/include/node/openssl
ls -l /usr/bin/node -h
```

Comando para hacer análisis dinámico:
```bash
ltrace --library='lib*' -o trace.txt wget https://...
```

## No scriptkiddie Way





