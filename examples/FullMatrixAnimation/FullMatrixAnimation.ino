#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <AsyncWebSocket.h>
#include <ArduinoJson.h>
#include <Vira_Matrix.h>

ViraMatrix Vira_Matrix(14, 13, 12, 8, 8);

String scrollText = " @VIRAMEDAR  LED Display ";
int scrollPos = 0;
uint32_t lastScroll = 0;
volatile int animMode = 0;
volatile bool smileToggle = false;
volatile bool heartToggle = false;

uint8_t animFrame[8] = {0};



const char* ssid = "WiFi-SSID";
const char* password = "WiFi-PASS";

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");


const char html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width, initial-scale=1, maximum-scale=1, user-scalable=no">
<title>Vira LED Display</title>
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

.header{
    display:flex;
    align-items:center;
    justify-content:center;
    gap:10px;
    margin-bottom:6px;
}

.header img{
    width:64px;
    height:32px;
    object-fit:contain;
}

.header h2{
    margin:0;
    font-size:14px;
    opacity:0.9;
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
    <div class="header">
        <h2>Vira Matrix LED Display</h2>
    </div>
    <div class="toolbar">
        <div class="toolbar-row">
            <button onclick="clearAll()">🧹 Clear</button>
            <button onclick="emoji1()">😀 Smile</button>
            <button onclick="emoji2()">❤️ Heart</button>
            <button onclick="toggleErase()">✏️ Draw/Erase</button>
        </div>
        <div class="toolbar-row">
            <button onclick="sendAnim('smile')">😀</button>
            <button onclick="sendAnim('heart')">❤️</button>
            <button onclick="sendAnim('heart2')">❤️</button>
            <button onclick="sendAnim('heartmove')">💖</button>
            <button onclick="sendAnim('spinner')">🌀</button>
            <button onclick="sendAnim('eyelook')">👀</button>
            <button onclick="sendAnim('stop')">⛔Stop</button>
        </div>
        <div class="toolbar-row">
            <input id="txt" type="text" placeholder="Enter text..." value=" @VIRAMEDAR " style="padding:6px;width:200px;">
            <button onclick="sendText()">Send</button>
            <button onclick="sendAnim('scroll')">📝 Scroll</button>

        </div>

    </div>
</div>
<div id="canvasWrap">
    <canvas id="c"></canvas>
</div>

<div id="footer">
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
    const wrap = document.getElementById("canvasWrap");
    let size = Math.min(window.innerWidth * 0.9, 320);
    canvas.style.width = size + "px";
    canvas.style.height = size + "px";
    canvas.width = 320;
    canvas.height = 320;
    draw();
}
window.addEventListener("load", () => {
    resizeCanvas();
    draw();
});
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
    ws.onopen = ()=>{
        console.log("WS OK");
        ws.send("ping");
        draw();
    };
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
    const r = canvas.getBoundingClientRect();
    const cellSize = r.width / 8;
    ctx.clearRect(0,0,canvas.width,canvas.height);
    for(let y=0;y<8;y++){
        for(let x=0;x<8;x++){
            let bit = 7-x;
            let on = (frame[7-y] >> bit) & 1;
            ctx.fillStyle = on ? "#00ff00" : "#333";
            ctx.fillRect(
                x * cellSize,
                y * cellSize,
                cellSize - 2,
                cellSize - 2
            );
        }
    }
}
function getCell(e){
    let r = canvas.getBoundingClientRect();
    let x = Math.floor((e.clientX - r.left) / (r.width / 8));
    let y = Math.floor((e.clientY - r.top) / (r.height / 8));
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
function sendAnim(name){
    ws.send(name);
}
function sendText()
{
    let t = document.getElementById("txt").value;
    ws.send("text:" + t);
}
canvas.addEventListener("pointerdown",(e)=>{
    e.preventDefault();
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
function factoryReset(){
    if(confirm("Factory Reset? All WiFi settings will be deleted!")){
        ws.send("factory_reset");
    }
}
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

    if(msg == "smile") {
        animMode = 1;
        return;
    }

    if(msg == "heart") {
        animMode = 2;
        return;
    }

    if(msg == "stop") {
        animMode = 0;

        Vira_Matrix.setFrame(frame);
        return;
    }

    if(msg == "heart2") {
        animMode = 4;
        return;
    }


    if(msg == "wave"){
      animMode = 5;  
      return;
    } 

    if(msg == "spinner"){
      animMode = 6;  
      return;
    } 

    if(msg == "eyelook"){
      animMode = 7;  
      return;
    } 

    if(msg == "heartmove"){
      animMode = 8;  
      return;
    } 

    if(msg == "scroll") {
    animMode = 3;
    }

    if(msg.startsWith("text:"))
    {
        scrollText = msg.substring(5);
        scrollPos = 0;
        animMode = 3; 
        return;
    }

    StaticJsonDocument<256> doc;
    if(deserializeJson(doc, msg)) return;

    JsonArray arr = doc.as<JsonArray>();

    for(int i=0;i<8;i++){
        frame[i] = arr[i];
        animFrame[i] = arr[i];
    }

    animMode = 0;
    Vira_Matrix.setFrame(frame);
}




void renderScroll()
{
    uint8_t buffer[8] = {0};

    int col = scrollPos;

    for(int x=0; x<8; x++)
    {
        int charIndex = (col + x) / 6;
        int colIndex  = (col + x) % 6;

        char c = scrollText[charIndex];

        const char charset[] =
        " 0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz"
        ":;<=>?@#."
        "آابپتثجچحخدذرزسشصضطظعغفقکگلمنوهی";

        int fontIndex = 0;
        for(int i=0;i<sizeof(charset)-1;i++)
        {
            if(charset[i] == c)
            {
                fontIndex = i;
                break;
            }
        }

        uint8_t column = 0;

        if(colIndex < 5)
            column = font5x7[fontIndex][colIndex];

        for(int y=0;y<8;y++)
        {
            if(column & (1 << y))
                buffer[7 - y] |= (1 << (7 - x));
        }
    }

    Vira_Matrix.setFrame(buffer);

    scrollPos++;
    if(scrollPos > scrollText.length() * 6)
        scrollPos = 0;
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

    Serial.println("");
    Serial.println(WiFi.localIP());
    Serial.println("");
    String IPShow ="IP : " + WiFi.localIP().toString() + " ";
    scrollText = IPShow;
    scrollPos = 0;
    animMode = 3;  

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
    ws.cleanupClients();

    static uint32_t t = 0;

    if(animMode == 1)
    {
        if(millis() - t > 120)
        {
            runSmileAnimation();
            t = millis();
        }

        Vira_Matrix.setFrame(frame);
    }
    else if(animMode == 2)
    {
        if(millis() - t > 120)
        {
            runHeartBeat();
            t = millis();
        }

        Vira_Matrix.setFrame(frame);
    }
    else if(animMode == 3)
    {
        if(millis() - t > 80)
        {
            renderScroll();
            t = millis();
        }
    }


    else if(animMode == 4)
    {
        if(millis() - t > 120)
        {
            runHeart2Beat();
            t = millis();
        }

        Vira_Matrix.setFrame(frame);
    }    

    else if(animMode == 5)
    {
        static uint32_t t = 0;

        if(millis() - t > 120)
        {
            runWaveAnimation();
            t = millis();
        }

        Vira_Matrix.setFrame(frame);
    }

    else if(animMode == 6)
    {
        static uint32_t t = 0;

        if(millis() - t > 120)
        {
            runSpinnerAnimation();
            t = millis();
        }

        Vira_Matrix.setFrame(frame);
    }


    else if(animMode == 7)
    {
        static uint32_t t = 0;

        if(millis() - t > 120)
        {
            runEyeLookAnimation();
            t = millis();
        }

        Vira_Matrix.setFrame(frame);
    }

    else if(animMode == 8)
    {
        static uint32_t t = 0;

        if(millis() - t > 120)
        {
            runHeartMoveAnimation();
            t = millis();
        }

        Vira_Matrix.setFrame(frame);
    }

    else
    {
        Vira_Matrix.setFrame(frame);
        
    }
    Vira_Matrix.refresh();
}