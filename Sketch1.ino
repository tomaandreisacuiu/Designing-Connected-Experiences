#include "WiFi.h"
#include "OOCSI.h"

const char* ssid = "iotroam";
const char* password = "FoaQOCFMAe";

const int toggleSwitchPin = 18;
int lastSwitchState = -1;   

int lastAnalogValue = -1;
int lastPotInterval = -1;

OOCSI oocsi = OOCSI();

struct songChoice {
  int actualValue;
  int startValue;
  int endValue;
  int song; // 1, 2, 3, 4
};

void setup() {
  Serial.begin(115200); 
  
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  Serial.println("Connected to WiFi");

//  oocsi.connect("senderESP32_toma", "oocsi.id.tue.nl", WiFi);
  oocsi.connect("senderESP32_toma", "oocsi.id.tue.nl", "ssid", "password", processOOCSI);

  Serial.println("Connected to OOCSI");
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

  int analogValue = analogRead(6);
//  Serial.print("actual pot value: ");
//  Serial.println(analogValue);
  int currentPotInterval = map(analogValue, 0, 4095, 0, 3); 

  if (currentPotInterval != lastPotInterval) {    
//    oocsi.newMessage("receiverESP32_toma");
//    oocsi.addInt("potentiometer_group8", currentPotInterval);
//    oocsi.sendMessage();

    // play song of index currentPotInterval

    Serial.print("Sent potentiometer interval: ");
    Serial.println(currentPotInterval);

    lastPotInterval = currentPotInterval; // Update the last interval
  }

  delay(50);
}

void processOOCSI() {
  // Processing incoming data
//   String svalue = oocsi.getString("toma_key", "-200");
//   Serial.println("Received message: " + svalue);
}
