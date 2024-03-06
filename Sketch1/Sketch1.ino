#include "WiFi.h"
#include "OOCSI.h"
#include "Audio.h"
#include "FS.h"
#include "SD.h"
#include "SPI.h"
#include <AudioGeneratorMP3.h>
#include <AudioFileSourceSD.h>
#include <AudioOutputI2S.h>

// Network variables
const char* ssid = "Parapet_Cafe";
const char* password = "bananaMiau206";
OOCSI oocsi = OOCSI();

//Toggle switch variables
const int toggleSwitchPin = 18;
int lastSwitchState = -1;   

//Potentiometer variables
int potPin = 6;
int lastAnalogValue = -1;
int lastPotInterval = -1;

//Audio variables
AudioGeneratorMP3 *mp3 = nullptr; // Initialize mp3 pointer to nullptr
AudioFileSourceSD *file = nullptr; // Initialize file pointer to nullptr
AudioOutputI2S *output = nullptr; // Initialize output pointer to nullptr



void setup() {
  Serial.begin(115200); 
  pinMode(14, INPUT_PULLUP);
  
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  Serial.println("Connected to WiFi");

  oocsi.connect("senderESP32_toma", "oocsi.id.tue.nl", "ssid", "password", processOOCSI);

  Serial.println("Connected to OOCSI");
  SPI.begin(12, 13, 11, 10);
  if (!SD.begin(10, SPI)) {
    Serial.println("Card Mount Failed");
    return;
  }
  uint8_t cardType = SD.cardType();

  if(cardType == CARD_NONE){
    Serial.println("No SD card attached");
    return;
  }

  // Serial.print("SD Card Type: ");
  // if(cardType == CARD_MMC){
  //   Serial.println("MMC");
  // } else if(cardType == CARD_SD){
  //   Serial.println("SDSC");
  // } else if(cardType == CARD_SDHC){
  //   Serial.println("SDHC");
  // } else {
  //   Serial.println("UNKNOWN");
  // }

  // uint64_t cardSize = SD.cardSize() / (1024 * 1024);
  // Serial.printf("SD Card Size: %lluMB\n", cardSize);
}

void loop() {
  oocsi.check();
  
  int switchState = digitalRead(toggleSwitchPin);

  if (switchState != lastSwitchState) {
   
    oocsi.newMessage("receiverESP32_toma"); 
    oocsi.addInt("toggle_Switch_group8", switchState);
    oocsi.sendMessage();
    
    lastSwitchState = switchState;

    Serial.print("Sent switch state: ");
    Serial.println(switchState);
  }
  playAudio(0);
  int analogValue = analogRead(potPin); 
  int potInterval = map(analogValue, 0, 8000, 0, 2);
  if (potInterval != lastPotInterval) {    
      oocsi.newMessage("receiverESP32_toma");
      oocsi.addInt("potentiometer_group8", potInterval);
      oocsi.sendMessage();

      lastPotInterval = potInterval;
  }
  delay(100);
}

void processOOCSI() {
  // Processing incoming data
//   String svalue = oocsi.getString("toma_key", "-200");
//   Serial.println("Received message: " + svalue);
}
