SCRIPT="var http = require('https');

var options = {
    host: '$1',
    path: '/'
}
var request = http.request(options, function (res) {
    var data = '';
    res.on('data', function (chunk) {
        data += chunk;
    });
    res.on('end', function () {
        console.log(data);

    });
});
request.on('error', function (e) {
    console.log(e.message);
});
request.end();
"


echo
read -p "JavaScript a falso $1 con cada shim"
ping "$1" -c 1 | grep from
echo

node -e "$SCRIPT"

read -p "............................................................................................"
gcc -Wall -fpic -shared -o shim.so shim_ssl_get_verify_result.c -ldl -lssl
LD_PRELOAD=./shim.so node -e "$SCRIPT"

read -p "............................................................................................"
gcc -Wall -fpic -shared -o shim.so shim_ssl_ctx_set_verify.c -ldl -lssl
LD_PRELOAD=./shim.so node -e "$SCRIPT"

read -p "............................................................................................"
gcc -Wall -fpic -shared -o shim.so shim_ssl_ctx_set_verify_short.c -ldl -lssl
LD_PRELOAD=./shim.so node -e "$SCRIPT"
