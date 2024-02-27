#include "WiFi.h"
#include "OOCSI.h"

const char* ssid = "iotroam";
const char* password = "FoaQOCFMAe";

const int toggleSwitchPin = 18;
int lastSwitchState = -1;   

int lastAnalogValue = -1;

OOCSI oocsi = OOCSI();

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
    oocsi.addInt("toma_key", switchState);
    oocsi.sendMessage();
    
    lastSwitchState = switchState;

    Serial.print("Sent switch state: ");
    Serial.println(switchState);
  }

  int analogValue = analogRead(6); 

  if (analogValue != lastAnalogValue) {
    Serial.print("Analog: ");
    Serial.print(analogValue);
    
    oocsi.newMessage("receiverESP32_toma"); 
    oocsi.addInt("toma_key", analogValue);
    oocsi.sendMessage();

    Serial.print("Sent potentiometer value: ");
    Serial.println(analogValue);

    lastAnalogValue = analogValue;
  }

  delay(50);
}

void processOOCSI() {
  // Processing incoming data
   String svalue = oocsi.getString("toma_key", "-200");
   Serial.println("Received message: " + svalue);
}

