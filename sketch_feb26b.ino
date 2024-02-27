#include "WiFi.h"
#include "OOCSI.h"

const char* ssid = "iotroam";
const char* password = "FoaQOCFMAe";

const int ledPin = 18;

OOCSI oocsi = OOCSI();

void setup() {
  Serial.begin(115200); 

  pinMode(ledPin, OUTPUT);
  
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
//  digitalWrite(ledPin, HIGH); 
  oocsi.check();
   
//  oocsi.newMessage("senderESP32_toma");
//  oocsi.addString("toma_key", "receiving from sender receiverESP32_toma");
//  oocsi.sendMessage();
//    
//  Serial.println("Message sent");

  delay(5000);
}

void processOOCSI() {
  int ivalue = oocsi.getInt("toma_key", 0);

  if (ivalue == 0 ) {
    digitalWrite (ledPin, LOW);
  } else {
    digitalWrite (ledPin, HIGH);  
  }

  Serial.print("switch state is: ");
  Serial.println(ivalue);

  int potentiometerValue = oocsi.getInt("toma_key", -1);
  Serial.print("potentiometer value is: ");
  Serial.println(potentiometerValue);
}

