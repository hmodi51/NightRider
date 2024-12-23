/*

Alaa Musa (amusa5 - 653431722)
Anirudh Yallapragada (ayall2 66975376)
Harshit Modi (hmodi5 669774429)
Hanel Vujic (hvuji2 - 669611768)

Project NightRider

Our project idea is a form of a dashboard system for bicycles. 
Biking in the city is quite scary, accidents are very likely if there isn't any signaling from the cyclist. 
We implemented robust safety features with Arduino boards to help cyclists safely bike in the city along with providing them information 
about their navigation and health metrics. These features would be akin to displays found on a car dashboard, 
such as turn signals, a pulse sensor, a taillight, and a GPS tracking system for the bike, providing a fun, safer, 
and more unique experience compared to the usual cycling experience.
*/

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Adafruit_NeoPixel.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ezButton.h>

#define LEFT_LED_PIN1 11
#define LEFT_LED_PIN2 12
#define LEFT_LED_PIN3 13

#define RIGHT_LED_PIN1 5
#define RIGHT_LED_PIN2 3
#define RIGHT_LED_PIN3 2

#define OLED_ADDR 0x3c

#define VRX_PIN  A3 // Arduino pin connected to VRX pin
#define VRY_PIN  A2 // Arduino pin connected to VRY pin
#define SW_PIN   6

ezButton button(SW_PIN);

int yValue = 0; // To store value of the Y axis
int prevYValue = 0;  

// Initialize the LiquidCrystal library with the I2C address, rows, and columns
LiquidCrystal_I2C lcd(0x27, 16, 2); // Address 0x27 is common for many LCDs

const int numofLeds = 8;
const int dinPin = 4;

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(numofLeds, dinPin, NEO_GRB + NEO_KHZ800);
Adafruit_SSD1306 display(128 , 64);



// Define I2C addresses for the slaves
#define PHOTO_SLAVE_ADDRESS 0x04 // I2C address of the photoresistor slave
#define PULSE_SLAVE_ADDRESS 0x05 // I2C address of the pulse sensor slave
#define THIRD_SLAVE_ADDRESS 0x06 // I2C address for future third slave

unsigned long previousMillis = 0; 
const unsigned long interval = 500;

int red = 255; 
int green = 0; 
int blue = 0;

bool blinkLeft = false;
bool blinkRight = false;

unsigned long previousMillisLeft = 0;
unsigned long previousMillisRight = 0;
const unsigned long LoopTimer = 500;

bool leftLedState = false;
bool rightLedState = false;

void setup() {
if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) 
  {
    Serial.println("SSD1306 allocation failed");
    for (;;); 
  }
  display.clearDisplay();
  display.display();

  button.setDebounceTime(50);

  pinMode(LEFT_LED_PIN1, OUTPUT);
  pinMode(LEFT_LED_PIN2, OUTPUT);
  pinMode(LEFT_LED_PIN3, OUTPUT);

  pinMode(RIGHT_LED_PIN1, OUTPUT);
  pinMode(RIGHT_LED_PIN2, OUTPUT);
  pinMode(RIGHT_LED_PIN3, OUTPUT);

  lcd.init();
  lcd.backlight();
  Serial.begin(9600);            // Start debug serial monitor
  lcd.begin(16, 2);              // Set up the LCD's columns and rows
  lcd.print("Awaiting Data...");
  pixels.begin(); 
  pixels.setBrightness(0); 
  Wire.begin();                  // Initialize I2C as master
}

void loop() {
  button.loop();

  unsigned long currentMillis = millis();
  // **Request Data from Pulse Sensor Slave**
  Wire.requestFrom(PULSE_SLAVE_ADDRESS, 1); // Request 1 byte from pulse sensor slave
  if (Wire.available()) {
    int pulseData = Wire.read();
    Serial.println("Received pulse data: " + String(pulseData)); // Debug output
    lcd.clear();
    lcd.setCursor(0, 0);  // Set cursor to the first line
    lcd.print("Pulse: ");
    lcd.setCursor(0, 1);  // Set cursor to the second line
    lcd.print(pulseData); // Display received pulse data
  } else {
    Serial.println("No valid pulse data received.");
  }

  // **Request Data from Photoresistor Slave**
  Wire.requestFrom(PHOTO_SLAVE_ADDRESS, 1); // Request 1 byte from photoresistor slave
  if (Wire.available()) {
    int sensorValue = Wire.read();
    Serial.println("Received photo data: " + String(sensorValue)); // Debug output

    // Update LEDs based on the received photoresistor value
    int brightness = map(sensorValue, 0, 10, 50, 5);
    // brightness = map(sensorValue, 0, 255, 255, 50);
    pixels.setBrightness(brightness); // Map 0-10 to 0-255
    Serial.println(brightness);
    for (int i = 0; i < numofLeds; i++) {
      pixels.setPixelColor(i, pixels.Color(red, green, blue)); 
    }
    pixels.show(); 
  } else {
    Serial.println("No valid photo data received.");
  }

  // **Placeholder for Future Third Slave**
  
if (currentMillis - previousMillis >= interval) 
  {
    previousMillis = currentMillis; 

    String receivedData = ""; 

    Wire.requestFrom(THIRD_SLAVE_ADDRESS, 32);
    
    int bytesReceived = 0; 
    while (Wire.available())            
    {
      char c = Wire.read();
      Serial.print(c);       
      receivedData += c;     
      bytesReceived++;
    }
    Serial.println(); 
    Serial.print("Bytes received: ");
    Serial.println(bytesReceived);

    if (bytesReceived > 0) {
      display.clearDisplay(); 
      display.setTextColor(SSD1306_WHITE);
      display.setCursor(0, 0);

      if (receivedData == "INVALID") 
      {
        display.clearDisplay();
        display.setTextColor(SSD1306_WHITE);
        display.setCursor(0, 0);
        display.setTextSize(2);
        display.print("No GPS Data");
        display.display();
      }
      else
      {
        float speed = parseFloatValue(receivedData, "Speed:");
        int satellites = parseIntValue(receivedData, "SAT:");
        float altitude = parseFloatValue(receivedData, "ALT:");

        display.setTextSize(2);
        display.print("Speed: ");
        display.print(speed);
        display.println(" km/h");

        display.print("SAT: ");
        display.println(satellites);

        display.print("ALT: ");
        display.print(altitude);
        display.println(" m");

        display.display();
      }
    } else {
      Serial.println("No valid data received from third slave.");
    }
  }
  
  yValue = analogRead(VRY_PIN);

  Serial.print("y = ");
  Serial.println(yValue);

  const int upThreshold = 1010;
  const int downThreshold = 10;

  if (yValue > upThreshold && prevYValue <= upThreshold) {
    blinkLeft = true;
    blinkRight = false;
    leftLedState = false; 
    previousMillisLeft = millis();

    digitalWrite(RIGHT_LED_PIN1, LOW);
    digitalWrite(RIGHT_LED_PIN2, LOW);
    digitalWrite(RIGHT_LED_PIN3, LOW);
  }


  if (yValue < downThreshold && prevYValue >= downThreshold) {
    blinkRight = true;
    blinkLeft = false;
    rightLedState = false; 
    previousMillisRight = millis();

    digitalWrite(LEFT_LED_PIN1, LOW);
    digitalWrite(LEFT_LED_PIN2, LOW);
    digitalWrite(LEFT_LED_PIN3, LOW);
  }

  prevYValue = yValue;

  if (button.isPressed()) {
    blinkLeft = false;
    blinkRight = false;

    digitalWrite(LEFT_LED_PIN1, LOW);
    digitalWrite(LEFT_LED_PIN2, LOW);
    digitalWrite(LEFT_LED_PIN3, LOW);

    digitalWrite(RIGHT_LED_PIN1, LOW);
    digitalWrite(RIGHT_LED_PIN2, LOW);
    digitalWrite(RIGHT_LED_PIN3, LOW);
  }


  if (blinkLeft) {
    if (currentMillis - previousMillisLeft >= interval) {
      previousMillisLeft = currentMillis;
      leftLedState = !leftLedState;
      digitalWrite(LEFT_LED_PIN1, leftLedState);
      digitalWrite(LEFT_LED_PIN2, leftLedState);
      digitalWrite(LEFT_LED_PIN3, leftLedState);
    }
  }


  if (blinkRight) {
    if (currentMillis - previousMillisRight >= interval) {
      previousMillisRight = currentMillis;
      rightLedState = !rightLedState;
      digitalWrite(RIGHT_LED_PIN1, rightLedState);
      digitalWrite(RIGHT_LED_PIN2, rightLedState);
      digitalWrite(RIGHT_LED_PIN3, rightLedState);
    }
  }

 if (currentMillis - previousMillisLoop >= LoopTimer) {        
     previousMillisLoop = currentMillis;
 }
}

float parseFloatValue(String data, String key) 
{
  int startIndex = data.indexOf(key) + key.length();
  int endIndex = data.indexOf(' ', startIndex);
  return data.substring(startIndex, endIndex).toFloat();
}

int parseIntValue(String data, String key) 
{
  int startIndex = data.indexOf(key) + key.length();
  int endIndex = data.indexOf(' ', startIndex);
  return data.substring(startIndex, endIndex).toInt();
}
