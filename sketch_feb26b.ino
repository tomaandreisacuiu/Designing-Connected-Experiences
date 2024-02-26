#include "WiFi.h"
#include "OOCSI.h"

const char* ssid = "TP-Link_6052";   
const char* password = "tomitza2022"; 

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
  delay(1000);
}

void processOOCSI() {
    String svalue = oocsi.getString("toma_key", "-200");

    Serial.println("Received message: " + svalue);
 
}
