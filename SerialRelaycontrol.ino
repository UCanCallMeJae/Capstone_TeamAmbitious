 //Serial Relay Control
 //Tynan Peter
 //Version 1
 //21-MAR-2018
 
 
 void setup() 
{
  pinMode(2, OUTPUT);
  Serial.begin(9600);
  while (!Serial);
  Serial.println("Input 1 to Turn Relay on and 2 to off");
}

void loop()
{
  if (Serial.available())
  {
    int state = Serial.parseInt();
    if (state == 1)
    {
     digitalWrite(2, HIGH);
     Serial.println("Command completed Relay turned ON");
     delay(5000);
     digitalWrite(2,LOW);
     Serial.println("Relay Turned OFF by timer 5 Second");
    }
    if (state == 2)
    {
     digitalWrite(2, HIGH);
     Serial.println("Command completed Relay turned ON 10 Second Timer");
     delay(10000);
     digitalWrite(2,LOW);
     Serial.println("Relay turned OFF by timer 10 Seconds");
    }  
  }
}
