//SLAVE - RECEIVER - Water Module
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#define CE_PIN 7 //Define the pins
#define CSN_PIN 8

const byte address[5] = {'R', 'x', 'A', 'A', 'B'}; //The address in the pipe

RF24 radio(CE_PIN, CSN_PIN); //Create radio

char dataReceived[10]; //MUST MATCH Tx
int ackData[2] = {109, -4000}; //ACK data we send to master, to confirm received data
bool newData = false; //Boolean used for obvious reasons
int moistureLevel = 438;
int waterOn = 0;
int pinLED = 4;

// Vars for LED control 0 is off 1 is on
int LEDState = 0;
bool LED = false;

//===================>Relay - Vars

int RelayPin = 2;
int RelayState = 0;
bool RELAY = false;


//======================> Moisture Sensors Vars
int sensor_pin = A0;
int output_value;
int output_value_mapped;
int PWR = 5;
//======================>

void setup() {// put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(4, OUTPUT);
  pinMode(RelayPin, OUTPUT);
  pinMode(PWR, OUTPUT);
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
    
    if(strcmp(dataReceived, "2waterLvl") == 0){ // Store moisture value
      getMoistureValue();
      ackData[0] = '0'; //Assign this value to ackData index 0
      ackData[1] = output_value_mapped; //Include the moisture level
      Serial.println("Moisture value stored");
   // radio.writeAckPayload(1, &ackData, sizeof(ackData)); // load the payload for the next time it receives something
   /** ackData[0] = '0';
    ackData[1] = '0'; **/
    }
    if(strcmp(dataReceived, "2water") == 0){ //Send moisture value back
      ackData[0] = '0';
      ackData[1] = output_value_mapped;
    radio.writeAckPayload(1, &ackData, sizeof(ackData)); // load the payload for the next time it receives something
    ackData[0] = '0';
    ackData[1] = '0';
    }
    if(strcmp(dataReceived, "2reset") == 0){
     waterOn = 0;
    }
    if(strcmp(dataReceived, "2LED") == 0){
      changeLEDState();
      ackData[0] = '0';
      ackData[1] = checkLEDState();
    radio.writeAckPayload(1, &ackData, sizeof(ackData)); // load the payload for the next time it receives something
    ackData[0] = '0';
    ackData[1] = '0';
    }

    if(strcmp(dataReceived, "2RelayState") == 0){
      ackData[0] = '0';
      ackData[1] = checkRelayState();
    radio.writeAckPayload(1, &ackData, sizeof(ackData)); // load the payload for the next time it receives something
    ackData[0] = '0';
    ackData[1] = '0';
    }
    if(strcmp(dataReceived, "2RELAY") == 0){
      changeRelayState();
      ackData[0] = '0';
      ackData[1] = checkRelayState();
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
int checkRelayState(){
  return RelayState;
}
//=============================>>
void changeRelayState(){
  if(!RELAY){
    RelayState = 1;
    RELAY = true;
    digitalWrite(RelayPin, HIGH);
  }
  else{
    RelayState = 0;
    RELAY = false;
    digitalWrite(RelayPin, LOW);
  }
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
void getMoistureValue(){
  digitalWrite(PWR, HIGH);
  delay(500);
  output_value= analogRead(sensor_pin);
  output_value_mapped = map(output_value,1005,335,0,100); //1005 IS WET, 335 IS DRY CHECK SOILCALIBRATION.INO
  moistureLevel = output_value_mapped;
  Serial.print("Mositure: ");
  Serial.print(moistureLevel);
  Serial.println("%");
  delay(500);
  digitalWrite(PWR,LOW);
  delay(500);
}

