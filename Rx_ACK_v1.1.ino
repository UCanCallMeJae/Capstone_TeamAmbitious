//SLAVE - RECEIVER
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#define CE_PIN 7 //Define the pins
#define CSN_PIN 8

const byte address[5] = {'R', 'x', 'A', 'A', 'A'}; //The address in the pipe

RF24 radio(CE_PIN, CSN_PIN); //Create radio

char dataReceived[10]; //MUST MATCH Tx
int ackData[2] = {109, -4000}; //ACK data we send to master, to confirm received data
bool newData = false; //Boolean used for obvious reasons
int moistureLevel = 438;
int trashLevel = 75;
int waterOn = 0;
//======================>


void setup() {// put your setup code here, to run once:
  Serial.begin(9600);

  Serial.println("Program beginning...");
  radio.begin(); //Begin radio setup
  radio.setDataRate( RF24_250KBPS ); //Setting our data rate, lower means farther
  radio.openReadingPipe(1, address); //Open the reading pipe to our Tx

  radio.enableAckPayload(); //Enable ackknowledgment of payloads
  radio.writeAckPayload(1, &ackData, sizeof(ackData)); //pre-load our acknowledgment data, when message is received on pipe, ack data will send what is in buffer 
                                                      //(pipe, buf, size)
  
  radio.startListening(); //Start receiver mode                                                   
  

}
//=================================>>

void loop() {// put your main code here, to run repeatedly:
  getData();
  showData();  

}
//=================================>>

void getData() { //Function to grab new data off pipe
  if(radio.available()){ //If there is data in the pipe address
    radio.read(&dataReceived, sizeof(dataReceived)); //Read it
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
    
    if(strcmp(dataReceived, "WaterLvl") == 0){
      ackData[0] = '0';
      ackData[1] = moistureLevel;
    radio.writeAckPayload(1, &ackData, sizeof(ackData)); // load the payload for the next time it receives something
   /** ackData[0] = '0';
    ackData[1] = '0'; **/
    }
    if(strcmp(dataReceived, "Water") == 0){
      waterOn ++;
      ackData[0] = '0';
      ackData[1] = waterOn;
    radio.writeAckPayload(1, &ackData, sizeof(ackData)); // load the payload for the next time it receives something
   /** ackData[0] = '0';
    ackData[1] = '0'; **/
    }
    if(strcmp(dataReceived, "Reset") == 0){
     waterOn = 0;
    }
    if(strcmp(dataReceived, "TrashLvl") == 0){
      ackData[0] = '0';
      ackData[1] = trashLevel;
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
