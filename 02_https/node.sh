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
LD_PRELOAD=../tls_shims/shim_v1.so node -e "$SCRIPT"

read -p "............................................................................................"
LD_PRELOAD=../tls_shims/shim_v2.so node -e "$SCRIPT"

read -p "............................................................................................"
LD_PRELOAD=../tls_shims/shim_v3.so node -e "$SCRIPT"
