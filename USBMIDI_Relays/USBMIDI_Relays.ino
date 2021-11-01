#include "MIDIUSB.h"

void setup() {
  // put your setup code here, to run once:
  Serial.begin(31250);
  for(int i=2; i<=5; i++) {
    pinMode(i, OUTPUT);
  }
  for(int i=2; i<=5; i++) {
    digitalWrite(i, HIGH);
  }  
}

void loop() {
  midiEventPacket_t rx;
  rx = MidiUSB.read();
  if (rx.header != 0) {
    if(rx.header == 0x9) {
      printMIDImessage("Note  ON", rx);
      digitalWrite(rx.byte2-26, LOW);
    } else if(rx.header == 0x8) {
      printMIDImessage("Note OFF", rx);
      digitalWrite(rx.byte2-26, HIGH);
      Serial.println(rx.byte2-26);
    }
  }
}

void printMIDImessage(String msg, midiEventPacket_t rx) {
      Serial.print(msg);
      Serial.print(": \t");
      Serial.print(rx.byte1);
      Serial.print("\t");
      Serial.print(rx.byte2);
      Serial.print("\t");
      Serial.print(rx.byte3);
      Serial.println(); 
}
