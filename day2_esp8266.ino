#include <ESP8266WiFi.h>

#define IN1 D1  
#define IN2 D2
#define IN3 D3
#define IN4 D4
#define ENA D5
#define ENB D6

const int maxDutyCycle = 1001;  
const int rampUpStep = 10;       
const int rampUpDelay = 20;     

const char* ssid = "MYWORLD07 1443";
const char* password = "NAmi94@#";

WiFiServer server(80);

void setup() {
  Serial.begin(115200);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(2, OUTPUT);

  analogWriteRange(1023);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  Serial.println("Connected to WiFi");
  server.begin();
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  WiFiClient client = server.available();
  if (client) {
    Serial.println("New client connected"); 
    while (client.connected()) {
      if (client.available()) {
        String request = client.readStringUntil('\r');
        Serial.print("Received command: ");
        Serial.println(request);

        if (request.endsWith("one")) {
          moveForward();
        } else if (request.endsWith("two")) {
          turnRight();
        } else if (request.endsWith("three")) {
          turnLeft();
        } else if (request.endsWith("four")) {
          moveBackward();
        } else if (request.endsWith("zero")) {
          stopMoving();
        }
      }
    }

    client.stop();
    digitalWrite(2, LOW);
    Serial.println("Client disconnected");
  }
}

void rampUpPWM(int pin, int targetDutyCycle) {
  for (int dutyCycle = 0; dutyCycle <= targetDutyCycle; dutyCycle += rampUpStep) {
    analogWrite(pin, dutyCycle);
    delay(rampUpDelay);
  }
}

void moveForward() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  rampUpPWM(ENA, maxDutyCycle);
  rampUpPWM(ENB, maxDutyCycle);
  Serial.println("Moving Forward");
}

void moveBackward() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  rampUpPWM(ENA, maxDutyCycle);
  rampUpPWM(ENB, maxDutyCycle);
  Serial.println("Moving Backward");
}

void turnLeft() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  rampUpPWM(ENA, maxDutyCycle / 2);
  rampUpPWM(ENB, maxDutyCycle / 2);
  Serial.println("Turning Left");
}

void turnRight() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  rampUpPWM(ENA, maxDutyCycle / 2);
  rampUpPWM(ENB, maxDutyCycle / 2);
  Serial.println("Turning Right");
}

void stopMoving() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
  Serial.println("Stopping");
}
