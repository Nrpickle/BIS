/*

Bus Information System Firmware
Written by Nick McComb
mccombn@onid.oregonstate.edu / nick@nickmccomb.net

You can find this code (and related files) at https://github.com/Nrpickle/BIS

If you are using this code from GitHub, you can find the Arduino libraries in the 'aux' folder.

Libraries used:
I2cDiscreteIoExpander: https://github.com/4-20ma/I2cDiscreteIoExpander
Jack Christensen's Timer: https://github.com/JChristensen/Timer
*/


#include <SoftwareSerial.h>
#include <Wire.h>
#include <I2cDiscreteIoExpander.h>
#include <Timer.h> //http://github.com/JChristensen/Timer

//Defines
#define LEFT 0
#define RIGHT 1

#define TIMING_CONSTANT 1000

//Create program-wide objects
SoftwareSerial PCComm(11,10);  //Rx, Tx

I2cDiscreteIoExpander device(0);
I2cDiscreteIoExpander device2(1);

Timer t;  //Instantiate timer

class Display {
	public:
		Display();
		uint16_t current;
		uint16_t left;
		uint16_t right;
};

void setup() {
  
  Wire.begin();  //A4(SDA), A5(SCL)
  
  PCComm.begin(57600);
  PCComm.print("\n\r[Board Init]\n\r");
  
  //t.every(2000, sayHello);

}

//Address 1 = 21 in hex

//Constants for the 
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
  static Display disp[2];
  
  static uint16_t timing = 0;
  static uint16_t i = 0;
  uint8_t status = 0;
  uint8_t status2 = 0;
  
  uint16_t to_write = 0;
  
  if(!timing){
	//PCComm.println("[Processing configureToWrite]");
	
	i = ++i%10;
	
	configureToWrite(disp[0].left, LEFT, '5' , ' ');
	configureToWrite(disp[0].right, RIGHT, '3', i + 48);
  }
  
  switch(CurrentState) {
    case DISPLEFT:
	  disp[0].current = disp[0].left;
      CurrentState = DISPRIGHT;
      break;
    case DISPRIGHT:
	  disp[0].current = disp[0].right;
      CurrentState = DISPLEFT;
      break;
  }
  
  
  status = device.digitalWrite(disp[0].current);
  status2 = device2.digitalWrite(disp[0].current);
  
  
  /*
  PCComm.print("[Wrote '");
  PCComm.print(to_write, BIN);
  PCComm.print("' to board timing: ");
  PCComm.print(timing); 
  PCComm.print("]\n\r");
  */
  
  ++timing;
  
  timing = timing % TIMING_CONSTANT;
  
}

void sayHello(){
	PCComm.println("Hello!");
}

/*
  @func configureToWrite
  @desc Sets up the to_write 16 bit uint to be written to the display boards
  
  @param {uint16_t} to_write - 16 bit integer
  @param {bool} l_r -  LEFT or RIGHT, depending on the disp half you want to write to
  @param {char} left_digit - 0-9 or a-f (LOWER CASE only), char to be displayed on the left. ' ' is nothing
  @param {char} right_digit - 0-9 or a-f (LOWER CASE only), char to be displayed on the right. ' ' is nothing
  @note REQUIRES a lower case char when using a-f currently
*/
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
	else if (left_digit >= 97 && left_digit <= 102){
		to_write += charsL[(int)left_digit - 87];
	}
	else if (left_digit == ' '){
		to_write += nothingL;
	}
	
	if(isdigit(right_digit)){
		to_write += (charsL[(int)right_digit - 48] << 8);
	}
	else if (right_digit >= 97 && right_digit <= 102){
		to_write += (charsL[(int)right_digit - 87] << 8);
	}
	else if (right_digit == ' '){
		to_write += (nothingL << 8);
	}
	
}

//Display constructor
Display::Display(){
	current = 0;
	left = 0;
	right = 0;
}

//Display struct classes

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
