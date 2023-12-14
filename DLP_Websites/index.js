// index.js

const express = require('express');
const app = express();
const port = 3000;
const mqtt = require('mqtt');

app.use(express.static("public"));
app.set("view engine", "ejs");
app.set("views", "./views");

var server = require("http").Server(app);
var io = require("socket.io")(server);

app.get('/', function(req, res)  {
    res.render('home.ejs');
});

app.get('/history', function(req, res){
    res.render('history.ejs');
})

server.listen(port, () => console.log(`App listening at http://localhost:${port}`));

//------------------------MQTT---------------------------
var options = {
    host: '192.168.15.65',
    port: 1883,
    protocol: 'mqtt',
    username: 'your_username',
    password: 'your_password',
    clientId: 'serverjs2'
};

const client = mqtt.connect(options);
let lastData = null; // Lưu trữ dữ liệu gần đây

client.on('connect', function () {
    console.log('Connected to MQTT broker');
    client.subscribe('send_info'); // Đăng ký để nhận các tin nhắn từ topic tương ứng
});

client.on('message', function (topic, message) {
    // Xử lý tin nhắn nhận được từ MQTT broker
    console.log('Received message:', message.toString());
    const data = JSON.parse(message); // Giả sử dữ liệu nhận được từ broker là một JSON

    if (lastData) {
        io.emit('agentData', lastData); // Gửi dữ liệu cũ về client thông qua socket.io
    }

    lastData = data; // Lưu dữ liệu mới nhất
});


