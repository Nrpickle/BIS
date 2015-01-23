#include <SoftwareSerial.h>
#include <Wire.h>
#include <I2cDiscreteIoExpander.h>

SoftwareSerial PCComm(11,10);  //Rx, Tx

I2cDiscreteIoExpander device(1);

#define LEFT 0
#define RIGHT 1

void setup() {
  // put your setup code here, to run once:
  
  Wire.begin();  //A4(SDA), A5(SCL)
  
  PCComm.begin(9600);
  PCComm.print("\n\r[Board Init]\n\r");
  

}

//Address 1 = 21 in hex

const uint8_t dispRight = (1<<7);
const uint16_t dispLeft = (dispRight<<8);
const uint8_t charsL [16] = {0b1000000,0b1111001,0b0100100,0b0110000,0b0011001,0b0010010,0b0000010,0b1111000,0b0000000,0b0010000,0b0001000,0b0000011,0b1000110,0b0100001,0b0000110,0b0001110};  //Contains the constants for the 7seg values
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
	  configureToWrite(to_write, LEFT, '1' , '2');

      CurrentState = DISPRIGHT;
      break;
    case DISPRIGHT:
      configureToWrite(to_write, RIGHT, '3', '4');
	  
      CurrentState = DISPLEFT;
      break;
  }
  
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

void configureToWrite(uint16_t & to_write, bool l_r, char left_digit, char right_digit){
	to_write = 0;
	
	if(l_r == LEFT){
		to_write += dispLeft;
	}
	else if (l_r == RIGHT){
		to_write += dispRight;
	}
	
	if(isdigit(left_digit)){
		to_write += charsL[(int)left_digit - 48];
	}
	
	if(isdigit(right_digit)){
		to_write += (charsL[(int)right_digit - 48] << 8);
	}
	
}

/* "Recycle Bin" 

             //  mgfedcbamgfedcba
  //to_write = 0b0100000101000000;//(1<<1) + (1<<2) + (1<<3) + (1<<4) + (1<<5) + (1<<6) + (1<<7);

	
    //  to_write += dispLeft;
    //  to_write += charsL[1];
    //  to_write += (charsL[2] << 8);
	
    //  to_write += dispRight;
    //  to_write += charsL[3];
    //  to_write += (charsL[4] << 8);
    
*/
