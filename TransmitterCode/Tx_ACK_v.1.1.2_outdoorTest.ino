//MASTER - TRANMITTER - PI

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#define CE_PIN 7
#define CSN_PIN 8
//Vars for Radio Comms
RF24 radio(CE_PIN, CSN_PIN); //Create the radio
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

//=====================>>
void setup() {
    Serial.begin(9600);
    Serial.println("Transmission program beginning...");

    radio.begin(); //Begin radio
    radio.setDataRate( RF24_250KBPS ); //Set our data rate

    radio.enableAckPayload(); //Enable acknowledgment of payloads
    radio.setPALevel(RF24_PA_HIGH);
    radio.setRetries(3,5); // delay, count , sets retries
    radio.openWritingPipe(slaveAddress); //Open our writing pipe, must match receiver reading pipe
    radio.printDetails();

}
//=====================>>
void loop() {
  currentMillis = millis(); //CurrentMillis is now the current millis
  recvWithEndMarker();
  if(commandReceived){ //If it has been the amount of time equal to txIntervalMillis
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

  rslt = radio.write(&receivedChars, sizeof(receivedChars)); //Rslt is result of write() [true or false]

  Serial.print("Data Sent ---> ");
  Serial.print(receivedChars);
  if(rslt){ //If the transmission was successful
    if(radio.isAckPayloadAvailable()){ //If ACK data is available
      radio.read(&ackData, sizeof(ackData)); //Read the payload
      newData = true; //We have new data
    }
   
    else{
      Serial.println(" Ackknowledge but no data "); //Message sent but no acknowledgment
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

