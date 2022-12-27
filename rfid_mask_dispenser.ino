#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>  
#include <SPI.h>
#include <MFRC522.h>
#define SS_PIN D2  //--> SDA / SS is connected to pinout D2
#define RST_PIN D1 //--> RST is connected to pinout D1
MFRC522 mfrc522(SS_PIN, RST_PIN);  //--> Create MFRC522 instance.
#define Motor 10
const int trigPin = D3;
const int echoPin = D8;
#define DCwater_pump D0  
long duration;
int distance;
#define ON_Board_LED D4  //--> Defining an On Board LED, used for indicators when the process of connecting to a wifi router
//----------------------------------------SSID and Password of your WiFi router-------------------------------------------------------------------------------------------------------------//
const char* ssid = "POCO X3 Pro";
const char* password = "achu1234!";
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
ESP8266WebServer server(80);  //--> Server on port 80
int readsuccess;
byte readcard[4];
char str[32] = "";
String StrUID;
//-----------------------------------------------------------------------------------------------SETUP-------------------------------------------------------------------------------------//
void setup() {
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input
  pinMode(DCwater_pump, OUTPUT);
  Serial.begin(115200); //--> Initialize serial communications with the PC
  SPI.begin();      //--> Init SPI bus
  mfrc522.PCD_Init();//--> Init MFRC522 card
  pinMode(Motor,OUTPUT);
  delay(500);
  WiFi.begin(ssid, password); //
  Serial.println("");
  pinMode(ON_Board_LED, OUTPUT);
  digitalWrite(ON_Board_LED, HIGH); //--> Turn off Led On Board
  //----------------------------------------Wait for connection
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    //----------------------------------------Make the On Board Flashing LED on the process of connecting to the wifi router.
    digitalWrite(ON_Board_LED, LOW);
    delay(250);
    digitalWrite(ON_Board_LED, HIGH);
    delay(250);
  }
  digitalWrite(ON_Board_LED, HIGH); //--> Turn off the On Board LED when it is connected to the wifi router.
  //----------------------------------------If successfully connected to the wifi router, the IP Address that will be visited is displayed in the serial monitor
  Serial.println("");
  Serial.print("Successfully connected to : ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println("Please tag a card or keychain to see the UID !");
  Serial.println("");
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//-----------------------------------------------------------------------------------------------LOOP---------------------------------------------------------------------------------------//
void loop() {
  // put your main code here, to run repeatedly
  readsuccess = getid();
  if (readsuccess) {
    digitalWrite(ON_Board_LED, LOW);
    HTTPClient http;    //Declare object of class HTTPClient
    String UIDresultSend, postData;
    UIDresultSend = StrUID;
    //Post Data
    postData = "rfid=" + String(UIDresultSend);
      http.begin("http://192.168.189.192/rfidreader/check.php");  //Specify request destination
    http.addHeader("Content-Type", "application/x-www-form-urlencoded"); //Specify content-type header
    int httpCode = http.POST(postData);   //Send the request
    String payload = http.getString();    //Get the response payload
    Serial.println(UIDresultSend);
    Serial.print("HTTP:");
    Serial.println(httpCode);
    Serial.print("Payload:"); //Print HTTP return code
    Serial.println(payload);    //Print request response payload
    http.end();  //Close connection
    digitalWrite(Motor,LOW
     );
    if(payload=="true")
    {
      Serial.println("Motor ON");
      digitalWrite(Motor,LOW);
      delay(3000);
      digitalWrite(Motor,HIGH);
    }
    else{
         Serial.println("Motor OFF");
         digitalWrite(Motor,HIGH);
     
    }
   

  }
  digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    duration = pulseIn(echoPin, HIGH);
    //Distance calculation
    distance= duration*0.034/2;
    // Printinng the distance on the Serial Monitor
    Serial.print("Distance: ");
    Serial.println(distance);
   
 if (distance < 30)
{
 digitalWrite(DCwater_pump,HIGH);
 Serial.println("DC Pump is ON Now!!");
 delay(1000);
}
else
{
 digitalWrite(DCwater_pump,LOW);
 Serial.println("DC Pump is OFF Now!!");
 delay(5000);

}
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//

//----------------------------------------Procedure for reading and obtaining a UID from a card or keychain---------------------------------------------------------------------------------//
int getid() {
  if (!mfrc522.PICC_IsNewCardPresent()) {
    return 0;
  }
  if (!mfrc522.PICC_ReadCardSerial()) {
    return 0;
  }


  Serial.print("THE UID OF THE SCANNED CARD IS : ");

  for (int i = 0; i < 4; i++) {
    readcard[i] = mfrc522.uid.uidByte[i]; //storing the UID of the tag in readcard
    array_to_string(readcard, 4, str);
    StrUID = str;
  }
  mfrc522.PICC_HaltA();
  return 1;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//

//----------------------------------------Procedure to change the result of reading an array UID into a string------------------------------------------------------------------------------//
void array_to_string(byte array[], unsigned int len, char buffer[]) {
  for (unsigned int i = 0; i < len; i++)
  {
    byte nib1 = (array[i] >> 4) & 0x0F;
    byte nib2 = (array[i] >> 0) & 0x0F;
    buffer[i * 2 + 0] = nib1  < 0xA ? '0' + nib1  : 'A' + nib1  - 0xA;
    buffer[i * 2 + 1] = nib2  < 0xA ? '0' + nib2  : 'A' + nib2  - 0xA;
  }
  buffer[len * 2] = '\0';
}
