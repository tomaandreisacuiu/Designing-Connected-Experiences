#include "WiFi.h"
#include "OOCSI.h"

const char* ssid = "Parapet_Cafe";
const char* password = "bananaMiau206";

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

  oocsi.check();
   


  delay(5000);
}

void processOOCSI() {
  int ivalue = oocsi.getInt("toggle_Switch_group8", 0);

  if (ivalue == 0 ) {
    digitalWrite (ledPin, LOW);
  } else {
    digitalWrite (ledPin, HIGH);  
  }

  Serial.print("switch state is: ");
  Serial.println(ivalue);

  int potentiometerValue = oocsi.getInt("potentiometer_group8", -1);

}
