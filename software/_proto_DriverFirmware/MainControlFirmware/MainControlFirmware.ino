/*

Bus Information System Firmware
Written by Nick McComb
mccombn@onid.oregonstate.edu / nick@nickmccomb.net

You can find this code (and related files) at https://github.com/Nrpickle/BIS

If you are using this code from GitHub, you can find the Arduino libraries in the 'auxiliary' folder.

Libraries used:
I2cDiscreteIoExpander: https://github.com/4-20ma/I2cDiscreteIoExpander
*/


#include <SoftwareSerial.h>
#include <Wire.h>
#include <I2cDiscreteIoExpander.h>
#include <TimerOne.h>

//Defines
#define LEFT 0
#define RIGHT 1

#define TIMING_CONSTANT 1000
#define INIT_TIMING_COUNTER 2

//Create program-wide objects
SoftwareSerial PCComm(11,10);  //Rx, Tx

//I2cDiscreteIoExpander device(0);
I2cDiscreteIoExpander device2(1);

//Timer t;  //Instantiate timer

class Display {
	public:
		Display(int deviceID);
		~Display();
		void dispRight();
		void dispLeft();
		int writeToDisplay();
		void setLeft(char * input);
		void setLeft(char input);
//		void setLeft(char input, char input2);
		void setRight(char * input);
		void setRight(char input);
//		void setRight(char input, char input2);
		void setAll(char * input);
		void setAll(char input);
	private:
		I2cDiscreteIoExpander * device;
		uint16_t current;
		uint16_t left;
		uint16_t right;
};

void setup() {
  
  Wire.begin();  //A4(SDA), A5(SCL)
  
  PCComm.begin(57600);
  PCComm.print("\n\r[Board Init]\n\r");
  
  Timer1.initialize(3000);  //def 100000
  Timer1.attachInterrupt( dispDrive );
  //t.every(2000, sayHello);

}

//Address 1 = 21 in hex

//Constants for the I2C Expander
const uint8_t dispRight = (1<<7);
const uint16_t dispLeft = (dispRight<<8);
const uint8_t charsL [16] = {0b1000000,0b1111001,0b0100100,0b0110000,0b0011001,0b0010010,0b0000010,0b1111000,0b0000000,0b0010000,0b0001000,0b0000011,0b1000110,0b0100001,0b0000110,0b0001110};  //Contains the constants for the 7seg values
const uint8_t nothingL = 0b1111111;
const uint8_t errorL = 0b0110110;
const uint8_t iL = 0b1111011; //Just segment c_str
const uint8_t tL = 0b0000111; // c d e f g
const uint8_t nL = 0b0101011; //c g e

enum MasterStates {
  POLLING,
  DISPLEFT,
  DISPRIGHT,
  MISC
} CurrentState = DISPLEFT;

static Display disp(0);
static Display disp2(1);

void loop()
{
  static uint16_t timing = 1;
  static uint16_t i = 0;
  static uint8_t initTimer = 0;
  uint8_t status = 0;
  uint8_t status2 = 0;
  
  //uint16_t to_write = 0;
  
  if(!timing){  //Time to do the asynch tasks
	//PCComm.println("[Processing Asynch Tasks]");
	
	i = ++i%10;
	char temp[2];
	
	//Initialization routine
	if (initTimer < INIT_TIMING_COUNTER){
		++initTimer;
		disp2.setAll("bi5 ");
		disp.setAll("init");
	}
	else {  //Main operating mode
		disp.setAll("1234");
		disp2.setLeft("ab");
		disp2.setRight("cd");
	}
	
  
  }
  
  switch(CurrentState) {
    case DISPLEFT:
	  disp.dispLeft();
	  disp2.dispLeft();
      CurrentState = DISPRIGHT;
      break;
    case DISPRIGHT:
	  disp.dispRight();
	  disp2.dispRight();
      CurrentState = DISPLEFT;
      break;
  }
  
  disp.writeToDisplay();
  disp2.writeToDisplay();
  
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

//Drives the I2C interface for the displays
void dispDrive(){
	
  //PCComm.println("[dispDrive]");
  

  

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
	else if (left_digit == 't'){
		to_write += tL;
	}
	else if (left_digit == 'i'){
		to_write += iL;
	}
	else if (left_digit == 'n'){
		to_write += nL;
	}
	else {
		to_write += errorL;
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
	else if (right_digit == 't'){
		to_write += (tL << 8);
	}
	else if (right_digit == 'i'){
		to_write += (iL << 8);
	}
	else if (right_digit == 'n'){
		to_write += (nL << 8);
	}
	else {
		to_write += (errorL << 8);
	}
	
}

/* * * * * * * * * * * * 
DISPLAY STRUCT FUNCTIONS
* * * * * * * * * * * */

//Display constructor
Display::Display(int deviceID){
	device = new I2cDiscreteIoExpander(deviceID);
	if(deviceID == 0){
		setAll("b15");
	}
	current = 0;
	left = 0;
	right = 0;
}

Display::~Display(){
	delete device;
}

void Display::dispRight(){
	current = right;
}

void Display::dispLeft(){
	current = left;
}

int Display::writeToDisplay(){
	return device->digitalWrite(current);
}

void Display::setLeft(char * input){
	input[0] = tolower(input[0]);
	input[1] = tolower(input[1]);
	configureToWrite(left, LEFT, input[0], input[1]);
}

void Display::setLeft(char input){
	input = tolower(input);
	configureToWrite(left, LEFT, input, ' ');
}

/*
//NOT WORKING
void Display::setLeft(char input, char input2){
	input = tolower(input);
	input2 = tolower(input2);
	configureToWrite(left, LEFT, input, input2);
}
*/

void Display::setRight(char * input){
	input[0] = tolower(input[0]);
	input[1] = tolower(input[1]);
	configureToWrite(right, RIGHT, input[0], input[1]);
}

void Display::setRight(char input){
	input = tolower(input);
	configureToWrite(right, RIGHT, input, ' ');
}

/*
//NOT WORKING
void Display::setRight(char input, char input2){
	input = tolower(input);
	input2 = tolower(input2);
	configureToWrite(right, RIGHT, input, input2);
}
*/

void Display::setAll(char * input){
	char temp[2];
	temp[0] = input[0];
	temp[1] = input[1];
	setLeft(temp);
	temp[0] = input[2];
	temp[1] = input[3];
	setRight(temp);
}

void Display::setAll(char input){
	setLeft(input);
	setRight(' ');
}

/* * * * * * * * * * * * * * 
END DISPLAY STRUCT FUNCTIONS
* * * * * * * * * * * * * */


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
