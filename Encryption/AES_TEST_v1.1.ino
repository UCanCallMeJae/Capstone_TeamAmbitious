#include <AES.h>
#include <printf.h>
AES aes ; //Create AES instance0
 
unsigned int keyLength [3] = {128, 192, 256}; // key length: 128b, 192b or 256b
 const int numChars = 10;
byte *key = (unsigned char*)"01234567890123456789012345678901"; // encryption key (32B / 256b*, 24B / 192b, 16B / 128b)
char plain[numChars] = "LED\n"; // plaintext to encrypt
 
unsigned long long int myIv = 36753562; // CBC initialization vector; real iv = iv x2 ex: 01234567 = 0123456701234567

bool newCharData;
bool commandReceived;

char receivedChars[numChars];

//=================================>>
 
void setup ()
{
  Serial.begin (9600) ;
  printf_begin();
}
 
void loop () 
{
  recvWithEndMarker();
  memset(plain, 0, sizeof(plain));

  for(int i = 0; i < sizeof(receivedChars); i++){
    plain[i] = receivedChars[i];
  }

  if(strcmp(receivedChars, "encrypt") == 0 && commandReceived == true){
    commandReceived = false;
    aesTest(keyLength[1], true);    
    Serial.println("Waiting....");
    newCharData = false;
  }
  if(strcmp(receivedChars, "decrypt") == 0 && commandReceived == true){
    aesTest(keyLength[1], false);
    Serial.println("Waiting....");
    newCharData = false;
    commandReceived = false;
  }
  else{
    newCharData = false;
  }
}
 
void aesTest (int bits, bool isEncrypt)
{
  aes.iv_inc(); //Increment the IV to randomize
  
  byte iv [N_BLOCK] ; //Our iv size of N_BLOCK ? 
  int plainPaddedLength = sizeof(plain) + (N_BLOCK - ((sizeof(plain)-1) % 16)); // length of padded plaintext [B]
  byte cipher [plainPaddedLength]; // var to store ciphertext (encrypted plaintext)
  byte check [plainPaddedLength]; // var to store our decrypted plaintext
  
   //-----------------------ENCRYPT----------------------------->>
   if(isEncrypt){
    aes.set_IV(myIv); //Update IV, this randomizes output each time
    aes.get_IV(iv);
   
    Serial.print("Encrypting plaintext:   ");
    aes.printArray(plain,(bool)true); //print plain with no padding
    aes.do_aes_encrypt(plain,sizeof(plain),cipher,key,bits,iv); //Encrypt function?
    Serial.println("Done!");
    Serial.print("Cipher text:  ");
    aes.printArray(cipher,(bool)false); //print cipher with padding
 
    Serial.print("Cipher text size:  ");
    Serial.println(sizeof(cipher));
   }
  
  //-----------------------DECRYPT----------------------------->>
  if(!isEncrypt){
    aes.set_IV(myIv); //Update IV
    aes.get_IV(iv);
   
    Serial.print("Decrypting..."); //Start timer for decryption
    aes.do_aes_decrypt(cipher,aes.get_size(),check,key,bits,iv); 
    Serial.println("Done!");
    Serial.print("Decrypted cipher text: ");
    aes.printArray(check,(bool)true); //print decrypted plain with no padding

  }
  
  //-----------------------RESULTS----------------------------->>
  
  Serial.print("- iv:      ");
  aes.printArray(iv,16); //print iv
  printf("\n===================================================================================\n");

  byte returnCipher(){
    return cipher;
  }
}

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
            Serial.print("Size of receivedChars: ");
            Serial.println(sizeof(receivedChars));
            ndx = 0;
            newCharData = true;
            commandReceived = true;
            Serial.flush();
        }
    }
}
