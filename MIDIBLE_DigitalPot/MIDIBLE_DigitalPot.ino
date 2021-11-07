#include <SPI.h>
#include <Arduino.h>
#include <BLEMidi.h>

/*
 * SPI code from:
 * https://arduino.stackexchange.com/questions/55805/spi-digital-potentiometer-not-working-on-esp32
 */

#define OUT 5
#define IN 2
#define CC 119
static const int spiClk = 1000000;
byte address = 0x00;
SPIClass * vspi = NULL;
uint8_t level      = 0,
        ccLevel    = 0,
        noteVel    = 0,
        noteLevel  = 0,
        dly        = 41,
        maxCount   = 3,
        levelCount = 3,
        prevInVal = 0,
        inVal      = 0,
        outVal     = 0,
        prevOutVal = 0;
float fade;
bool MIDIctl = false;

void onNoteOn(uint8_t channel, uint8_t note, uint8_t velocity, uint16_t timestamp)
{
  if(velocity > 0) {
      MIDIctl = true;
      noteVel = velocity;
      levelCount = maxCount;    
    }
  //Serial.printf("Received note on : channel %d, note %d, velocity %d (timestamp %dms)\n", channel, note, velocity, timestamp);
}

void onControlChange(uint8_t channel, uint8_t controller, uint8_t value, uint16_t timestamp)
{
    if(controller = CC) {
      MIDIctl = true;
      ccLevel = value;
    }
    //Serial.printf("Received control change : channel %d, controller %d, value %d (timestamp %dms)\n", channel, controller, value, timestamp);
}

void connected()
{
  Serial.println("Connected");
  for(int i=0; i<3; i++) {
    digitalPotWrite(64);
    delay(100);    
    digitalPotWrite(0);
    delay(100);    
  }
}

void setup()
{
  Serial.begin(115200);
  pinMode(OUT, OUTPUT);
  pinMode(IN, INPUT);

  // SPI init:
  vspi = new SPIClass(VSPI);
  vspi->begin();
  // BLE MIDI init:
  BLEMidiServer.begin("Led Ring 1");
  BLEMidiServer.setOnConnectCallback(connected);
  BLEMidiServer.setOnDisconnectCallback([](){     // lambda function
    Serial.println("Disconnected");
  });
  BLEMidiServer.setNoteOnCallback(onNoteOn);
  BLEMidiServer.setControlChangeCallback(onControlChange);
  //BLEMidiServer.setProgramChangeCallback(onProgramChange);
  //BLEMidiServer.enableDebugging();

  digitalPotWrite(0);
  delay(100);
  digitalPotWrite(0);
}

void loop()
{
  inVal = map(analogRead(IN),0,4095,0,127);
  if(abs(inVal - prevInVal) > 20) {
    MIDIctl = false;
  }
  if(abs(inVal - prevInVal)>2 && !MIDIctl) {
    prevInVal = inVal;
    outVal = inVal;
  }
  if(MIDIctl) {
    fade = (float) levelCount / 3;
    noteLevel = (uint8_t) ( (float) noteVel * fade);
    outVal = noteLevel + ccLevel;
    if(outVal > 127) outVal = 127;    
  }
  if(prevOutVal != outVal) {
    digitalPotWrite(outVal);
     prevOutVal = outVal;
  }
  if(levelCount>0) levelCount--;
  delay(dly); 
}

int digitalPotWrite(int value)
{
  Serial.println(value);
  vspi->beginTransaction(SPISettings(spiClk, MSBFIRST, SPI_MODE0));
  digitalWrite(OUT, LOW);
  vspi->transfer(address);
  vspi->transfer(value);
  digitalWrite(OUT, HIGH);
  vspi->endTransaction();
}
