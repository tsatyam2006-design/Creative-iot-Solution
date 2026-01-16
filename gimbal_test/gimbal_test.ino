/*
  ESP32 Gimbal Web Hotspot for 2 MG995 servos
  - Creates a WiFi Access Point
  - Serves a simple web page with two "knobs" (range sliders)
  - Updates servo angles on /set?s1=ANGLE&s2=ANGLE

  Wiring:
   - Servo1 signal -> GPIO_SERVO1 (default 18)
   - Servo2 signal -> GPIO_SERVO2 (default 19)
   - Servo V+ -> External 5-6V supply (capable of several amps)
   - Servo GND -> External supply GND
   - Connect external supply GND to ESP32 GND

  NOTE: Do NOT power MG995 from ESP32 USB or 3.3V.
*/

#include <WiFi.h>
#include <WebServer.h>
#include <ESP32Servo.h>

// Change these if needed
const char* apSSID = "GimbalAP";
const char* apPassword = "gimbal1234"; // min 8 chars

// Servo pins (change to the pins you want)
const int GPIO_SERVO1 = 18;
const int GPIO_SERVO2 = 19;

// Servos
Servo servo1;
Servo servo2;

// Web server on port 80
WebServer server(80);

// Basic HTML page (kept compact)
const char index_html[] PROGMEM = R"rawliteral(
<!doctype html>
<html>
<head>
  <meta charset="utf-8" />
  <meta name="viewport" content="width=device-width,initial-scale=1" />
  <title>ESP32 Gimbal — 2 Knobs</title>
  <style>
    body { font-family: -apple-system,BlinkMacSystemFont,"Segoe UI",Roboto,Arial; margin:0; padding:18px; background:#0f1720; color:#e6eef8; display:flex; flex-direction:column; align-items:center; }
    h1 { font-size:20px; margin:6px 0 18px; text-align:center }
    .controls { display:flex; gap:28px; flex-wrap:wrap; justify-content:center; }
    .knob { background:#111827; border-radius:12px; padding:14px; width:180px; box-shadow: 0 6px 18px rgba(0,0,0,0.6); text-align:center; }
    label { display:block; font-size:14px; margin-bottom:8px; color:#cbd5e1; }
    input[type=range] { width:140px; transform: rotate(-90deg); margin: 28px 0; }
    .val { font-weight:700; font-size:18px; margin-top:6px; color:#e2e8f0; }
    .footer { margin-top:16px; font-size:13px; color:#93c5fd; opacity:0.9; }
    button { margin-top:8px; padding:8px 12px; border-radius:8px; border:0; background:#2563eb; color:white; cursor:pointer; }
    .row { display:flex; align-items:center; justify-content:center; gap:10px; }
  </style>
</head>
<body>
  <h1>ESP32 Gimbal — Control 2 Servos</h1>
  <div class="controls">
    <div class="knob" id="k1">
      <label for="s1">Servo 1 (Pitch)</label>
      <div class="row">
        <input id="s1" type="range" min="0" max="180" value="90" step="1" />
      </div>
      <div class="val">Angle: <span id="a1">90</span>°</div>
      <button onclick="center(1)">Center</button>
    </div>

    <div class="knob" id="k2">
      <label for="s2">Servo 2 (Yaw)</label>
      <div class="row">
        <input id="s2" type="range" min="0" max="180" value="90" step="1" />
      </div>
      <div class="val">Angle: <span id="a2">90</span>°</div>
      <button onclick="center(2)">Center</button>
    </div>
  </div>

  <div class="footer">AP: <b id="ssid">...</b> — Open this page from any device connected to the hotspot.</div>

<script>
const ssid = "%AP_SSID%";
document.getElementById('ssid').textContent = ssid;

const s1 = document.getElementById('s1');
const s2 = document.getElementById('s2');
const a1 = document.getElementById('a1');
const a2 = document.getElementById('a2');

let sendTimer = null;
function sendAngles() {
  const v1 = s1.value;
  const v2 = s2.value;
  // Send GET request to /set?s1=..&s2=..
  fetch('/set?s1=' + encodeURIComponent(v1) + '&s2=' + encodeURIComponent(v2))
    .catch(err => console.log('net err', err));
}

function scheduleSend() {
  // debounce to avoid flooding the ESP with requests
  if (sendTimer) clearTimeout(sendTimer);
  sendTimer = setTimeout(sendAngles, 80);
}

s1.addEventListener('input', () => {
  a1.textContent = s1.value;
  scheduleSend();
});
s2.addEventListener('input', () => {
  a2.textContent = s2.value;
  scheduleSend();
});

function center(n) {
  if (n===1) { s1.value = 90; a1.textContent = 90; }
  else { s2.value = 90; a2.textContent = 90; }
  sendAngles();
}

// On load, push initial values to device
window.addEventListener('load', () => {
  sendAngles();
});
</script>
</body>
</html>
)rawliteral";

void handleRoot() {
  // Insert the AP SSID into page before sending
  String html = String(index_html);
  html.replace("%AP_SSID%", apSSID);
  server.send(200, "text/html", html);
}

void handleSet() {
  if (server.hasArg("s1")) {
    String v = server.arg("s1");
    int angle = v.toInt();
    angle = constrain(angle, 0, 180);
    servo1.write(angle);
  }
  if (server.hasArg("s2")) {
    String v = server.arg("s2");
    int angle = v.toInt();
    angle = constrain(angle, 0, 180);
    servo2.write(angle);
  }
  server.send(200, "text/plain", "OK");
}

void setup() {
  Serial.begin(115200);
  delay(100);

  // Attach servos
  servo1.attach(GPIO_SERVO1);
  servo2.attach(GPIO_SERVO2);
  // Set to center
  servo1.write(90);
  servo2.write(90);

  // Start Access Point
  WiFi.softAP(apSSID, apPassword);
  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP started. IP: ");
  Serial.println(IP);

  // Routes
  server.on("/", HTTP_GET, handleRoot);
  server.on("/set", HTTP_GET, handleSet);
  server.onNotFound([](){
    server.send(404, "text/plain", "Not found");
  });

  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
}
