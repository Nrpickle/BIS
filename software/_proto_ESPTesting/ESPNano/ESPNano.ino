#include <SoftwareSerial.h>

#define SSID    "NickNet"
#define PASS    "27304600"
#define DST_IP  "220.181.111.85"

SoftwareSerial dbgSerial(11, 10);

void setup(){
	//Open serial communication
	Serial.begin(115200);
	Serial.setTimeout(5000);
	
	dbgSerial.begin(57600);
	dbgSerial.println("ESP8266 Demo");
	
	//Test if the module is ready
	Serial.println("AT");
	delay(2000);
	if(Serial.available())
	{
		dbgSerial.println("Module is ready");
	}
	else
	{
		dbgSerial.println("The module has not responded.");
		while(1);
	}
}

void loop(){
	if(Serial.available()){
		dbgSerial.print(Serial.read(), DEC);
	}
}