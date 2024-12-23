#include <LiquidCrystal.h>
#include <Wire.h>

#define ROLE_SND 0
#define ROLE_RC 1
#define ROLE ROLE_SND

int photoPin = A0;   // Pin for photoresistor

long long currTime;  // Variable for noting current time
int range;

// I2C address for this slave device
const byte SLAVE_ADDRESS = 0x04; // Assign a unique address (1-127)

void setup() {
  // Initialize Serial Monitor for debugging
  Serial.begin(9600);
  Serial.println("print");

  // Initialize I2C communication as a slave
  Wire.begin(SLAVE_ADDRESS);
  Wire.onRequest(requestEvent); // Register function to respond to requests from master

  // Optional: Initialize LCD if needed (commented out as not used in this code)
  // lcd.begin(16, 2);    // Set up the LCD's number of columns and rows
}

void loop() {
  // No code needed here for I2C communication
  delay(100); // Short delay to prevent excessive looping
}

void requestEvent() {
  int lightRaw = analogRead(photoPin);  // Read from photoresistor
  int light = map(lightRaw, 0, 1023, 0, 10); // Map reading from 0-1023 to 0-10

  // Serial debugging output
  // Serial.print("lightRaw: ");
  // Serial.print(lightRaw);
  // Serial.print(", light: ");
  // Serial.println(light);

  // Depending on light level, print to serial monitor
  if (light < 2) {
    Serial.println("dark");
  } else if (light <= 4) {
    Serial.println("partially dark");
  } else if (light <= 6) {
    Serial.println("medium");
  } else if (light <= 8) {
    Serial.println("fully lit");
  } else {
    Serial.println(light);
  }
  // Send the light value to the master
  Wire.write(light); // Send as a single byte (0-10)
  Serial.println("Data Sent");
} 