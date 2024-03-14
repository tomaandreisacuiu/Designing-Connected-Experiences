#include "WiFi.h"
#include "OOCSI.h"
#include "FS.h"
#include "SD.h"
#include "SPI.h"
// Audio libraries that were tried during development 
//#include "Audio.h"
//#include <AudioGeneratorMP3.h>
//#include <AudioFileSourceSD.h>
//#include <AudioOutputI2S.h>

// SENDER CODE

// LED pin
const int ledPin = 0;

// Network constants
const char* ssid = "Parapet_Cafe";
const char* password = "bananaMiau206";

OOCSI oocsi = OOCSI();

//Toggle switch variables and constants
const int toggleSwitchPin = 13;
int lastSwitchState = -1;   
int switchState = 1;

// "Send" button constants and variables
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

void setup() {
  Serial.begin(115200); 
  pinMode(14, INPUT_PULLUP);
  
  // BEGIN WiFi Connection
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  Serial.println("Connected to WiFi");
  // END - WiFI Connection setup done (if successful, otherwise keeps trying)

  // BEGIN OOCSI Connection
  oocsi.connect("senderESP32_toma", "oocsi.id.tue.nl", "ssid", "password", processOOCSI);
  Serial.println("Connected to OOCSI");
  // END - Connected to OOCSI successfully

  // Mounting of SD Card (tried during development)
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

  /*
  Sets up: two pins as pull-up input(button and toggle switch),
  and one as output (LED)
  */
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(toggleSwitchPin, INPUT_PULLUP);
  pinMode(ledPin, OUTPUT);

  // Toggle switch - for bi-directional communication
}

void loop() {
  oocsi.check();

  // Same as for the receiver - for future bi-directional communication
  // switchState = digitalRead(toggleSwitchPin);

  // If the switch state has changed, it sends it to the receiver
  // Also relevant for the a possible bi-directionality
  if (switchState != lastSwitchState) {
    oocsi.newMessage("receiverESP32_toma"); 
    oocsi.addInt("toggle_Switch_1", switchState);
    oocsi.sendMessage();
    
    lastSwitchState = switchState;

    Serial.print("Sent switch state: ");
    Serial.println(switchState);
  }

  // Sending the potentiometer value to the receiver
  int analogValue = analogRead(potPin); 
  int potInterval = map(analogValue, 0, 8191, 1, 3);

  // Gets the state of the button with digital reading
  bool reading = digitalRead(buttonPin);

  if (digitalRead(buttonPin) == LOW) {
    // Button is pressed
    Serial.println("Button pressed!");
    // Some debounce delay
    delay(50);    
    // Waiting for the button to be released
    while(digitalRead(buttonPin) == LOW) {
      // It just waits, does nothing else.
    }

    // Sends the new potentiomerer value (the new mood)
    oocsi.newMessage("receiverESP32_toma");
    oocsi.addInt("potentiometer_1", potInterval);
    oocsi.sendMessage();

    Serial.print("Sent potentiometer interval on button press: ");
    Serial.println(potInterval);
    Serial.println("Button released!");
  }

  delay(500);
}

/*
The handler method for incoming OOCSI messages
*/
void processOOCSI() {
  // Gets the value of the receiver's toggle switch
  int ivalue = oocsi.getInt("toggle_Switch_2", -1);
  Serial.print("received switch state is: ");
  Serial.println(ivalue);

  // Adjusting the LED for bi-directional communication
  // if (ivalue == 1) {
  //   digitalWrite (0, LOW);
  //   Serial.println("Led is LOW");
  // } else {
  //   digitalWrite (0, HIGH); 
  //   Serial.println("Led is HIGH");
  // }

  // For bi-directional communication - pot value of receiver (when it works like a sender)
  // int potentiometerValue = oocsi.getInt("potentiometer_2", -1);
  // lastSongFetched = potentiometerValue;
  // Serial.print("Potentiometer value received:");
  // Serial.println(potentiometerValue);
}
