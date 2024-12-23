#include <Wire.h>
#include <TinyGPSPlus.h>
#include <SoftwareSerial.h>
#include <SD.h>
#define SLAVE_ADDRESS 0x06

static const int RXPin = 4, TXPin = 3;
static const uint32_t GPSBaud = 9600;

// The TinyGPSPlus object
TinyGPSPlus gps;

// The serial connection to the GPS device
SoftwareSerial ss(RXPin, TXPin);

char i2cDataBuffer[100]; 
bool dataReady = false;  

const int chipSelect = 10;
File myFile;

void setup() 
{
  Wire.begin(SLAVE_ADDRESS);
  Wire.onRequest(requestEvent);

  Serial.begin(115200);
  ss.begin(GPSBaud);

  if (!SD.begin(chipSelect)) {
    Serial.println("initialization failed. Things to check:");
    Serial.println("1. is a card inserted?");
    Serial.println("2. is your wiring correct?");
    Serial.println("3. did you change the chipSelect pin to match your shield or module?");
    Serial.println("Note: press reset button on the board and reopen this Serial Monitor after fixing your issue!");
    while (true);
  }

  Serial.println("Setup complete.");
}

void loop() 
{
  while (ss.available() > 0)
  {
    char c = ss.read();
    gps.encode(c);
  }

  if (gps.location.isUpdated())
  {
    prepareData();
    displayInfo();
  }
}

void requestEvent()              
{   
  if (dataReady)
  {
    Wire.write(i2cDataBuffer, strlen(i2cDataBuffer));
    dataReady = false; 
  }
  else
  {
    Wire.write("No Data");
  }
}

void prepareData()
{
  Serial.println("inside PrepareData");
  if (gps.location.isValid())
  {
    char speedBuffer[15];
    dtostrf(gps.speed.kmph(), 8, 6, speedBuffer);

    char satelliteBuffer[15];
    sprintf(satelliteBuffer, "%d", gps.satellites.value());

    char altitudeBuffer[15];
    dtostrf(gps.altitude.meters(), 8, 2, altitudeBuffer);

    snprintf(i2cDataBuffer, sizeof(i2cDataBuffer), "Speed:%s km/h SAT:%s ALT:%s", speedBuffer, satelliteBuffer, altitudeBuffer);

    dataReady = true; 
  }
  else
  {
    strcpy(i2cDataBuffer, "INVALID");
    dataReady = true;
  }
}


void displayInfo()
{
  Serial.println("inside DisplayInfo");
  myFile = SD.open("test.txt", FILE_WRITE);
  
  if (myFile) {
    Serial.print(F("Location: ")); 
    myFile.println(F("Location: ")); 
    if (gps.location.isValid())
    {
      Serial.print(gps.location.lat(), 6);
      myFile.println(gps.location.lat(), 6);

      Serial.print(F(","));
      myFile.println(F(","));

      Serial.print(gps.location.lng(), 6);
      myFile.println(gps.location.lng(), 6);
    }
    else
    {
      Serial.print(F("INVALID"));
      myFile.println(F("INVALID"));
    }

    Serial.print(F("  Date/Time: "));
    myFile.println(F("  Date/Time: "));
    if (gps.date.isValid())
    {
      Serial.print(gps.date.month());
      myFile.println(gps.date.month());

      Serial.print(F("/"));
      myFile.println(F("/"));

      Serial.print(gps.date.day());
      myFile.println(gps.date.day());

      Serial.print(F("/"));
      myFile.println(F("/"));

      Serial.print(gps.date.year());
      myFile.println(gps.date.year());
    }
    else
    {
      Serial.print(F("INVALID"));
      myFile.println(F("INVALID"));
    }

    Serial.print(F(" "));
    myFile.println(F(" "));
    if (gps.time.isValid())
    {
      if (gps.time.hour() < 10) Serial.print(F("0"));
      Serial.print(gps.time.hour());
      myFile.println(gps.time.hour());

      Serial.print(F(":"));
      myFile.println(F(":"));

      if (gps.time.minute() < 10) Serial.print(F("0"));
      Serial.print(gps.time.minute());
      myFile.println(gps.time.minute());

      Serial.print(F(":"));
      myFile.println(F(":"));

      if (gps.time.second() < 10) Serial.print(F("0"));
      Serial.print(gps.time.second());
      myFile.println(gps.time.second());

      Serial.print(F("."));
      myFile.println(F("."));
      if (gps.time.centisecond() < 10) Serial.print(F("0"));
      Serial.print(gps.time.centisecond());
      myFile.println(gps.time.centisecond());
    }
    else
    {
      Serial.print(F("INVALID"));
      myFile.println(F("INVALID"));
    }
    // close the file:

  } else {
    
    // if the file didn't open, print an error:
    Serial.println("error opening test.txt");
  }


  myFile.close();
  Serial.println();
}
