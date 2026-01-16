#include <esp_now.h>
#include <WiFi.h>

// Motor driver pins
#define IN1  25
#define IN2  26
#define IN3  27
#define IN4  14

// Structure to receive data
typedef struct struct_message {
  int x;
  int y;
} struct_message;

struct_message joystickData;

// Motor movement functions
void moveForward() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void moveBackward() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

void turnLeft() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void turnRight() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

void stopMotors() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}

// ✅ Updated callback when data is received
void onDataRecv(const esp_now_recv_info *info, const uint8_t *incomingData, int len) {
  memcpy(&joystickData, incomingData, sizeof(joystickData));

  int x = joystickData.x;
  int y = joystickData.y;

  Serial.print("X: "); Serial.print(x);
  Serial.print(" | Y: "); Serial.println(y);

  // Thresholds
  int center = 2048; // joystick center
  int deadzone = 600;

  if (y > center + deadzone) {
    moveForward();
  } else if (y < center - deadzone) {
    moveBackward();
  } else if (x > center + deadzone) {
    turnRight();
  } else if (x < center - deadzone) {
    turnLeft();
  } else {
    stopMotors();
  }

  // Optional: print MAC of sender (for debugging with multiple transmitters)
  char macStr[18];
  snprintf(macStr, sizeof(macStr),
           "%02X:%02X:%02X:%02X:%02X:%02X",
           info->src_addr[0], info->src_addr[1], info->src_addr[2],
           info->src_addr[3], info->src_addr[4], info->src_addr[5]);
  Serial.print("From: "); Serial.println(macStr);
}

void setup() {
  Serial.begin(115200);

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  stopMotors(); // safety

  WiFi.mode(WIFI_STA);

  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  
  esp_now_register_recv_cb(onDataRecv); // ✅ now correct

  Serial.println("Receiver ready...");
}

void loop() {
  // Nothing here, motors controlled in callback
}
