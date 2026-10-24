/*
 * Project: Professional BO Motor Control via ESP32 Webpage
 * Author: [Your Name]
 * 
 * Description:
 * ESP32 creates a Wi-Fi Access Point that hosts a webpage with a professional design.
 * The page has two large buttons ("Move Left" and "Move Right") to control the motor,
 * and they momentarily change color on successful data transfer.
 * 
 * For queries or assistance, contact Roboslog at:
 * Website: https://roboslog.in
 */

#include <WiFi.h>
#include <WebServer.h>

// Wi-Fi credentials
const char* ssid = "DOOR_LOOK_ROOM_NO_22";
const char* password = "prem chor hai";

WebServer server(80);

// Motor control pins
const int IN1 = 25; // Motor Direction 1
const int IN2 = 26; // Motor Direction 2

// Variables to manage command execution
bool lastCommandLeft = false;

void setup() {
  // Initialize Serial Monitor
  Serial.begin(115200);

  // Set motor pins as outputs
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  
  // Create ESP32 Access Point
  WiFi.softAP(ssid, password);
  Serial.println("Access Point started");
  Serial.print("IP Address: ");
  Serial.println(WiFi.softAPIP());
  
  // Define web page routes
  server.on("/", []() {
    server.send(200, "text/html", R"rawliteral(
      <!DOCTYPE html>
      <html>
      <head>
        <title>Motor Control</title>
        <style>
          body {
            font-family: Arial, sans-serif;
            text-align: center;
            background-color: #f3f4f7;
            color: #333;
            margin: 0;
            padding: 0;
          }
          h1 {
            margin-top: 20px;
          }
          .button {
            display: inline-block;
            background-color: #007bff;
            color: white;
            border: none;
            border-radius: 5px;
            padding: 20px 40px;
            font-size: 20px;
            margin: 20px;
            cursor: pointer;
            transition: background-color 0.3s ease, transform 0.1s ease;
          }
          .button:active {
            transform: scale(0.98);
          }
          .success {
            background-color: #28a745 !important; /* Green */
          }
        </style>
      </head>
      <body>
        <h1>Professional Motor Control</h1>
        <button class="button" id="leftButton" onclick="sendRequest('/left', 'leftButton')">CLOSE DOOR</button>
        <button class="button" id="rightButton" onclick="sendRequest('/right', 'rightButton')">OPEN DOOR</button>
        <script>
          async function sendRequest(endpoint, buttonId) {
            const button = document.getElementById(buttonId);
            button.classList.add('success'); // Turn button green
            await fetch(endpoint);           // Send request to ESP32
            setTimeout(() => button.classList.remove('success'), 500); // Reset to blue
          }
        </script>
      </body>
      </html>
    )rawliteral");
  });

  server.on("/left", []() {
    if (!lastCommandLeft) { // Only execute if the last command was not left
      lastCommandLeft = true;
      Serial.println("Left command executed");
      moveMotor(true);
      server.send(200, "text/plain", "LEFT_EXECUTED");
    } else {
      Serial.println("Left command ignored: Already executed");
      server.send(200, "text/plain", "LEFT_IGNORED");
    }
  });

  server.on("/right", []() {
    if (lastCommandLeft) { // Only execute if the last command was left
      lastCommandLeft = false;
      Serial.println("Right command executed");
      moveMotor(false);
      server.send(200, "text/plain", "RIGHT_EXECUTED");
    } else {
      Serial.println("Right command ignored: Already executed");
      server.send(200, "text/plain", "RIGHT_IGNORED");
    }
  });

  server.begin();
  Serial.println("Web server started");
}

void loop() {
  server.handleClient();
}

// Function to move the motor
void moveMotor(bool isLeft) {
  if (isLeft) {
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    Serial.println("Motor is rotating LEFT.");
  } else {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    Serial.println("Motor is rotating RIGHT.");
  }
  delay(10000); // Run motor for 10 seconds
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  Serial.println("Motor stopped.");
}