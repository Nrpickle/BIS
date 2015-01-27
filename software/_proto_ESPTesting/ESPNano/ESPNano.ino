#include <SoftwareSerial.h>

#define SSID    "OSU_Access"
#define PASS    ""
#define DST_IP  "220.181.111.85"

SoftwareSerial dbgSerial(11, 10); //Rx, Tx

void setup(){
	//Open serial communication
	Serial.begin(9600);
	Serial.setTimeout(5000);
	
	dbgSerial.begin(9600);
	dbgSerial.println("\n\r[ESP8266 Interface Test]");
	
	//Test if the module is ready
	Serial.print("AT\015\012");
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
	
	if(Serial.find("OK")){
		dbgSerial.println("[Module Responded OK]");
	}
	
	delay(500);
	
	dbgSerial.println("[Prompting Reset]");
	Serial.print("AT+RST\015\012");
	
	if(Serial.find("OK")){
		dbgSerial.println("[Module Responded OK]");
	}
	
	bool connected = false;
	for (int i = 0; i < 5; i++){
		if(connectWiFi()){
			connected = true;
			break;
		}
	}
	if(!connected){
		dbgSerial.print("[### Failed to connect to WiFi ###]");
	}
	
	delay(5000);	
	
	dbgSerial.println("[Set single connection mode]");
	Serial.print("AT+CIPMUX=0\015\012");
	
	if(Serial.find("OK")){
		dbgSerial.println("[Module Responded OK]");
	}
}

void loop(){
	/*
	if(Serial.available()){
		dbgSerial.print((char) Serial.read());
	}
	*/
	
	
	

	
	//dbgSerial.println("[Prompting for available networks]");
	//Serial.print("AT+CWLAP\015\012");
	
	while(1){
		if(Serial.available()){
			dbgSerial.print((char) Serial.read());
		}
		
		if(dbgSerial.available()){
			Serial.print((char)dbgSerial.read());
		}
	}
	
	
	while(1);
}

bool connectWiFi() {
	Serial.println("AT+CWMODE=1");
	
	String cmd = "AT+CWJAP=\"";
	cmd += SSID;
	cmd += "\",\"";
	cmd += PASS;
	cmd += "\"";
	cmd += "\015\012";
	
	dbgSerial.println(cmd);
	Serial.print(cmd);
	
	if(Serial.find("OK")) {
		dbgSerial.println("[Connected to WiFi]");
		return true;
	}
	else {
		dbgSerial.println("[### Failed to connect to Wifi ###]");
		return false;
	}

}

