#include <SPI.h>

/*
 * SPI code from:
 * https://arduino.stackexchange.com/questions/55805/spi-digital-potentiometer-not-working-on-esp32
 */

static const int spiClk = 1000000;
byte address = 0x00;
SPIClass * vspi = NULL;
const int cs = 5;
const int in = 15;

void setup()
{
  Serial.begin(115200);
  pinMode(cs, OUTPUT);
  pinMode(in, INPUT);
  vspi = new SPIClass(VSPI);
  vspi->begin(); 
}

void loop()
{
  /*
  for (int i = 0; i <= 128; i++)
  {
    digitalPotWrite(i);
    delay(10);
  }
  delay(500);
  for (int i = 128; i >= 0; i--)
  {
    digitalPotWrite(i);
    delay(10);
  }*/
  int inVal = analogRead(in);
  int outVal = map(inVal,0,4095,0,127);
  Serial.print("in: ");
  Serial.print(inVal);
  Serial.print("\tout: ");
  Serial.println(outVal);
  digitalPotWrite(outVal);
  delay(50);
}

int digitalPotWrite(int value)
{
  vspi->beginTransaction(SPISettings(spiClk, MSBFIRST, SPI_MODE0));
  digitalWrite(cs, LOW);
  vspi->transfer(address);
  vspi->transfer(value);
  digitalWrite(cs, HIGH);
  vspi->endTransaction();
}
