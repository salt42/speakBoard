#include "SDCard.h"


void setup_SDCard() {

    delay(500);

    if(!SD.begin(SD_ChipSelectPin)){
        Serial.println("Card Mount Failed");
        return;
    }
    uint8_t cardType = SD.cardType();

    if(cardType == CARD_NONE){
        Serial.println("No SD card attached");
        return;
    }
}