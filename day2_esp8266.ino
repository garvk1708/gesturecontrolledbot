#include <ESP8266WiFi.h>
// Motor driver pin definitions
#define IN1 D1  
#define IN2 D2
#define IN3 D3
#define IN4 D4
#define ENA D5  // PWM pin for Motor A
#define ENB D6  // PWM pin for Motor B

// Define PWM properties
const int dutyCycle1 = 500;   // Speed duty cycle for forward/backward
const int dutyCycle2 = 500;   // Speed duty cycle for turning

/****************************************************/
// WiFi credentials
//Enter according to your device

const char* ssid = "MYWORLD07 1443";
const char* password = "NAmi94@#";

/****************************************************/

// WiFi server on port 80
WiFiServer server(80);

void setup() {
  // Start Serial communication
  Serial.begin(115200);
  
  // Set motor driver pins as outputs
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(2, OUTPUT); //LED is connected to pin 2 for status indication

  // Configure PWM 
  analogWriteRange(1023); // Set PWM range (0-1023 for ESP8266)

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  // Wi-Fi connected, start the server
  Serial.println("Connected to WiFi");
  server.begin();
  Serial.println("Server started");

  // Display the IP address
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  // Check if a new client has connected
  WiFiClient client = server.available();
  if (client) {
    Serial.println("New client connected"); 

    // While the client is connected, process incoming requests
    while (client.connected()) {
      if (client.available()) {
        String request = client.readStringUntil('\r');
        Serial.print("Received command: ");
        Serial.println(request);

        // Process commands
        if (request.endsWith("one")) {
          Serial.println("Command: Move Forward");
          moveForward();
        } else if (request.endsWith("two")) {
          Serial.println("Command: Turn Right");
          turnRight();
        } else if (request.endsWith("three")) {
          Serial.println("Command: Turn Left");
          turnLeft();
        } else if (request.endsWith("four")) {
          Serial.println("Command: Move Backward");
          moveBackward();
        } else if (request.endsWith("zero")) {
          Serial.println("Command: Stop");
          stopMoving();
        }
      }
    }

    // Client disconnected
    client.stop();
    digitalWrite(2, LOW);  // Turn off the LED to indicate the client disconnected
    Serial.println("Client disconnected");
  }
}

// Function to move the robot forward
void moveForward() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  analogWrite(ENA, dutyCycle1); // Set speed for ENA 
  analogWrite(ENB, dutyCycle1); // Set speed for ENB 
  Serial.println("Moving Forward");
}

// Function to move the robot backward
void moveBackward() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  analogWrite(ENA, dutyCycle1); // Set speed for ENA 
  analogWrite(ENB, dutyCycle1); // Set speed for ENB 
  Serial.println("Moving Backward");
}

// Function to turn the robot left
void turnLeft() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  analogWrite(ENA, dutyCycle2); // Set lower speed for ENA 
  analogWrite(ENB, dutyCycle2); // Set lower speed for ENB 
  Serial.println("Turning Left");
}

// Function to turn the robot right
void turnRight() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  analogWrite(ENA, dutyCycle2); // Set lower speed for ENA 
  analogWrite(ENB, dutyCycle2); // Set lower speed for ENB 
  Serial.println("Turning Right");
}

// Function to stop the robot
void stopMoving() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
  Serial.println("Stopping");
}
