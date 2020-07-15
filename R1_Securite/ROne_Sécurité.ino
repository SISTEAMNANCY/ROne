

#include <SoftwareSerial.h>

#include <DFPlayer_Mini_Mp3.h>

SoftwareSerial DFPlayerSerial( 10, 11 ); // RX, TX

int ValidTempo=9;
int DebPat=0;
int Oxy=1;
int Reglage=6;
int Tempo=5;
int AlimOK=4;
int LedDebPat=3;
int LedOxy=2;
int LedAlim=13;
int AlarmPat=0;
int AlarmOxy=0;
int AlarmAlim=0;
int BipReglage=0;
int BoutonTempo=0;

void setup()

{

  Serial.begin( 115200 );
  Serial.print( "\n\nDFPlayer_Mini_Mp3\n" );
  DFPlayerSerial.begin( 9600 );
  mp3_set_serial( DFPlayerSerial );
  mp3_set_device( 2 );  // Carte SD
  mp3_set_volume( 22 ); // 0-30  
  pinMode(ValidTempo,OUTPUT);
  pinMode(Reglage,INPUT_PULLUP);
  pinMode(Tempo,INPUT_PULLUP);
  pinMode(AlimOK,INPUT_PULLUP);
  pinMode(LedDebPat,OUTPUT);
  pinMode(LedOxy,OUTPUT);
  pinMode(LedAlim,OUTPUT);
  digitalWrite(LedDebPat,LOW);
  digitalWrite(LedOxy,LOW);
  digitalWrite(LedAlim,LOW);

     
  AlarmAlim = digitalRead (AlimOK);
  if (AlarmAlim ==0)
  {
     while (AlarmAlim ==0)    
    { 
      digitalWrite(LedAlim,HIGH);
      mp3_play( 3 );
     _delay_ms( 5000 );
     AlarmAlim = digitalRead (AlimOK);
    }
  }
  else
  {
  digitalWrite(LedAlim,LOW);
  mp3_play( 7 );  
  _delay_ms( 3000 );
  mp3_play( 11 ); 
  _delay_ms( 8000 ); 
  mp3_play( 7 );  
  _delay_ms( 3000 );
  digitalWrite (ValidTempo, HIGH);
  delay(3000);
  digitalWrite (ValidTempo, LOW);
  }
  
Serial.begin(9600);
}

void loop()

{ 
    
  AlarmPat = (analogRead (A0) *5)/1023;
  Serial.println(AlarmPat);
    if (AlarmPat >= 3)
  {
  digitalWrite(LedDebPat,HIGH);
  mp3_play( 1 );
  _delay_ms( 4000 );
  }
  else
  {
   digitalWrite(LedDebPat,LOW);  
  }

 AlarmOxy = (analogRead (A1)*5)/1023;
 Serial.println(AlarmOxy);
 Serial.println();
    if (AlarmOxy >=3)
  {
  digitalWrite(LedOxy,HIGH);
  mp3_play( 2 );
  _delay_ms( 4000 );
  }
  else
  {
    digitalWrite(LedOxy,LOW);  
  }
  
 BipReglage = digitalRead (Reglage);
    if (BipReglage ==1)
  {
    mp3_play( 10 );
   _delay_ms( 4000 );
  }
  
}
