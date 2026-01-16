#include <WiFi.h>
#include <WebServer.h>

// ---------- WiFi Access Point ----------
const char* ssid = "ESP32_Control";   // AP SSID
const char* password = "12345678";    // AP Password

// ---------- Pin Config ----------
#define RELAY1  26   // Bulb 1 (manual + PIR)
#define RELAY2  27   // Bulb 2 (manual only)
#define PIR_PIN 25   // PIR sensor input

// ---------- Variables ----------
bool bulb1State = false;   // Bulb 1 state (manual)
bool bulb2State = false;   // Bulb 2 state

WebServer server(80);

// ---------- Web Page ----------
String webpage() {
  String html = "<!DOCTYPE html><html><head>";
  html += "<meta name='viewport' content='width=device-width, initial-scale=1.0'>";
  html += "<style>body{font-family:Arial;text-align:center;margin-top:50px;}";
  html += ".btn{padding:15px 30px;margin:10px;font-size:20px;border:none;border-radius:10px;cursor:pointer;}";
  html += ".on{background:green;color:white;}";
  html += ".off{background:red;color:white;}";
  html += "</style></head><body>";

  html += "<h2>ESP32 Bulb Control</h2>";

  // Bulb 1
  html += "<p>Bulb 1 (Manual + PIR)</p>";
  if (bulb1State) html += "<a href='/bulb1off'><button class='btn off'>Turn OFF</button></a>";
  else html += "<a href='/bulb1on'><button class='btn on'>Turn ON</button></a>";

  // Bulb 2
  html += "<p>Bulb 2 (Manual Only)</p>";
  if (bulb2State) html += "<a href='/bulb2off'><button class='btn off'>Turn OFF</button></a>";
  else html += "<a href='/bulb2on'><button class='btn on'>Turn ON</button></a>";

  html += "</body></html>";
  return html;
}

// ---------- Web Handlers ----------
void handleRoot() { server.send(200, "text/html", webpage()); }

void handleBulb1On() {
  bulb1State = true;
  digitalWrite(RELAY1, LOW);  // Relay ON
  handleRoot();
}
void handleBulb1Off() {
  bulb1State = false;
  digitalWrite(RELAY1, HIGH); // Relay OFF
  handleRoot();
}
void handleBulb2On() {
  bulb2State = true;
  digitalWrite(RELAY2, LOW);
  handleRoot();
}
void handleBulb2Off() {
  bulb2State = false;
  digitalWrite(RELAY2, HIGH);
  handleRoot();
}

// ---------- Setup ----------
void setup() {
  Serial.begin(115200);

  pinMode(RELAY1, OUTPUT);
  pinMode(RELAY2, OUTPUT);
  pinMode(PIR_PIN, INPUT);

  digitalWrite(RELAY1, HIGH); // OFF initially
  digitalWrite(RELAY2, HIGH);

  // Start Access Point
  WiFi.softAP(ssid, password);
  Serial.print("AP IP: ");
  Serial.println(WiFi.softAPIP());

  // Web routes
  server.on("/", handleRoot);
  server.on("/bulb1on", handleBulb1On);
  server.on("/bulb1off", handleBulb1Off);
  server.on("/bulb2on", handleBulb2On);
  server.on("/bulb2off", handleBulb2Off);
  server.begin();
}

// ---------- Loop ----------
void loop() {
  server.handleClient();

  // PIR control for Bulb1
  if (digitalRead(PIR_PIN) == HIGH) {
    digitalWrite(RELAY1, LOW);   // ON
  } else if (!bulb1State) {      // Only turn OFF if user didn't force ON
    digitalWrite(RELAY1, HIGH);  // OFF
  }
}
