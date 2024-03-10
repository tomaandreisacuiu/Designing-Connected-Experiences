#include "WiFi.h"
#include "OOCSI.h"
//#include "Audio.h"
#include "FS.h"
#include "SD.h"
#include "SPI.h"
//#include <AudioGeneratorMP3.h>
//#include <AudioFileSourceSD.h>
//#include <AudioOutputI2S.h>


// Network variables
const char* ssid = "iotroam";
const char* password = "FoaQOCFMAe";

OOCSI oocsi = OOCSI();

//Toggle switch variables
const int toggleSwitchPin = 13;
int lastSwitchState = -1;   
int switchState = 1;

// Send button variables
const int buttonPin = 2; // GPIO pin for the button
bool lastButtonState = HIGH; // Initial state for input pullup
unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 50; // the debounce time in milliseconds

//Potentiometer variables
int potPin = 6;
int lastAnalogValue = -1;
int lastPotInterval = -1;

//Audio variables
// AudioGeneratorMP3 *mp3 = nullptr; // Initialize mp3 pointer to nullptr
// AudioFileSourceSD *file = nullptr; // Initialize file pointer to nullptr
// AudioOutputI2S *output = nullptr; // Initialize output pointer to nullptr

int lastSongFetched = -1;

// LED pin
const int ledPin = 0;

void setup() {
  Serial.begin(115200); 
  pinMode(14, INPUT_PULLUP);
  
  // Connecting to the OOCSI network

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  Serial.println("Connected to WiFi");

  oocsi.connect("senderESP32_toma", "oocsi.id.tue.nl", "ssid", "password", processOOCSI);

  Serial.println("Connected to OOCSI");

  // // SD card mounting
  // SPI.begin(12, 13, 11, 10);
  // if (!SD.begin(10, SPI)) {
  //   Serial.println("Card Mount Failed");
  //   return;
  // }
  // uint8_t cardType = SD.cardType();

  // if(cardType == CARD_NONE){
  //   Serial.println("No SD card attached");
  //   return;
  // }

  // Button set up
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(toggleSwitchPin, INPUT_PULLUP);
  pinMode(0, OUTPUT);
  // Toggle switch 

}

void loop() {
  oocsi.check();

  // Send availability for the receiver's LED
  switchState = digitalRead(toggleSwitchPin);

  if (switchState != lastSwitchState) {
   
    oocsi.newMessage("receiverESP32_toma"); 
    oocsi.addInt("toggle_Switch_1", switchState);
    oocsi.sendMessage();
    
    lastSwitchState = switchState;

    // Serial.print("Sent switch state: ");
    // Serial.println(switchState);
  }
  // Sending the potentiometer value
  int analogValue = analogRead(potPin); 
  int potInterval = map(analogValue, 0, 8191, 0, 4);

  bool reading = digitalRead(buttonPin);

  if (digitalRead(buttonPin) == HIGH) {
    // Button is pressed
    Serial.println("Button pressed!");
    // Debounce delay
    delay(50);

    //Sending the new mood
    oocsi.newMessage("receiverESP32_toma");
    oocsi.addInt("potentiometer_1", potInterval);
    oocsi.sendMessage();

    Serial.print("Sent potentiometer interval on button press: ");
    Serial.println(potInterval);
    // Wait for the button to be released
    while(digitalRead(buttonPin) == HIGH) {
      // Do nothing here until the button is released
    }
    Serial.println("Button released!");
  }

  delay(500);
}

void processOOCSI() {
  // Processing incoming data
  int ivalue = oocsi.getInt("toggle_Switch_2", -1);
  if (ivalue == 1) {
    digitalWrite (0, LOW);
    Serial.println("Led is LOW");
  } else {
    digitalWrite (0, HIGH); 
    Serial.println("Led is HIGH");
  }

  

  Serial.print("received switch state is: ");
  Serial.println(ivalue);

  // int potentiometerValue = oocsi.getInt("potentiometer_2", -1);
  // lastSongFetched = potentiometerValue;
  // Serial.print("Potentiometer value received:");
  // Serial.println(potentiometerValue);
}