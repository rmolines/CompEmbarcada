var path = require('path');
var mime = require('mime');
var express = require('express');
var app = express();

app.use(express.static('public'));
app.use('/static', express.static('public'))

app.get('/', function (req, res) {
  //res.writeHead(200, {'Content-Type': ''text/event-stream''});
   res.send('akaOlá, aula do Rafael.');
})

var server = app.listen(8081, function () {
   var host = server.address().address
   var port = server.address().port

   console.log("Example app listening at http://%s:%s", host, port)
})
