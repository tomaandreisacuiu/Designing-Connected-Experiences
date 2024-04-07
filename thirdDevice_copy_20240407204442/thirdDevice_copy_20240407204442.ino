#include "WiFi.h"
#include "OOCSI.h"
#include "FS.h"
#include "SD.h"
#include "SPI.h"

#include "YX5300_ESP32.h"
#define RX 16
#define TX 17

YX5300_ESP32 mp3;

// const int buttonPin = 13;
int hoi = 1;

// LED pin
const int ledPin = 2;

//Toggle switch variables and constants
const int toggleSwitchPin = 13;  // toggle switch pin
int lastSwitchState = -1;
int switchState = 1;

int lastSongFetched = -1;

int pair = -1;
int song = -1;
int sing = 1;
int lastsong = -1;

// Network constants
// const char* ssid = "Ziggo2070639";
// const char* password = "upjfzbke5zJu";

const char* ssid = "Parapet_Cafe";
const char* password ="bananaMiau206";

// const char* ssid = "banana";
// const char* password ="gluglu2469";

OOCSI oocsi = OOCSI();

void setup() {
  Serial.begin(115200);

  // BEGIN WiFi Connection
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  Serial.println("Connected to WiFi");
  // END - WiFI Connection setup done (if successful, otherwise keeps trying)

  // BEGIN OOCSI Connection
  oocsi.connect("device3Team8DBS", "oocsi.id.tue.nl", ssid, password, processOOCSI);
  Serial.println("Connected to OOCSI");

  mp3 = YX5300_ESP32(Serial2, RX, TX);

  // pinMode(buttonPin, INPUT_PULLUP);
  pinMode(toggleSwitchPin, INPUT_PULLUP);
  pinMode(ledPin, OUTPUT);

  // LED to be turned off initially
  digitalWrite(ledPin, LOW);
}

void loop() {
  oocsi.check();
  // The state of the toggle switch is updated here
  switchState = digitalRead(toggleSwitchPin);
  // Serial.println(switchState);

  // If the switch state has changed, the program handles it as follows:
  if (switchState != lastSwitchState) {
    /*
    Sends a message to the sender with this new state which describes
    if the receiver wants to play the songs transmitted.
    */
    oocsi.newMessage("wepAppTeam8DBSU");
    oocsi.addInt("toggle_Switch_3", switchState);
    oocsi.sendMessage();
  }
  if (lastSwitchState != 1) {
    mp3.stop();
    digitalWrite(ledPin, LOW);
  }
  lastSwitchState = switchState;
  delay(100);
}


void processOOCSI() {

  int pairing = oocsi.getInt("pairState", -1);
  pair = pairing;

  if (pair == 1) {
    Serial.println("supposed to play");
    //mp3.playTrack(1);
    digitalWrite(ledPin, HIGH);
  } else {
    mp3.stop();
    digitalWrite(ledPin, LOW);
    Serial.println("supposed to stop");
  }

  //PLAY A SONG when web app button is pressed
  int songFromWeb = oocsi.getInt("songNumber", -1);
  song = songFromWeb;
  Serial.println(song);
  if (song != -1) {
    if (song == lastsong) {
      sing++;
      if ((sing % 2) == 0) {
        mp3.stop();
      } else {
        mp3.playTrack(song);
        Serial.println("playing song");
      }
    } else {
      mp3.playTrack(song);
      Serial.println("playing song");
      sing = 1;
    }
    lastsong = song;
  }
}
