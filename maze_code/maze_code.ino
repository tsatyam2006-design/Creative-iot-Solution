#define L_S 15   // Left IR sensor
#define C_S 2   // Center IR sensor
#define R_S 4   // Right IR sensor
#define in1 5   // Motor 1 forward
#define in2 18   // Motor 1 backward
#define in3 19   // Motor 2 forward
#define in4 21  // Motor 2 backward

void setup() {
  pinMode(L_S, INPUT);
  pinMode(C_S, INPUT);
  pinMode(R_S, INPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);
}

void loop() {
  int left = digitalRead(L_S);
  int center = digitalRead(C_S);
  int right = digitalRead(R_S);

  if (left == 1 && center == 1 && right == 1) {
    // Dead End: Turn Back
    turnAround();
  } else if (left == 1 && center == 0 && right == 1) {
    // Straight Path
    forward();
  } else if (left == 0 && center == 1 && right == 1) {
    // Left Turn
    turnLeft();
  } else if (left == 1 && center == 1 && right == 0) {
    // Right Turn
    turnRight();
  } else {
    // Default: Stop
    stopMotors();
  }
}

// Motor control functions
void forward() {
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);
}

void turnLeft() {
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);
}

void turnRight() {
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);
}

void turnAround() {
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);
  delay(500);  // Adjust for 180-degree turn
}

void stopMotors() {
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);
}