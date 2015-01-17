#include <SoftwareSerial.h>
#include <Wire.h>
#include <I2cDiscreteIoExpander.h>

SoftwareSerial PCComm(11,10);  //Rx, Tx

I2cDiscreteIoExpander device(1);

void setup() {
  // put your setup code here, to run once:
  
  Wire.begin();
  
  PCComm.begin(9600);
  PCComm.println("[Board Init]");
  

}

//Address 1 = 21 in hex

void loop()
{
  uint8_t status = 0;
  static uint16_t i;
  
  // display device information on serial console
  Serial.print("Loop ");
  Serial.print(++i, DEC);
  Serial.print(", address ");
  Serial.print(device.getAddress(), DEC);
  Serial.print(", ");
  
  // attempt to write 16-bit word
  status = device.digitalWrite(i);
  if (TWI_SUCCESS == status)
  {
    // display success information on serial console
    Serial.print("write 0x");
    Serial.print(i, HEX);
    Serial.print(", ");
  }
  else
  {
    // display error information on serial console
    Serial.print("write error ");
    Serial.print(status, DEC);
    Serial.print(", ");
  }
  
  // attempt to read 16-bit word
  status = device.digitalRead();
  if (TWI_SUCCESS == status)
  {
    // display success information on serial console
    Serial.print("read 0x");
    Serial.print(device.getPorts(), HEX);
    Serial.println(".");
  }
  else
  {
    // display error information on serial console
    Serial.print("read error ");
    Serial.print(status, DEC);
    Serial.println(".");
  }
  
  delay(1000);
}
