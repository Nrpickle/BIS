#include <SoftwareSerial.h>

#define SSID    "NickNet"
#define PASS    "27304600"
#define DST_IP  "140.211.12.4" //Corvalis, OR Transit System

#define LED 13

SoftwareSerial dbgSerial(11, 10); //Rx, Tx

void setup(){
	//Open serial communication
	Serial.begin(9600);
	Serial.setTimeout(5000);
	
	dbgSerial.begin(9600);
	dbgSerial.println("\n\r[ESP8266 Interface Test]");
	
	pinMode(LED, OUTPUT);
	digitalWrite(LED, LOW);
	
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
	/* else {
		testFailed();
	} */
	
	delay(500);
	
	dbgSerial.println("[Prompting Reset]");
	Serial.print("AT+RST\015\012");
	
	if(Serial.find("OK")){
		dbgSerial.println("[Module Responded OK]");
	}
	else {
		testFailed();
	}
	
	dbgSerial.println("[Attempting to connect to WiFi]");
	bool connected = false;
	for (int i = 0; i < 5; i++){
		if(connectWiFi()){
			connected = true;
			break;
		}
	}
	if(!connected){
		dbgSerial.println("[### Failed to connect to WiFi ###]");
		testFailed();
	}
	
	digitalWrite(LED, HIGH);
	delay(1000);	
	
	dbgSerial.println("[Set single connection mode]");
	Serial.print("AT+CIPMUX=0\015\012");
	
	if(Serial.find("OK")){
		dbgSerial.println("[Module Responded OK]");
	}
	
	dbgSerial.println("[Requesting IP]");
	Serial.print("AT+CIFSR\015\012");
	for(int i = 0; i < 500; ++i){
		if(Serial.available()){
			dbgSerial.print((char) Serial.read());
		}
		delay(1);
	}
}

void loop(){
	

  String cmd = "AT+CIPSTART=\"TCP\",\"";
  cmd += DST_IP;
  cmd += "\",80";
  Serial.println(cmd);
  dbgSerial.println(cmd);
  if (Serial.find("Error")) return;
  cmd = "GET / HTTP/1.0\r\n\r\n";
  Serial.print("AT+CIPSEND=");
  Serial.println(cmd.length());
  if (Serial.find(">"))
  {
    dbgSerial.print(">");
  } else
  {
    Serial.println("AT+CIPCLOSE");
    dbgSerial.println("connect timeout");
    delay(1000);
    return;
  }
  Serial.print(cmd);
  delay(2000);
  //Serial.find("+IPD");
  while (Serial.available())
  {
    char c = Serial.read();
    dbgSerial.write(c);
    if (c == '\r') dbgSerial.print('\n');
  }
  dbgSerial.println("====");
  delay(1000);
	
  
	
	/*
	dbgSerial.println("[Prompting for available networks]");
	Serial.print("AT+CWLAP\015\012");
	*/
	
	//Command prompt to send commands to ESP module
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
		dbgSerial.println("[# Failed to connect to Wifi #]\n");
		return false;
	}

}

void testFailed(){
	dbgSerial.println("[### Previous Test Failed ###]");
	while(1);
}
