#include <esp_now.h>
#include <WiFi.h>

// Receiver MAC (replace with your receiver’s MAC address)
uint8_t receiverMac[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

// Structure to send data
typedef struct struct_message {
  int x;
  int y;
} struct_message;

struct_message joystickData;

void setup() {
  Serial.begin(115200);

  WiFi.mode(WIFI_STA);

  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Add peer
  esp_now_peer_info_t peerInfo;
  memcpy(peerInfo.peer_addr, receiverMac, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }
}

void loop() {
  // Example joystick values
  joystickData.x = analogRead(34); // Replace with your joystick X pin
  joystickData.y = analogRead(35); // Replace with your joystick Y pin

  // Send
  esp_err_t result = esp_now_send(receiverMac, (uint8_t *) &joystickData, sizeof(joystickData));

  if (result == ESP_OK) {
    Serial.println("Sent successfully");
  } else {
    Serial.println("Error sending the data");
  }

  delay(200);
}
