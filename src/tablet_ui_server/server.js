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
//var client = mqtt.connect(config.mqtt_server || 'mqtt://127.0.0.1');
var fs = require('fs');




const rosnodejs = require('rosnodejs');
const std_msgs = rosnodejs.require('std_msgs').msg;







//{"display":{"curr_kmh":0,"curr_kn":0,"alert":0,"motor":1,"light":0, "alert_msg":"12m3"}}
var VERSION = 0.2;

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

var rosnode = null;
var pub = null;


// Register node with ROS master
rosnodejs.initNode('/tablet_node')
    .then((rosNode) => {
        rosnode = rosNode;

        pub = rosnode.advertise('/uimsg', std_msgs.String)
        // Create ROS subscriber on the 'chatter' topic expecting String messages
        let sub = rosNode.subscribe('state', std_msgs.String,
            (data) => { // define callback execution
                 rosnodejs.log.info('broadcast I heard: [' + data.data + ']');

                var json = data.data;
                try {
                    json = JSON.parse(data.data);
                } catch (error) {

                }

                var ctl = 0;
                if (json.ctlmode) {
                    ctl = 0;
                } else {
                    ctl = 1;
                }
                io.emit('broadcast', {
                    display: {
                        kn: json.kn,
                        kmh: json.kmh,
                        kompressordruck: json.kompressordruck,
                        breaklevel: json.breaklevel,
                        direction: json.direction,
                        state_v0: json.state_v0,
                        state_v1: json.state_v1,
                        state_v2: json.state_v2,
                        state_v3: json.state_v3,
                        ctlmode: ctl,
                        asc_state: json.asc_state,
                        fire_detcted: json.fire_detcted,
                        temperature: json.temperature,
                        batt_charge: json.batt_charge,
                        light_state: json.light_state,
                        asc_rest_dist: json.asc_rest_dist,
                        emergencybrake: json.emergencybrake,
                        kompressorstate: json.kompressorstate,
                        lightstate: json.lightstate,
                        kompressor_power_state:json.kompressor_power_state,
                        emergencybrakereset:json.emergencybrakereset,
                        hupe_state:json.hupe_state

                    }
                });
            }
        );
    });





var last_update = Math.round(new Date().getTime() / 1000);


app.get('/', function (req, res) {
    sess = req.session;
    res.render('index.ejs', {
    });
});


app.get('/break', function (req, res) {
    sess = req.session;
    res.render('emergencyctl.ejs', {
    });
});




io.on('connection', (socket) => {
    socket.on('disconnect', function () {
        console.log('user disconnected');
    });



    socket.on('event', function (_msg) {
        console.log(_msg.event + ' event');
        if (pub != null) {
            var msg = new std_msgs.String();
            msg.data = JSON.stringify(_msg);

            pub.publish(msg);
        }
    });


});
//TODO BUTTONS FÜR MANUELL SC_HALTEN EINBAUEN


function RESET_ALL() {
    last_update = Math.round(new Date().getTime() / 1000);
}
RESET_ALL(); //AUSGANGSZUSTAND




