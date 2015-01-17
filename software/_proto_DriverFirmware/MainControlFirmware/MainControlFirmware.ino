#include <SoftwareSerial.h>
#include <Wire.h>
#include <I2cDiscreteIoExpander.h>

SoftwareSerial PCComm(11,10);  //Rx, Tx

I2cDiscreteIoExpander device(1);

void setup() {
  // put your setup code here, to run once:
  
  Wire.begin();
  
  PCComm.begin(9600);
  PCComm.print("\n\r[Board Init]\n\r");
  

}

//Address 1 = 21 in hex

const uint8_t dispRight = (1<<7);
const uint16_t dispLeft = (dispRight<<8);
const uint8_t charsL [16] = {0b1000000,0b1111001,0b0100100,0b0110000,0b0011001,0b0010010,0b0000010,0b1111000,0b0000000,0b0010000,0b0001000,0b0000011,0b1000110,0b0100001,0b0000110,0b0001110};
const uint8_t nothingL = 0b1111111;

enum MasterStates {
  POLLING,
  DISPLEFT,
  DISPRIGHT,
  MISC
} CurrentState = DISPLEFT;


void loop()
{
  uint8_t status = 0;
  static uint16_t iter = 0;
  static uint16_t i = 4;
  uint16_t to_write = 0;
  
  switch(CurrentState) {
    case DISPLEFT:
      to_write += dispLeft;
      to_write += charsL[1];
      to_write += (charsL[2] << 8);
      
      CurrentState = DISPRIGHT;
      break;
    case DISPRIGHT:
      to_write += dispRight;
      to_write += charsL[3];
      to_write += (charsL[4] << 8);
      
      CurrentState = DISPLEFT;
      break;
  }
  
  
  //to_write = 0 + charsL[4] + (charsL[2] << 8) + dispLeft;
  
  status = device.digitalWrite(to_write);
  
  
  /*
  
  PCComm.print("[Wrote '");
  PCComm.print(to_write, BIN);
  PCComm.print("' to board iter: ");
  PCComm.print(iter); 
  PCComm.print("]\n\r");
  */
  
  //delayMicroseconds(100);
  ++iter;
}


/* "Recycle Bin" 

             //  mgfedcbamgfedcba
  //to_write = 0b0100000101000000;//(1<<1) + (1<<2) + (1<<3) + (1<<4) + (1<<5) + (1<<6) + (1<<7);


*/
