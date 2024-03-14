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

// LED pin
const int ledPin = 0;

// Speaker Pin
const int dacPin = 17;

// Network constants
const char* ssid = "Parapet_Cafe";
const char* password = "bananaMiau206";

OOCSI oocsi = OOCSI();

//Toggle switch variables and constants
const int toggleSwitchPin = 6; // toggle switch pin
int lastSwitchState = -1; 
int switchState = 1;

/*"Send" button constants and variables (for bi-directional communication, 
not implemented in hardware)
*/
const int buttonPin = 2; 
bool lastButtonState = HIGH; 
unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 50; 

//Potentiometer variables and constants
const int potPin = 6;
int lastAnalogValue = -1;
int lastPotInterval = -1;

// variable that is used to check if the last sent song has been played
int lastSongFetched = -1;

// Audio variables
// AudioGeneratorMP3 *mp3 = nullptr; // Initialize mp3 pointer to nullptr
// AudioFileSourceSD *file = nullptr; // Initialize file pointer to nullptr
// AudioOutputI2S *output = nullptr; // Initialize output pointer to nullptr

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
  oocsi.connect("receiverESP32_toma", "oocsi.id.tue.nl", ssid, password, processOOCSI);
  Serial.println("Connected to OOCSI");
  // END - Connected to OOCSI successfully

  /*
  Sets up: two pins as pull-up input(button and toggle switch),
  and two pins as output (LED and DAC)
  */
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(toggleSwitchPin, INPUT_PULLUP);
  pinMode(ledPin, OUTPUT);
  pinMode(dacPin, OUTPUT);

  // LED to be turned off initially
  digitalWrite(ledPin, LOW);

  // Mounting of SD Card (tried during development)
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

  // The state of the toggle switch is updated here
  switchState = digitalRead(toggleSwitchPin);
  
  // If the switch state has changed, the program handles it as follows:
  if (switchState != lastSwitchState) {
    /*
    Sends a message to the sender with this new state which describes
    if the receiver wants to play the songs transmitted.
    */ 
    oocsi.newMessage("senderESP32_toma"); 
    oocsi.addInt("toggle_Switch_2", switchState);
    oocsi.sendMessage();

    /*
    If the receiver wants to play a song and there is one available already sent
    (the led is high), play the song that was last received.
    */ 
    if(lastSwitchState == 1 && switchState == 0 && digitalRead(ledPin)== HIGH)
    {
      playSong(lastSongFetched);
      lastSongFetched = -1;
    }
    
    lastSwitchState = switchState;
  }
  
  delay(100);
}

/*
The handler method for incoming OOCSI messages
*/
void processOOCSI() {
  /*
  For bi-directional communication, not implemented in hardware.
  The sender also would have a toggle switch, and would have the same 
  receiver functionalities as the actual receiver.
  */ 
  // int ivalue = oocsi.getInt("toggle_Switch_1", -1);
  // Serial.print("received switch state is: ");
  // Serial.println(ivalue);

  // Gets the value of the potentiometer from the sender and updates the last song in queue
  int potentiometerValue = oocsi.getInt("potentiometer_1", -1);
  lastSongFetched = potentiometerValue;

  Serial.print("Switch state: ");
  Serial.println(switchState);
  Serial.print("Potentiometer value received:");
  Serial.println(potentiometerValue);

  /*
  If the toggle switch is not turned, the led turns on, and it it's turned, then play the song directly, 
  without lighting up the led and wait for the user to switch the toggle switch.
  */ 
  if (switchState && potentiometerValue != -1) {
    digitalWrite (ledPin, HIGH);
    Serial.println("Led is ON");
  } else if(switchState == 0) {
    playSong(lastSongFetched);
    lastSongFetched = -1;
  } else {
    digitalWrite (ledPin, LOW); 
    Serial.println("Led is OFF");
  }

}

void playSong(int song)
{  
  Serial.println("Buzzing");

  for (int j = 0; j < song; ++j) {
    // Buzzes as many times as the value of songs.
    long startTime = millis();

    // Plays a buzz for 2 seconds
    while (millis() - startTime < 2000) { 
      // This is a tone of 1kHz 
      for (int i = 0; i < 100; ++i) {
        dacWrite(dacPin, i % 2 ? 0 : 255);
        delayMicroseconds(500);
      }
    }

    // Pause for half a second between buzzes
    delay(500); 
  }

  // "Song" was listened, so led is turning off
  digitalWrite(ledPin, LOW);
}
