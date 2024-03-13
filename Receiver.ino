#include "WiFi.h"
#include "OOCSI.h"
//#include "Audio.h"
#include "FS.h"
#include "SD.h"
#include "SPI.h"
//#include <AudioGeneratorMP3.h>
//#include <AudioFileSourceSD.h>
//#include <AudioOutputI2S.h>

// Button Pin

// Network variables
const char* ssid = "Parapet_Cafe";
const char* password = "bananaMiau206";

OOCSI oocsi = OOCSI();

//Toggle switch variables
const int toggleSwitchPin = 6;
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

// Check variable that the last sent song has been played
int lastSongFetched = -1;

// LED pin
const int ledPin = 0;

// Speaker Pin
const int dacPin = 17;


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

  oocsi.connect("receiverESP32_toma", "oocsi.id.tue.nl", ssid, password, processOOCSI);

  Serial.println("Connected to OOCSI");

  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(toggleSwitchPin, INPUT_PULLUP);
  pinMode(ledPin, OUTPUT);
  pinMode(17, OUTPUT);
  // Set LED to off at start.
  digitalWrite(ledPin, LOW);
  // SD card mounting
  /*SPI.begin(12, 13, 11, 10);
  if (!SD.begin(10, SPI)) {
    Serial.println("Card Mount Failed");
    return;
  }
  else
  {
    Serial.printf("Mount Successful");
  }
  uint8_t cardType = SD.cardType();

  if(cardType == CARD_NONE){
    Serial.println("No SD card attached");
    return;
  }*/

}

void loop() {
  oocsi.check();

  // Send availability for the receiver's LED
  switchState = digitalRead(toggleSwitchPin);
  //Serial.println(switchState);
  if (switchState != lastSwitchState) {
   
    oocsi.newMessage("senderESP32_toma"); 
    oocsi.addInt("toggle_Switch_2", switchState);
    oocsi.sendMessage();

    if(lastSwitchState == 1 && switchState == 0 && digitalRead(ledPin)== HIGH)
    {
      playSong(lastSongFetched);
      lastSongFetched = -1;
    }
    
    lastSwitchState = switchState;

    // Serial.print("Sent switch state: ");
    // Serial.println(switchState);
  }
  
  delay(100);
}

void processOOCSI() {
  // Processing incoming data
  int ivalue = oocsi.getInt("toggle_Switch_1", -1);

  // if (ivalue == 1) {
  //   digitalWrite (0, LOW);
  //   Serial.println("Led is LOW");
  // } else if(ivalue == 0){
  //   digitalWrite (0, HIGH); 
  //   Serial.println("Led is HIGH");
  // }

  Serial.print("received switch state is: ");
  Serial.println(ivalue);

  int potentiometerValue = oocsi.getInt("potentiometer_1", -1);
  lastSongFetched = potentiometerValue;
  Serial.print("Switch state: ");
  Serial.println(switchState);
  Serial.print("Potentiometer value received:");
  Serial.println(potentiometerValue);

  if (switchState && potentiometerValue != -1) {
    digitalWrite (ledPin, HIGH);
    Serial.println("Led is ON");
  } else if(switchState == 0)
  {
    playSong(lastSongFetched);
    lastSongFetched = -1;
  } else {
   digitalWrite (ledPin, LOW); 
   Serial.println("Led is OFF");
  }
}

void playSong(int song)
{
  // int buzzPin = 8; // Buzzer pin
  // int buzzDuration = 200; // Duration of each buzz in milliseconds
  // int pauseBetweenBuzzes = 200; // Pause between buzzes in milliseconds
  
  Serial.println("Buzzing");
  for (int j = 0; j < song; ++j) {
    // Each iteration of this loop is one "play" of the tone
    long startTime = millis();
    while (millis() - startTime < 2000) { // Play tone for approximately 2 seconds
      // Generate a 1kHz tone
      for (int i = 0; i < 100; ++i) {
        dacWrite(dacPin, i % 2 ? 0 : 255); // Square wave
        delayMicroseconds(500); // Approximately 1kHz
      }
    }
    // Optional: Pause between repeats
    delay(500); // Wait for 0.5 seconds before the next play
  }

  digitalWrite(ledPin, LOW);
}