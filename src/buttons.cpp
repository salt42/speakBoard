#include "buttons.h"

#define GAIN 1.5
#define INTERRUPT_MODE true


AudioGeneratorMP3 *mp3;
AudioFileSourceSD *file;
AudioFileSourceBuffer *buff;
AudioOutputI2S *out;

volatile bool playing = 0;

int latchPin = 14;
int dataPin = 15;
int clockPin = 32;

unsigned long previousMillis = 0;
const long buttonHandleInterval = 50;

bool lastButtonStates[64];
int activeButtons = 16;
char trackMapping[64][25] = {
  "",
  "",
  "",
  "/cato.mp3",
  "",
  "",
  "",
  "",
  "",
  "",
  "/leckerlie.mp3",
  "/spielen.mp3",
  "/schmusen.mp3",
  "",
  "/maxi.mp3",
  "/hunger.mp3"
};

int getButtonIndex(const char* name){
    int i = 0;
    for(i = 0; i < 64; i++) {
        if (strcmp(name, trackMapping[i]) == 0) {
            return i;
        }
    }
    return -1;
}

bool playPath(const char* path){
    if (!SD.exists(path) ) return false;
    if(playing && mp3->isRunning()) mp3->stop();
    //file = new AudioFileSourceSD(fileName);
    file = new AudioFileSourceSD(path);
    buff = new AudioFileSourceBuffer(file, 2048);
    
    out->SetGain(GAIN);
    mp3 -> begin(buff,out); //Start playing the track loaded
    playing = 1;
    return true;
}

void playTrackNumber(int trackNumber) {
    Serial.println(trackNumber);
    if (trackMapping[trackNumber]) {
      Serial.println(trackMapping[trackNumber]);
    } else {
      return;
    }
    if(playing && mp3->isRunning()) mp3->stop();
    //file = new AudioFileSourceSD(fileName);
    file = new AudioFileSourceSD(trackMapping[trackNumber]);
    buff = new AudioFileSourceBuffer(file, 2048);
    
    out->SetGain(GAIN);
    mp3 -> begin(buff,out); //Start playing the track loaded
    playing = 1;
}

void handleButtons() { 
  int i;
  int temp = 0;
  int changedButton = -1;
  
  digitalWrite(latchPin,1);
  delayMicroseconds(20);
  digitalWrite(latchPin,0);
  
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, INPUT);

  for (i = activeButtons - 1; i >= 0; i--) {

    digitalWrite(clockPin, 0);
    delayMicroseconds(2);
    temp = digitalRead(dataPin);

    if (temp) {
      if(lastButtonStates[i] == false) {
        changedButton = i;
      }
      lastButtonStates[i] = true;
    } else {
      if(lastButtonStates[i] == true) {
        //changed !!!!!!!!
      }
      lastButtonStates[i] = false;
    }

    digitalWrite(clockPin, 1);
  }
  
  if (changedButton != -1) {
    if (INTERRUPT_MODE) {
      playTrackNumber(changedButton);
    } else {
      if(playing && mp3->isRunning()) return;
      playTrackNumber(changedButton);
    }
  }
}

void setup_buttons() {

    pinMode(latchPin, OUTPUT);
    pinMode(clockPin, OUTPUT); 
    pinMode(dataPin, INPUT);

    out = new AudioOutputI2S();
    out -> SetPinout(33,27,12);
    //out->SetOversampling(64);
    mp3 = new AudioGeneratorMP3();
}

void loop_buttons(){
    unsigned long currentMillis = millis();

    if (currentMillis - previousMillis >= buttonHandleInterval) {
        previousMillis = currentMillis;
        handleButtons();
    }

    if(playing && mp3->isRunning()) {
        if (!mp3->loop()) {
            mp3->stop();
            playing = 0;
            Serial.println("Stopped");
        }
    }
}

