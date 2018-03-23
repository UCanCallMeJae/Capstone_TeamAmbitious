//SLAVE - RECEIVER
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <AES.h>
#include <printf.h>

#define CE_PIN 7 //Define the pins
#define CSN_PIN 8

const byte address[5] = {'R', 'x', 'A', 'A', 'A'}; //The address in the pipe
const byte numChars = 10;
const byte cipherSize = 17; //?

RF24 radio(CE_PIN, CSN_PIN); //Create radio
AES aes ; //Create AES instance

char encryptedData[cipherSize];
char dataReceived[10]; //MUST MATCH Tx
int ackData[2] = {109, -4000}; //ACK data we send to master, to confirm received data
bool newData = false; //Boolean used for obvious reasons
int moistureLevel = 438;
int trashLevel = 75;
int waterOn = 0;
int pinLED = 4;

// Vars for LED control 0 is off 1 is on
int LEDState = 0;
bool LED = false;

//===================>HC SR04 - MODULE Vars
const int trigPin = 3;
const int echoPin = 5;
long duration;
int distance;

const int trigPin1 = 6;
const int echoPin1 = 9;
long duration1;
long distance1;
//======================>AES Vars
unsigned int keyLength [3] = {128, 192, 256}; // key length: 128b, 192b or 256b
byte *key = (unsigned char*)"01234567890123456789012345678901"; // encryption key (32B / 256b*, 24B / 192b, 16B / 128b)
char plain[numChars] = "LED\n"; // plaintext to encrypt

unsigned long long int myIv = 36753562; // CBC initialization vector; real iv = iv x2 ex: 01234567 = 0123456701234567

byte iv [N_BLOCK] ; //Our iv size of N_BLOCK ?
const int plainPaddedLength = sizeof(plain) + (N_BLOCK - ((sizeof(plain) - 1) % 16)); // length of padded plaintext [B]
byte cipher [plainPaddedLength]; // var to store ciphertext (encrypted plaintext)
byte check [plainPaddedLength]; // var to store our decrypted plaintext

//======================>

void setup() {// put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(4, OUTPUT);
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input
  pinMode(trigPin1, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin1, INPUT); // Sets the echoPin as an Input
  Serial.println("Pin for LED powered!");
  Serial.println("Radio program beginning...");
  radio.begin(); //Begin radio setup
  radio.setDataRate( RF24_250KBPS ); //Setting our data rate, lower means farther
  radio.setPALevel(RF24_PA_HIGH);
  radio.openReadingPipe(1, address); //Open the reading pipe to our Tx
  radio.enableAckPayload(); //Enable ackknowledgment of payloads
  radio.writeAckPayload(1, &ackData, sizeof(ackData)); //pre-load our acknowledgment data, when message is received on pipe, ack data will send what is in buffer 
                                                      //(pipe, buf, size)
  
  radio.startListening(); //Start receiver mode  
  startupSeq();                                                 
  Serial.println("Listening...");

}
//=================================>>

void loop() {// put your main code here, to run repeatedly:
  getData();
  showData();  

}
//=================================>>

void getData() { //Function to grab new data off pipe
  if(radio.available()){ //If there is data in the pipe address
    radio.read(&cipher, sizeof(cipher)); //Read it and put it in dataReceived
    aesFunc(keyLength[0], false);
    processData(); //Update our ACK data in this function
    newData = true; //Set the newData flag to true
  }
}
//=================================>>
void showData() { //Show data
  if(newData == true){  //If there is new data, print it out
        Serial.print("Data received ");
        Serial.println(dataReceived);
        Serial.print(" ackPayload sent "); //Print our ackPayload
        Serial.print(ackData[0]); //Send data in ackData index 0
        Serial.print(", ");
        Serial.println(ackData[1]); //Same for index 1
        newData = false; //newData is now false
  }
}
//=================================>>
void processData() { //Update our acknowledgment data
    
    if(strcmp(dataReceived, "waterLvl") == 0){ // If the received command is WaterLvl
      ackData[0] = '0'; //Assign this value to ackData index 0
      ackData[1] = moistureLevel; //Include the moisture level
    radio.writeAckPayload(1, &ackData, sizeof(ackData)); // load the payload for the next time it receives something
   /** ackData[0] = '0';
    ackData[1] = '0'; **/
    }
    if(strcmp(dataReceived, "water") == 0){
      waterOn ++;
      ackData[0] = '0';
      ackData[1] = waterOn;
    radio.writeAckPayload(1, &ackData, sizeof(ackData)); // load the payload for the next time it receives something
   /** ackData[0] = '0';
    ackData[1] = '0'; **/
    }
    if(strcmp(dataReceived, "reset") == 0){
     waterOn = 0;
    }
    if(strcmp(dataReceived, "trashLvl") == 0){
      
    /**radio.writeAckPayload(1, &ackData, sizeof(ackData)); // load the payload for the next time it receives something
    ackData[0] = '0';
    ackData[1] = '0'; **/
    storeTrashLvl(); /**Run our trash level func**/
    Serial.println("Trash Lvl stored");
    ackData[0] = '0';
    ackData[1] = trashLevel;
    }
    if(strcmp(dataReceived, "gTrashLvl") == 0){
    ackData[0] = '0';
    ackData[1] = trashLevel; 
    radio.writeAckPayload(1, &ackData, sizeof(ackData)); // load the payload for the next time it receives something
    ackData[0] = '0';
    ackData[1] = '0'; 
    Serial.println("Trash Lvl stored");
    
    }
    if(strcmp(dataReceived, "LEDSTATE") == 0){
      ackData[0] = '0';
      ackData[1] = checkLEDState();
    radio.writeAckPayload(1, &ackData, sizeof(ackData)); // load the payload for the next time it receives something
    ackData[0] = '0';
    ackData[1] = '0';
    }
    if(strcmp(dataReceived, "LED") == 0){
      changeLEDState();
      ackData[0] = '0';
      ackData[1] = checkLEDState();
    radio.writeAckPayload(1, &ackData, sizeof(ackData)); // load the payload for the next time it receives something
    ackData[0] = '0';
    ackData[1] = '0';
    }
    else{
     /** ackData[0] -= 1; //Index 0 is now value - 1
      ackData[1] -= 1;// Same for Index 1
      if (ackData[0] < 100) { //Reset data if its below 100
          ackData[0] = 109;
      }
      if (ackData[1] < -4009) { //Same below 4000
          ackData[1] = -4000;
      } 
    radio.writeAckPayload(1, &ackData, sizeof(ackData)); // load the payload for the next time it receives something **/
    Serial.println("Received but did nothing");
    }
}
//=================================>>
int checkLEDState(){
  return LEDState;
}
//=============================>>
void changeLEDState(){
  if(!LED){
    LEDState = 1;
    LED = true;
    digitalWrite(pinLED, HIGH);
  }
  else{
    LEDState = 0;
    LED = false;
    digitalWrite(pinLED, LOW);
  }
}
//============================>>
void storeTrashLvl(){
 
    activateSensor(trigPin, echoPin, 0);
    delay(5);
    activateSensor(trigPin1, echoPin1, 1);
    
    
    trashLevel = (distance + distance1)/2;
}
//========================>>
void startupSeq(){
  changeLEDState();
  delay(350);
  changeLEDState();
  delay(350);
  changeLEDState();
  delay(350);
  changeLEDState();
  delay(350);
  
}
// ===========================>>
void activateSensor(const int triPin, const int echPin, int sensorID){
   // Clears the trigPin
    digitalWrite(triPin, LOW);
    delayMicroseconds(2);
    // Sets the trigPin on HIGH state for 10 micro seconds
    digitalWrite(triPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(triPin, LOW);
    // Reads the echoPin, returns the sound wave travel time in microseconds
    
    if(sensorID == 0){
      // Calculating the distance
      duration = pulseIn(echPin, HIGH);
      distance= duration*0.034/2;
      // Prints the distance on the Serial Monitor
      delay(100); //So we get ONE value
      Serial.println(distance);
    }
    if(sensorID == 1){
      // Calculating the distance
      duration1 = pulseIn(echPin, HIGH);
      distance1 = duration1 *0.034/2;
      // Prints the distance on the Serial Monitor
      delay(100); //So we get ONE value
      Serial.println(distance1);
    }
}
//============================>>
void aesFunc (int bits, bool isEncrypt)
{
  aes.iv_inc(); //Increment the IV to randomize
  
  

  //-----------------------ENCRYPT----------------------------->>
  if (isEncrypt) {
    aes.set_IV(myIv); //Update IV, this randomizes output each time
    aes.get_IV(iv);

    Serial.print("Encrypting plaintext:   ");
    aes.printArray(plain, (bool)true); //print plain with no padding
    aes.do_aes_encrypt(plain, sizeof(plain), cipher, key, bits, iv); //Encrypt function?
    Serial.println("Done!");
    Serial.print("Cipher text:  ");
    aes.printArray(cipher, (bool)false); //print cipher with padding

    Serial.print("Cipher text size:  ");
    Serial.println(sizeof(cipher));

  }

  //-----------------------DECRYPT----------------------------->>
  if (!isEncrypt) {
    aes.set_IV(myIv); //Update IV
    aes.get_IV(iv);

    Serial.print("Decrypting..."); //Start timer for decryption
    aes.do_aes_decrypt(cipher, aes.get_size(), check, key, bits, iv);
    Serial.println("Done!");
    Serial.print("Decrypted cipher text: ");
    aes.printArray(check, (bool)true); //print decrypted plain with no padding

  }


}
