#include "WiFi.h"
#include "OOCSI.h"

const char* ssid = "iotroam";
const char* password = "FoaQOCFMAe";

OOCSI oocsi = OOCSI();

void setup() {
  Serial.begin(115200); 
  
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  Serial.println("Connected to WiFi");

  oocsi.connect("receiverESP32_toma", "oocsi.id.tue.nl", "ssid", "password", processOOCSI);

  Serial.println("Connected to OOCSI");
}

void loop() {
//  Serial.println("Hello, World!"); 
//  delay(1000); 
  oocsi.check();
   
  oocsi.newMessage("senderESP32_toma");
  oocsi.addString("toma_key", "receiving from sender receiverESP32_toma");
  oocsi.sendMessage();
    
  Serial.println("Message sent");

  delay(5000);
}

void processOOCSI() {
    String svalue = oocsi.getString("toma_key", "-200");

    Serial.println("Received message: " + svalue);
 
}

