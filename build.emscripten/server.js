var http = require('http');
var fs = require('fs');
var port = process.env.PORT || 8080;

http.createServer(function (request, response) {
    var fileHandler = function (error, file) {
        if (error) {
            throw error;
        }
        else {
            response.write(file);
            response.end();
        }
    }
    console.log("Serving " + request.url);
    if (request.url == "/") {
        response.writeHeader(200, { 'Content-Type': 'text/html' });
        fs.readFile('index.html', fileHandler);
    }
    else if (request.url.endsWith ('.js')) {
        response.writeHeader(200, { 'Content-Type': 'text/script' });
        fs.readFile('.' + request.url, fileHandler);
    }
    else if (request.url.endsWith ('.wasm')) {
        response.writeHeader(200, { 'Content-Type': 'application/wasm' });
        fs.readFile('.' + request.url, fileHandler);
    }
}).listen(port);
