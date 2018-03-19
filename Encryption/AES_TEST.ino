#include <AES.h>
#include <AES_config.h>
#include <printf.h>
AES aes ; //Create AES instance0
 
unsigned int keyLength [3] = {128, 192, 256}; // key length: 128b, 192b or 256b
 
byte *key = (unsigned char*)"01234567890123456789012345678901"; // encryption key (32B / 256b*, 24B / 192b, 16B / 128b)
byte plain[] = "Sensor data from modules"; // plaintext to encrypt
 
unsigned long long int myIv = 36753562; // CBC initialization vector; real iv = iv x2 ex: 01234567 = 0123456701234567

//=================================>>
 
void setup ()
{
  Serial.begin (9600) ;
  printf_begin();
}
 
void loop () 
{
  for (int i=0; i < 3; i++)
  {
    Serial.print("- key length [b]: ");
    Serial.println(keyLength [i]);
    aesTest (keyLength[i]);
    delay(2000);
  }
}
 
void aesTest (int bits)
{
  aes.iv_inc(); //Increment the IV to randomize
  
  byte iv [N_BLOCK] ; //Our iv size of N_BLOCK ? 
  int plainPaddedLength = sizeof(plain) + (N_BLOCK - ((sizeof(plain)-1) % 16)); // length of padded plaintext [B]
  byte cipher [plainPaddedLength]; // var to store ciphertext (encrypted plaintext)
  byte check [plainPaddedLength]; // var to store our decrypted plaintext
  
   //-----------------------ENCRYPT----------------------------->>
  aes.set_IV(myIv); //Update IV, this randomizes output each time
  aes.get_IV(iv);
 
  Serial.print("- encryption time [us]: "); // Start timer for encryption
  unsigned long ms = micros ();
  aes.do_aes_encrypt(plain,sizeof(plain),cipher,key,bits,iv); //Encrypt function?
  Serial.println(micros() - ms);
  
  //-----------------------DECRYPT----------------------------->>
  aes.set_IV(myIv); //Update IV
  aes.get_IV(iv);
 
  Serial.print("- decryption time [us]: "); //Start timer for decryption
  ms = micros ();
  aes.do_aes_decrypt(cipher,aes.get_size(),check,key,bits,iv); 
  Serial.println(micros() - ms);
  //-----------------------RESULTS----------------------------->>
  Serial.print("- plain:   ");
  aes.printArray(plain,(bool)true); //print plain with no padding
 
  Serial.print("- cipher:  ");
  aes.printArray(cipher,(bool)false); //print cipher with padding
 
  Serial.print("- check:   ");
  aes.printArray(check,(bool)true); //print decrypted plain with no padding
  
  Serial.print("- iv:      ");
  aes.printArray(iv,16); //print iv
  printf("\n===================================================================================\n");
}
