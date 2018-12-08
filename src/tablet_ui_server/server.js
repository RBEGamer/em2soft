var express = require('express');
var app = express();
var path = require('path');
var server = require('http').createServer(app);
var io = require('socket.io')(server);
var express = require('express');
var session = require('express-session');
var bodyParser = require('body-parser');
var sessionstore = require('sessionstore');
var os = require("os");
var chalk = require('chalk');
var mqtt = require('mqtt');
var config = require('./config.json');
var uuidv1 = require('uuid/v1');
var got = require('got');
var mqtt = require('mqtt');
var client = mqtt.connect(config.mqtt_server || 'mqtt://127.0.0.1');
var fs = require('fs');


//{"display":{"curr_kmh":0,"curr_kn":0,"alert":0,"motor":1,"light":0, "alert_msg":"12m3"}}
var VERSION =0.2;

var port = process.env.PORT || config.webserver_default_port || 3000;

//----------------------------- EXPRESS APP SETUP ------------------------------------------//
app.set('trust proxy', 1);
app.use(function (req, res, next) {
    if (!req.session) {
        return next(); //handle error
    }
    next(); //otherwise continue
});
app.set('views', __dirname + '/views');
app.engine('html', require('ejs').renderFile);
// Routing
app.use(express.static(path.join(__dirname, 'public')));
app.use(session({
    secret: 'ssshhfddghjhgewretjrdhfsgdfadvsgvshthhh',
    store: sessionstore.createSessionStore(),
    resave: true,
    saveUninitialized: true
}));
app.use(bodyParser.json());
app.use(bodyParser.urlencoded({
    extended: true
}));

app.use(function (req, res, next) {
    res.header("Access-Control-Allow-Origin", "*");
    res.header("Access-Control-Allow-Headers", "Origin, X-Requested-With, Content-Type, Accept");
    next();
});

server.listen(port, function () {
    console.log('Server listening at port %d', port);
});




setInterval(() => {
    client.publish("heartbeat_webui", String(Math.round(new Date().getTime() / 1000)));
}, 1000);

client.on('connect', function () {
    client.subscribe('status', function (err) {
        if (!err) {
           
        }
    });
});

client.on('message', function (topic, message) {
    // message is Buffer
    console.log(message.toString());
  
    var json = message;
    try
{
         json = JSON.parse(message);
}
catch(e)
{
      
}

    io.emit('broadcast', json);
   // io.emit('broadcast', {
     //   display: {
       //     alert: 0,
         //   motor: 1,
           // light: 1,

       //     curr_kn: 0,
         //   curr_kmh: 0

     //   }
   // });


})




var last_update = Math.round(new Date().getTime() / 1000);


app.get('/', function (req, res) {
    sess = req.session;
    res.render('index.ejs', {
    });
});





io.on('connection', (socket) => {
    socket.on('disconnect', function () {
        console.log('user disconnected');
    });
});
//TODO BUTTONS FÜR MANUELL SC_HALTEN EINBAUEN


function RESET_ALL(){
    last_update = Math.round(new Date().getTime() / 1000);
}
RESET_ALL(); //AUSGANGSZUSTAND



