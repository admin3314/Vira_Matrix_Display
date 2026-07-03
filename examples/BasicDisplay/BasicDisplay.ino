#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <AsyncWebSocket.h>
#include <ArduinoJson.h>
#include <Vira_Matrix.h>


const char* ssid = "WiFi-SSID";
const char* password = "WiFi-PASS";


ViraMatrix Vira_Matrix(14, 13, 12, 8, 8);
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");


const char html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width, initial-scale=1, maximum-scale=1, user-scalable=no">
<title>Vira Matrix LED Display</title>
<style>
html, body{
    margin:0;
    padding:0;
    height:100%;
    overflow:hidden;
    background: radial-gradient(circle at top, #0f0f10, #050505);
    color:white;
    font-family: "Segoe UI", Arial;
}
*{
    box-sizing:border-box;
}
#ui{
    position:fixed;
    top:10px;
    left:50%;
    transform:translateX(-50%);
    width:94%;
    max-width:460px;
    background: rgba(20,20,20,0.55);
    border: 1px solid rgba(255,255,255,0.08);
    backdrop-filter: blur(14px);
    border-radius:14px;
    padding:8px;
    text-align:center;
    box-shadow: 0 10px 30px rgba(0,0,0,0.6);
}
#ui h2{
    margin:4px 0 6px 0;
    font-size:14px;
    opacity:0.85;
}
.toolbar-row{
    display:flex;
    justify-content:center;
    flex-wrap:wrap;
    gap:5px;
    padding:4px;
    border-radius:10px;
    background: rgba(255,255,255,0.03);
}
.toolbar{
    display:flex;
    flex-direction:column;
    gap:6px;
}
.toolbar div{
    display:flex;
    justify-content:center;
    flex-wrap:wrap;
    gap:4px;
}
button{
    padding:5px 8px;
    font-size:12px;
    background: rgba(255,255,255,0.06);
    border: 1px solid rgba(255,255,255,0.1);
    color:#fff;
    border-radius:8px;
    transition:0.15s;
}
button:hover{
    background: rgba(255,255,255,0.15);
    transform: scale(1.05);
}
input{
    padding:5px 8px;
    font-size:12px;

    width:140px;

    border-radius:8px;
    border:1px solid rgba(255,255,255,0.15);

    background: rgba(0,0,0,0.4);
    color:white;
}
#canvasWrap{
    position:fixed;
    top:50%;
    left:50%;
    transform:translate(-50%,-50%);
    width:320px;
    height:320px;
    margin-top:30px;
    background: rgba(15,15,15,0.6);
    border: 1px solid rgba(255,255,255,0.08);
    border-radius:18px;
    backdrop-filter: blur(12px);
    -webkit-backdrop-filter: blur(12px);
    box-shadow: 0 20px 60px rgba(0,0,0,0.8);
    display:flex;
    align-items:center;
    justify-content:center;
}
canvas{
    background:#000;
    border-radius:12px;
    touch-action: none; 
    user-select: none;
}
#footer{
    position:fixed;
    bottom:10px;
    left:50%;
    transform:translateX(-50%);
    width:92%;
    max-width:420px;
    text-align:center;
    font-size:12px;
    color:#aaa;
    background: rgba(15,15,15,0.6);
    border: 1px solid rgba(255,255,255,0.08);
    backdrop-filter: blur(12px);
    -webkit-backdrop-filter: blur(12px);
    border-radius:14px;
    padding:8px;
    box-shadow: 0 10px 30px rgba(0,0,0,0.6);
}
a{
    color:#66ccff;
    text-decoration:none;
}
a:hover{
    color:#ffffff;
}
</style>
</head>
<body>
<div id="ui">
    <h2>Vira Matrix LED Display</h2>
    <div class="toolbar">
        <div class="toolbar-row">
            <button onclick="clearAll()">🧹 Clear</button>
            <button onclick="emoji1()">😀 Smile</button>
            <button onclick="emoji2()">❤️ Heart</button>
            <button onclick="toggleErase()">✏️ Draw/Erase</button>
        </div>
    </div>
</div>
<div id="canvasWrap">
    <canvas id="c"></canvas>
</div>
<div id="footer" class="toolbar-row">
    <hr style="border:0;border-top:1px solid #444;margin:10px;">
    <div style="text-align:center; font-size:13px; color:#888;">
        Powered By <span style="color:#fff">@viramedar</span><br>
        🔗 
        <a href="#"
            onclick="openYT()"
            target="_blank"
            rel="noopener noreferrer"
            onclick="event.stopPropagation()"
            style="color:#ff5555;">
            YouTube
        </a> |
        <a href="#"
            onclick="openTG()"
            target="_blank"
            rel="noopener noreferrer"
            onclick="event.stopPropagation()"
            style="color:#55aaff;">
            Telegram
        </a> |
        <a href="#"
            onclick="openIG()"
            target="_blank"
            rel="noopener noreferrer"
            onclick="event.stopPropagation()"
            style="color:#ff55ff;">
            Instagram
        </a>
    </div>
    <br>
</div>
<script>
const canvas = document.getElementById("c");
const ctx = canvas.getContext("2d");
canvas.width = 320;
canvas.height = 320;
const size = 8;
const cell = canvas.width / size;
let frame = new Array(8).fill(0);
let ws = null;
let isDrawing = false;
let lastCell = -1;
let eraseMode = false;
let lastPoint = null;
function resizeCanvas(){
    let size = Math.min(window.innerWidth, 420); 
    canvas.style.width = size + "px";
    canvas.style.height = size + "px";
    canvas.width = 320;
    canvas.height = 320;
    draw();
}
window.addEventListener("resize", resizeCanvas);
function drawLine(x0, y0, x1, y1){
    let dx = Math.abs(x1 - x0);
    let dy = Math.abs(y1 - y0);
    let sx = (x0 < x1) ? 1 : -1;
    let sy = (y0 < y1) ? 1 : -1;
    let err = dx - dy;
    while(true){
        setPixel(x0, y0, !eraseMode);
        if(x0 === x1 && y0 === y1) break;
        let e2 = 2 * err;
        if(e2 > -dy){
            err -= dy;
            x0 += sx;
        }
        if(e2 < dx){
            err += dx;
            y0 += sy;
        }
    }
}
window.addEventListener("error", function(e){
    console.log("JS ERROR:", e.message);
});
function openYT(){
  window.open("https://youtube.com/@viramedar", "_blank");
}
function openTG(){
  window.open("https://t.me/viramedar", "_blank");
}
function openIG(){
  window.open("https://instagram.com/viramedar", "_blank");
}
function connectWS(){
    ws = new WebSocket("ws://" + location.host + "/ws");

    ws.onopen = ()=>console.log("WS OK");

    ws.onmessage = (e)=>{
        try{
            frame = JSON.parse(e.data);
            draw();
        }catch(err){}
    };
    ws.onclose = ()=>setTimeout(connectWS,1000);
}
connectWS();
function draw(){
    ctx.fillStyle="#111";
    ctx.fillRect(0,0,320,320);

    for(let y=0;y<size;y++){
        for(let x=0;x<size;x++){

            let bit = 7-x;
            let on = (frame[7-y] >> bit) & 1;

            ctx.fillStyle = on ? "#00ff00" : "#333";
            ctx.fillRect(x*cell, y*cell, cell-2, cell-2);
        }
    }
}
function getCell(e){
    let r = canvas.getBoundingClientRect();
    let scaleX = 320 / r.width;
    let scaleY = 320 / r.height;
    let x = Math.floor(((e.clientX - r.left) * scaleX) / 40);
    let y = Math.floor(((e.clientY - r.top) * scaleY) / 40);
    if(x<0||x>7||y<0||y>7) return null;
    return {x,y};
}
function setPixel(x,y,val){
    let bit = 7-x;

    if(val){
        frame[7-y] |= (1<<bit);
    }else{
        frame[7-y] &= ~(1<<bit);
    }
}
function paint(x,y){

    if(lastCell === y*8 + x) return;
    lastCell = y*8 + x;

    if(lastPoint){
        drawLine(lastPoint.x, lastPoint.y, x, y);
    }else{
        setPixel(x,y,!eraseMode);
    }

    lastPoint = {x,y};

    draw();
    send();
}
function send(){
    if(ws && ws.readyState === 1){
        ws.send(JSON.stringify(frame));
    }
}
canvas.addEventListener("pointerdown",(e)=>{
    isDrawing = true;
    canvas.setPointerCapture(e.pointerId);

    let c = getCell(e);
    if(c) paint(c.x,c.y);
});

canvas.addEventListener("pointermove",(e)=>{
    if(!isDrawing) return;

    let c = getCell(e);
    if(c) paint(c.x,c.y);
});

canvas.addEventListener("pointerup",()=>{
    isDrawing = false;
    lastCell = -1;
    lastPoint = null;
});

canvas.addEventListener("pointercancel",()=>{
    isDrawing = false;
    lastCell = -1;
});
function clearAll(){
    frame = new Array(8).fill(0);
    draw();
    send();
}

function toggleErase(){
    eraseMode = !eraseMode;
}

function emoji1(){
    frame = [
        0b00111100,
        0b01000010,
        0b10011001,
        0b10100101,
        0b10000001,
        0b10100101,
        0b01000010,
        0b00111100
    ];

    draw();
    send();
}

function emoji2(){
    frame = [
        0b00000000,
        0b00011000,
        0b00111100,
        0b01111110,
        0b11111111,
        0b11100111,
        0b01100110,
        0b00000000
    ];

    draw();
    send();
}
window.onload = function(){
    resizeCanvas();
    draw();
};
</script>
</body>
</html>
)rawliteral";


void onWsEvent(
AsyncWebSocket *server,
AsyncWebSocketClient *client,
AwsEventType type,
void *arg,
uint8_t *data,
size_t len
)
{
if(type != WS_EVT_DATA) return;
AwsFrameInfo *info = (AwsFrameInfo*)arg;
if(!(info->final && info->index == 0 && info->len == len)) return;
String msg;
for(size_t i=0;i<len;i++) msg += (char)data[i];
StaticJsonDocument<256> doc;
if(deserializeJson(doc, msg)) return;
JsonArray arr = doc.as<JsonArray>();
for(int i=0;i<8;i++){
    frame[i] = arr[i];
}
Vira_Matrix.setFrame(frame);
}


void setup()
{
Serial.begin(115200);
Vira_Matrix.begin();
WiFi.begin(ssid,password);
while(WiFi.status()!=WL_CONNECTED){
    delay(300);
    Serial.print(".");
}
Serial.println("");
Serial.println(WiFi.localIP());  
ws.onEvent(onWsEvent);
server.addHandler(&ws);
server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", html);
});
server.on("/favicon.ico", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(204);
});
server.begin();
}


void loop()
{
Vira_Matrix.refresh();
}