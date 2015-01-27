#include <SoftwareSerial.h>

SoftwareSerial PCComm(11,10);  //Rx, Tx

void setup(){
   Serial.begin(9600);
   
   PCComm.begin(115200);
   pinMode(13, OUTPUT);
}


void loop(){
	if(PCComm.available()){
		if(PCComm.read() == 'r'){
			digitalWrite(13, HIGH);
			PCComm.println("Fiiiiiiiiiiiiiiiiine.");
		}
		else {
			digitalWrite(13, LOW);
		}
	}
}