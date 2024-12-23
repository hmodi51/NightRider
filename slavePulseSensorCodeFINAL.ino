#include <PulseSensorPlayground.h>
#include <Wire.h>

#define PULSE_SENSOR_PIN 0   // Analog pin where the PulseSensor is connected
#define LED_PIN 13           // On-board LED pin
#define THRESHOLD 550        // Threshold for detecting a heartbeat
#define SLAVE_ADDRESS 0x05   // I2C address for this slave device

PulseSensorPlayground pulseSensor;  // Create PulseSensorPlayground object

unsigned long previousMillis = 0;
const long interval = 20;  // Sampling interval in milliseconds

int latestBPM = 0;  // Variable to store the latest BPM value

void setup() {
  Serial.begin(9600);
  Wire.begin(SLAVE_ADDRESS);
  Wire.onRequest(requestEvent);

  pulseSensor.analogInput(PULSE_SENSOR_PIN);
  pulseSensor.blinkOnPulse(LED_PIN);  // Automatically blink the LED every beat.
  pulseSensor.setThreshold(THRESHOLD);

  if (!pulseSensor.begin()) {
    Serial.println("Pulse Sensor Failed to Initialize");
  } else {
    Serial.println("Pulse Sensor Initialized");
  }
}

void loop() {
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    if (pulseSensor.sawStartOfBeat()) {
      latestBPM = pulseSensor.getBeatsPerMinute();  // Get the Beats Per Minute.
      Serial.print("BPM: ");
      Serial.println(latestBPM);  // Print the BPM value to the Serial Monitor.
    }
  }
}

void requestEvent() {
  byte bpmToSend = (byte)constrain(latestBPM, 0, 255);  // Ensure value is within byte range.
  Wire.write(bpmToSend);  // Send BPM as a single byte to the master.
  Serial.println("Data Sent: BPM " + String(bpmToSend));
}