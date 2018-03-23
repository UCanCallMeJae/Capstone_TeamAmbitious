//MASTER - TRANMITTER - PI

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <AES.h>
#include <printf.h>

#define CE_PIN 7
#define CSN_PIN 8
//Vars for Radio Comms
RF24 radio(CE_PIN, CSN_PIN); //Create the radio
AES aes ; //Create AES instance

const byte numChars = 10;
bool newData = false;
char dataToSend[numChars] = "Message 0"; //The data we want to send
char txNum = '0';
const byte slaveAddress[5] = {'R','x','A','A','A'}; //Our address, matching the one with slave
int ackData[2] = {-1, -1}; // To hold the values coming from slave
String command = "";

unsigned long currentMillis;
unsigned long prevMillis;
unsigned long txIntervalMillis = 1000; //Send once per second

//Vars for receiving / processing user input

char receivedChars[numChars];   // an array to store the received data
char convertedChar = ' ';

boolean newCharData = false;
boolean commandReceived = false;

//Vars for LED control

//AES Vars
unsigned int keyLength [3] = {128, 192, 256}; // key length: 128b, 192b or 256b
byte *key = (unsigned char*)"01234567890123456789012345678901"; // encryption key (32B / 256b*, 24B / 192b, 16B / 128b)
char plain[numChars] = "LED\n"; // plaintext to encrypt

unsigned long long int myIv = 36753562; // CBC initialization vector; real iv = iv x2 ex: 01234567 = 0123456701234567

byte iv [N_BLOCK] ; //Our iv size of N_BLOCK ?
const int plainPaddedLength = sizeof(plain) + (N_BLOCK - ((sizeof(plain) - 1) % 16)); // length of padded plaintext [B]
byte cipher [plainPaddedLength]; // var to store ciphertext (encrypted plaintext)
byte check [plainPaddedLength]; // var to store our decrypted plaintext

//=====================>>
void setup() {
    Serial.begin(9600);
    printf_begin(); //Needed for AES
    Serial.println("Transmission program beginning...");

    radio.begin(); //Begin radio
    radio.setDataRate( RF24_250KBPS ); //Set our data rate

    radio.enableAckPayload(); //Enable acknowledgment of payloads

    radio.setRetries(3,5); // delay, count , sets retries
    radio.openWritingPipe(slaveAddress); //Open our writing pipe, must match receiver reading pipe

}
//=====================>>
void loop() {
  currentMillis = millis(); //CurrentMillis is now the current millis
  recvWithEndMarker();
  if(commandReceived){ //If it has been the amount of time equal to txIntervalMillis
    memset(plain, 0, sizeof(plain)); //Clear our plaintext
    for (int i = 0; i < sizeof(receivedChars); i++) {
      plain[i] = receivedChars[i];
    }
    send(); //Send function
    processData();
    commandReceived = false;
  }
  showData(); //Show it
  showNewData(); //Set newData for commands to false
  

}

//=====================>>

void send(){
  bool rslt; //Boolean for radio transmission
  aesFunc(keyLength[0], true);
  rslt = radio.write(&receivedChars, sizeof(receivedChars)); //Rslt is result of write() [true or false]
  aesFunc(keyLength[0], false);
  Serial.print("Data Sent ---> ");
  Serial.print(receivedChars);
  if(rslt){ //If the transmission was successful
    if(radio.isAckPayloadAvailable()){ //If ACK data is available
      radio.read(&ackData, sizeof(ackData)); //Read the payload
      newData = true; //We have new data
    }
   
    else{
      Serial.println(" Acknowledge but no data "); //Message sent but no acknowledgment
    }
   updateMessage(); 
  }
 
  else{ //Our transmission failed
    Serial.println("Tx failed!");
  }
  prevMillis = millis(); //Store the last millis we sent a message
}

//===========================>>

void showData(){ //Show the acknowledge data
  if(newData == true){ //If we have new data
    convertedChar = ackData[0];
    Serial.print(" Acknowledge data ");
    Serial.print(convertedChar);
    Serial.print(", ");
    Serial.println(ackData[1]);
    Serial.println();
    newData = false; //No longer new
  }
}
//===========================>>

void updateMessage(){
  //so you can see new data is being sent

  txNum += 1; //Add one to TxNum
  if(txNum > '9'){ //If its more than 9
    txNum = '0'; //Set to 0
  }
  dataToSend[8] = txNum; //Put the new TxNum in index 8 of dataToSend
}
//==============================>>
void recvWithEndMarker() {
    static byte ndx = 0;
    char endMarker = '\n';
    char rc;
   
    while (Serial.available() > 0 && newCharData == false) {
        rc = Serial.read();

        if (rc != endMarker) {
            receivedChars[ndx] = rc;
            ndx++;
            if (ndx >= numChars) {
                ndx = numChars - 1;
            }
        }
        else {
            receivedChars[ndx] = '\0'; // terminate the string
            ndx = 0;
            newCharData = true;
            commandReceived = true;
        }
    }
}

//==========================>>
void showNewData() {
    if (newCharData == true) {
        newCharData = false;
    }
}
void processData(){
  if(strcmp((receivedChars), "LEDSTATE") == 0){
    Serial.print("LED STATE IS ");
    Serial.println(ackData[1]);
    
  }
  
}

// =========================>> 

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

