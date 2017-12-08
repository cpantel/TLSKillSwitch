gcc -Wall -o say_random.exe say_random.c
gcc -Wall -fpic -shared -o shim.so shim.c -ldl
read -p "azar!!!"
LD_PRELOAD=./shim.so  ./say_random.exe
