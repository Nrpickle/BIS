#include <SoftwareSerial.h>

#define SSID    "NickNet"
#define PASS    "27304600"
#define DST_IP  "74.208.156.175" //Nickmccomb.net

#define LED 13

SoftwareSerial ESPComm(11, 10); //Rx, Tx

void setup(){ 
	//Open ESPComm communication
	ESPComm.begin(9600);
	ESPComm.setTimeout(5000);
	
	Serial.begin(9600);
	Serial.println("\n\r[ESP8266 Interface Test]");
	
	pinMode(LED, OUTPUT);
	digitalWrite(LED, LOW);
	
	//Test if the module is ready
	ESPComm.print("AT\015\012");
	delay(2000);
	if(ESPComm.available())
	{
		Serial.println("Module is ready");
	}
	else
	{
		Serial.println("The module has not responded.");
		while(1);
	}
	
	if(ESPComm.find("OK")){
		Serial.println("[Module Responded OK]");
	}
	/* else {
		testFailed();
	} */
	
	delay(500);
	
	Serial.println("[Prompting Reset]");
	ESPComm.print("AT+RST\015\012");
	
	if(ESPComm.find("OK")){
		Serial.println("[Module Responded OK]");
	}
	else {
		testFailed();
	}
	
	Serial.println("[Attempting to connect to WiFi]");
	bool connected = false;
	for (int i = 0; i < 5; i++){
		if(connectWiFi()){
			connected = true;
			break;
		}
	}
	if(!connected){
		Serial.println("[### Failed to connect to WiFi ###]");
		testFailed();
	}
	
	digitalWrite(LED, HIGH);
	delay(1000);	
	
	Serial.println("[Set single connection mode]");
	ESPComm.print("AT+CIPMUX=0\015\012");
	
	if(ESPComm.find("OK")){
		Serial.println("[Module Responded OK]");
	}
	
	Serial.println("[Requesting IP]");
	ESPComm.print("AT+CIFSR\015\012");
	for(int i = 0; i < 500; ++i){
		if(ESPComm.available()){
			Serial.print((char) ESPComm.read());
		}
		delay(1);
	}
}

int loops = 1;
int failCount = 0;

void loop(){
	Serial.print("[Using External Code] [loop count: ");
	Serial.print(loops);
	Serial.print("] [failCount: "); 
	Serial.print(failCount);
	Serial.println("]");
//	reset();  //only CERTAIN way I've found of keeping it going
//  delay(5000);  //esp takes a while to restart
 // ESPComm.print("loops = ");  //check for successful connections to server
  //ESPComm.println(loops); 
  loops++;
  String cmd = "AT+CIPSTART=\"TCP\",\"";  //make this command: AT+CPISTART="TCP","146.227.57.195",80
  cmd += DST_IP;
  cmd += "\",80";
  cmd += "\015\012";
  
  ESPComm.print(cmd);  //send command to device

  delay(2000);  //wait a little while for 'Linked' response - this makes a difference
  if(ESPComm.find("Linked"))  //message returned when connection established WEAK SPOT!! DOESN'T ALWAYS CONNECT
  {
   Serial.print("Connected to server at ");  //debug message
   Serial.println(DST_IP);
  }
  else
  {
    Serial.println("'Linked' response not received");  //weak spot! Need to recover elegantly
	++failCount;
  }

  cmd =  "GET / HTTP/1.1\r\n";  //construct http GET request
  cmd += "Host: nickmccomb.com\r\n\r\n";        //test file on my web
  ESPComm.print("AT+CIPSEND=");                //www.cse.dmu.ac.uk/~sexton/test.txt
  ESPComm.print(cmd.length());  //esp8266 needs to know message length of incoming message - .length provides this
  ESPComm.print("\015\012");
  
  if(ESPComm.find(">"))    //prompt offered by esp8266
  {
    Serial.println("found > prompt - issuing GET request");  //a debug message
    ESPComm.print(cmd);  //this is our http GET request
	ESPComm.print("\015\012");
  }
  else
  {
    ESPComm.print("AT+CIPCLOSE");  //doesn't seem to work here?
	ESPComm.print("\015\012");
    Serial.println("No '>' prompt received after AT+CPISEND");
  }

  //Parse the returned header & web page. Looking for 'Date' line in header

  
  if (ESPComm.find("Date: ")) //get the date line from the http header (for example)
  {
    for (int i=0;i<31;i++)  //this should capture the 'Date: ' line from the header
    {
      if (ESPComm.available())  //new cahracters received?
      {
        char c=ESPComm.read();  //print to console
        Serial.write(c);
      }
      else i--;  //if not, keep going round loop until we've got all the characters
    }
  }

  ESPComm.print("AT+CIPCLOSE");  
  ESPComm.print("\015\012");

  if(ESPComm.find("Unlink"))  //rarely seems to find Unlink? :(
  {
    Serial.println("Connection Closed Ok...");
  }
  else
  {
    //ESPComm.println("connection close failure");
  }
	
	
	
// Grab a webpage by using IP only
/*
  String cmd = "AT+CIPSTART=\"TCP\",\"";
  cmd += DST_IP;
  cmd += "\",80";
  ESPComm.println(cmd);
  Serial.println(cmd);
  if (ESPComm.find("Error")) return;
  cmd = "GET / HTTP/1.0\r\n\r\n";
  ESPComm.print("AT+CIPSEND=");
  ESPComm.println(cmd.length());
  if (ESPComm.find(">"))
  {
    Serial.print(">");
  } else
  {
    ESPComm.println("AT+CIPCLOSE");
    Serial.println("connect timeout");
    delay(1000);
    return;
  }
  ESPComm.print(cmd);
  delay(2000);
  //ESPComm.find("+IPD");
  while (ESPComm.available())
  {
    char c = ESPComm.read();
    Serial.write(c);
    if (c == '\r') Serial.print('\n');
  }
  Serial.println("====");
  delay(1000);
*/	
  
	
	/*
	Serial.println("[Prompting for available networks]");
	ESPComm.print("AT+CWLAP\015\012");
	*/
	
	//Command prompt to send commands to ESP module
	/*
	while(1){
		if(ESPComm.available()){
			Serial.print((char) ESPComm.read());
		}
		
		if(Serial.available()){
			ESPComm.print((char)Serial.read());
		}
	}
	*/
	
}

bool connectWiFi() {
	ESPComm.println("AT+CWMODE=1");
	
	String cmd = "AT+CWJAP=\"";
	cmd += SSID;
	cmd += "\",\"";
	cmd += PASS;
	cmd += "\"";
	cmd += "\015\012";
	
	Serial.println(cmd);
	ESPComm.print(cmd);
	
	if(ESPComm.find("OK")) {
		Serial.println("[Connected to WiFi]");
		return true;
	}
	else {
		Serial.println("[# Failed to connect to Wifi #]\n");
		return false;
	}

}

void testFailed(){
	Serial.println("[### Previous Test Failed ###]");
	while(1);
}
