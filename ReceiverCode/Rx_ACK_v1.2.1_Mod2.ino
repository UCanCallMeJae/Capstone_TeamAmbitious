//SLAVE - RECEIVER
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

int RelayPin = 2;
int RelayState = 0;
bool RELAY = false;


//======================>


void setup() {// put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(4, OUTPUT);
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input
  pinMode(trigPin1, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin1, INPUT); // Sets the echoPin as an Input

  pinMode(RelayPin, OUTPUT);
  
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
    if(strcmp(dataReceived, "2LED") == 0){
      changeLEDState();
      ackData[0] = '0';
      ackData[1] = checkLEDState();
    radio.writeAckPayload(1, &ackData, sizeof(ackData)); // load the payload for the next time it receives something
    ackData[0] = '0';
    ackData[1] = '0';
    }

    if(strcmp(dataReceived, "RelayState") == 0){
      ackData[0] = '0';
      ackData[1] = checkRelayState();
    radio.writeAckPayload(1, &ackData, sizeof(ackData)); // load the payload for the next time it receives something
    ackData[0] = '0';
    ackData[1] = '0';
    }
    if(strcmp(dataReceived, "RELAY") == 0){
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
