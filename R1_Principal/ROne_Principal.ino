
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <math.h>

LiquidCrystal_I2C lcd(0x27, 20, 4);
//LiquidCrystal_I2C lcd(0x3F,20,4);

int Pompe = 10; // Relais de commande de la pompe sur sortie D10 (HIGH = Pompe arrêtée)
int EVanE = 11; // Relais de commande de l'électrovanne de vidaznge des brassards sur sortie D11 ((HIGH = Vannes ouverte)
int EVanI = 9;// Relais de commande de l'électrovanne de gonflage des brassards sur sortie D9 ((HIGH = Vannes ouverte)
int AlimSec = 8;// Relais d'alimentation de la nano de sécurité
int Bip=7;//Commande de bip de confirmation de réglage
int AlimOK = 6;// Surveillance alim Uno
int Fr=0;// Valeur de fréquence respiratoire
int FrMem=0;// Valeur sauvegardée de fréquence respiratoire
int IE=0;// Valeur X du rapport I:E = 1:X
int IEMem=0;//Valeur sauvegardée du rapport I:E
int Tot=0;// Valeur de durée de cycle
int CorTot=64; // Valeur moyenne d'erreur sur la période, mesure expérimentale
int I=0;// Valeur de durée d'inspiration
int IP=0;// Valeur de durée d'inspiration avec pompe
int I1=0;// Valeur de durée d'inspiration EV Brassard fermée
int I2=0;// Valeur de durée d'inspiration EV Brassard ouverte
int IP11=0;// Valeur de durée d'inspiration EV Brassard fermée avec pompe
int IP12=0;// Valeur de durée d'inspiration EV Brassard fermée sans pompe
int IP21=0;// Valeur de durée d'inspiration EV Brassard ouverte avec pompe
int IP22=0;// Valeur de durée d'inspiration EV Brassard ouverte sans pompe
int E=0;// Valeur de durée d'expiration
int E1=0;// Valeur de durée d'expiration
int E2=0;// Valeur de durée d'expiration + pompe
int PAI=0;// Valeur de durée de commande de la pompe avant inspiration
int EPI=0;//Valeur de durée de commande de EVanE pendant inspiration
int PPI=100;//Valeur de durée de commande de la pompe pendant inspiration
int SelectVt=0;
int SelectVtMem=0;
int FiO2=0;
int FiO2Mem=0;
float Vt=0;
float x1=0;
float x2=0;
float x3=0;
float x4=0;
float x5=0;
float x6=0;

void setup(){
 lcd.init(); // initialisation de l'afficheur
 lcd.backlight();// Rétroéclairage de l'afficheur
 pinMode(Pompe,OUTPUT);// initialisation de la sortie logique 10 pour le relais de la pompe
 pinMode(EVanE,OUTPUT);// initialisation de la sortie logique 11 pour le relais de l'électrovanne de vidange des brassards
 pinMode(EVanI,OUTPUT);// initialisation de la sortie logique 9 pour le relais de l'électrovanne de gonflage des brassards
 pinMode(AlimSec,OUTPUT);// initialisation de la sortie logique 8 pour le relais de l'alimentation de sécurité
 pinMode(Bip,OUTPUT);// initialisation de la sortie logique 7 pour le son de confirmation
 pinMode(AlimOK,OUTPUT);// initialisation de la sortie logique 7 pour le son de confirmation
 
 digitalWrite(Pompe,LOW);
 digitalWrite(EVanI,LOW);
 digitalWrite(EVanE,LOW);
 digitalWrite(AlimSec,LOW);
 digitalWrite(Bip,LOW);
 digitalWrite(AlimOK,HIGH);
 Serial.begin(9600);
}

void loop(){

   digitalWrite(Bip,LOW);

// 1. ACQUISITION DES PARAMETRES DE REGLAGE ET VIDANGE PREVENTIVE DES BRASSARDS EN CAS DE MODIFICATION

  // 1.1 VALEUR DE REGLAGE DE FREQUENCE
  
 Fr = map(analogRead(3),0,1023,12,18);// Lecture de la valeur de réglage de fréquence et mise à l'échelle

 if ( Fr == 18)
 {
  Fr=17;
 }
 
 if (Fr!=FrMem)// Si le ré&glage de fréquence a été modifié, vidanger les brassards pendant deux secondes
  {
    digitalWrite(EVanE,LOW);
    digitalWrite(Pompe,LOW);
    delay(3000);
    digitalWrite(EVanE,HIGH);
    digitalWrite(Bip,HIGH);
    FrMem =Fr;// Enregistrement de la valeur actuelle de régflage de fréquence
   } 

  // 1.2 VALEUR DE REGLAGE DU RAPPORT I:E
 IE = map(analogRead(1),0,1023,1,4);// Lecture de la valeur de réglage de rapport I:E et mise à l'échelle
  if ( IE == 4)
 {
  IE=3;
 } 

 
 if (IE!=IEMem)//  Si le ré&glage de rapport I:E a été modifié, vidanger les brassards pendant deux secondes
  {
    digitalWrite(EVanE,LOW);
    digitalWrite(Pompe,LOW);
    delay(3000);
    digitalWrite(EVanE,HIGH);
    digitalWrite(Bip,HIGH);
    IEMem =IE;// Enregistrement de la valeur actuelle de réglage de rapport I:E
   } 
   
      // 1.3 VALEUR DE REGLAGE DE VOLUME COURANT
 SelectVt = map(analogRead(2),0,1023,0,4);// Lecture de la valeur de réglage de temps de commande de pompe en avant inspiration et mise à l'échelle
   if ( SelectVt == 4)
 {
  SelectVt=3;
 } 
 

 if (SelectVt!=SelectVtMem)//  Si le réglage de Vt a été modifié, vidanger les brassards pendant deux secondes
  {
    digitalWrite(EVanE,LOW);
    digitalWrite(Pompe,LOW);
    delay(3000);
    digitalWrite(EVanE,HIGH);
    digitalWrite(Bip,HIGH);
    SelectVtMem =SelectVt;// Enregistrement de la valeur actuelle de réglage de rapport I:E
   } 
 Vt=0.4 + (SelectVt * 0.05);

    // 1.4 VALEUR DE REGLAGE DE LA FIO2
FiO2 = map(analogRead(0),0,1023,60,101);// Lecture de la valeur de réglage de temps de commande de pompe en avant inspiration et mise à l'échelle
   if ( FiO2 == 101)
 {
  FiO2=100;
 } 
 Serial.println(((1000* SelectVt) + FiO2),DEC);// Envoi de la valeur de 1000*VT +FIO2 vers la nano mélangeur

 if (FiO2!=FiO2Mem)//  Si le réglage de FiO2 a été modifié, 
  {
    
    FiO2Mem =FiO2;// Enregistrement de la valeur actuelle de réglage de FiO2
   } 



  // 1.5 PARAMETRAGE

  if (IE == 1)
  {
      if (Fr == 12)
      {
            if (SelectVt == 0)
            {
            PAI=0;
            EPI=40;
            } 
            if (SelectVt == 1)
            {
            PAI=0;
            EPI=40;
            }
            if (SelectVt == 2)
            {
            PAI=0;
            EPI=45;
            }
            if (SelectVt == 3)
            {
            PAI=0;
            EPI=70;
            }
        }  
        if (Fr == 13)
      {
            if (SelectVt == 0)
            {
            PAI=0;
            EPI=40;
            } 
            if (SelectVt == 1)
            {
            PAI=0;
            EPI=40;
            }
            if (SelectVt == 2)
            {
            PAI=0;
            EPI=70;
            }
            if (SelectVt == 3)
            {
            PAI=0;
            EPI=90;
            }
        } 
        if (Fr == 14)
      {
            if (SelectVt == 0)
            {
            PAI=0;
            EPI=50;
            } 
            if (SelectVt == 1)
            {
            PAI=0;
            EPI=65;
            }
            if (SelectVt == 2)
            {
            PAI=0;
            EPI=90;
            }
            if (SelectVt == 3)
            {
            PAI=0;
            EPI=100;
            }
        } 
        if (Fr == 15)
      {
            if (SelectVt == 0)
            {
            PAI=0;
            EPI=60;
            } 
            if (SelectVt == 1)
            {
            PAI=0;
            EPI=80;
            }
            if (SelectVt == 2)
            {
            PAI=0;
            EPI=95;
            }
            if (SelectVt == 3)
            {
            PAI=0;
            EPI=100;
            }
        } 
        if (Fr == 16)
      {
            if (SelectVt == 0)
            {
            PAI=0;
            EPI=70;
            } 
            if (SelectVt == 1)
            {
            PAI=0;
            EPI=92;
            }
            if (SelectVt == 2)
            {
            PAI=0;
            EPI=100;
            }
            if (SelectVt == 3)
            {
            PAI=50;
            EPI=100;
            }
        } 
        if (Fr == 17)
      {
            if (SelectVt == 0)
            {
            PAI=0;
            EPI=85;
            } 
            if (SelectVt == 1)
            {
            PAI=0;
            EPI=100;
            }
            if (SelectVt == 2)
            {
            PAI=50;
            EPI=100;
            }
            if (SelectVt == 3)
            {
            PAI=50;
            EPI=100;
            }
        } 
  }
  if (IE == 2)
  {
      if (Fr == 12)
      {
            if (SelectVt == 0)
            {
            PAI=0;
            EPI=80;
            } 
            if (SelectVt == 1)
            {
            PAI=0;
            EPI=80;
            }
            if (SelectVt == 2)
            {
            PAI=0;
            EPI=80;
            }
            if (SelectVt == 3)
            {
            PAI=0;
            EPI=100;
            }
        }  
        if (Fr == 13)
      {
            if (SelectVt == 0)
            {
            PAI=0;
            EPI=80;
            } 
            if (SelectVt == 1)
            {
            PAI=0;
            EPI=80;
            }
            if (SelectVt == 2)
            {
            PAI=0;
            EPI=100;
            }
            if (SelectVt == 3)
            {
            PAI=10;
            EPI=100;
            }
        } 
        if (Fr == 14)
      {
            if (SelectVt == 0)
            {
            PAI=0;
            EPI=90;
            } 
            if (SelectVt == 1)
            {
            PAI=0;
            EPI=100;
            }
            if (SelectVt == 2)
            {
            PAI=10;
            EPI=100;
            }
            if (SelectVt == 3)
            {
            PAI=30;
            EPI=100;
            }
        } 
        if (Fr == 15)
      {
            if (SelectVt == 0)
            {
            PAI=0;
            EPI=100;
            } 
            if (SelectVt == 1)
            {
            PAI=5;
            EPI=100;
            }
            if (SelectVt == 2)
            {
            PAI=20;
            EPI=100;
            }
            if (SelectVt == 3)
            {
            PAI=80;
            EPI=100;
            }
        } 
        if (Fr == 16)
      {
            if (SelectVt == 0)
            {
            PAI=5;
            EPI=100;
            } 
            if (SelectVt == 1)
            {
            PAI=15;
            EPI=100;
            }
            if (SelectVt == 2)
            {
            PAI=40;
            EPI=100;
            }
            if (SelectVt == 3)
            {
            PAI=80;
            EPI=100;
            }
        } 
        if (Fr == 17)
      {
            if (SelectVt == 0)
            {
            PAI=10;
            EPI=100;
            } 
            if (SelectVt == 1)
            {
            PAI=25;
            EPI=100;
            }
            if (SelectVt == 2)
            {
            PAI=100;
            EPI=100;
            }
            if (SelectVt == 3)
            {
            PAI=100;
            EPI=100;
            }
        } 
  }
if (IE == 3)
  {
      if (Fr == 12)
      {
            if (SelectVt == 0)
            {
            PAI=0;
            EPI=80;
            } 
            if (SelectVt == 1)
            {
            PAI=0;
            EPI=80;
            }
            if (SelectVt == 2)
            {
            PAI=0;
            EPI=80;
            }
            if (SelectVt == 3)
            {
            PAI=0;
            EPI=100;
            }
        }  
        if (Fr == 13)
      {
            if (SelectVt == 0)
            {
            PAI=0;
            EPI=80;
            } 
            if (SelectVt == 1)
            {
            PAI=0;
            EPI=80;
            }
            if (SelectVt == 2)
            {
            PAI=0;
            EPI=100;
            }
            if (SelectVt == 3)
            {
            PAI=10;
            EPI=100;
            }
        } 
        if (Fr == 14)
      {
            if (SelectVt == 0)
            {
            PAI=0;
            EPI=90;
            } 
            if (SelectVt == 1)
            {
            PAI=0;
            EPI=100;
            }
            if (SelectVt == 2)
            {
            PAI=10;
            EPI=100;
            }
            if (SelectVt == 3)
            {
            PAI=30;
            EPI=100;
            }
        } 
        if (Fr == 15)
      {
            if (SelectVt == 0)
            {
            PAI=0;
            EPI=100;
            } 
            if (SelectVt == 1)
            {
            PAI=5;
            EPI=100;
            }
            if (SelectVt == 2)
            {
            PAI=20;
            EPI=100;
            }
            if (SelectVt == 3)
            {
            PAI=80;
            EPI=100;
            }
        } 
        if (Fr == 16)
      {
            if (SelectVt == 0)
            {
            PAI=5;
            EPI=100;
            } 
            if (SelectVt == 1)
            {
            PAI=15;
            EPI=100;
            }
            if (SelectVt == 2)
            {
            PAI=40;
            EPI=100;
            }
            if (SelectVt == 3)
            {
            PAI=80;
            EPI=100;
            }
        } 
        if (Fr == 17)
      {
            if (SelectVt == 0)
            {
            PAI=10;
            EPI=100;
            } 
            if (SelectVt == 1)
            {
            PAI=25;
            EPI=100;
            }
            if (SelectVt == 2)
            {
            PAI=100;
            EPI=100;
            }
            if (SelectVt == 3)
            {
            PAI=100;
            EPI=100;
            }
        } 
  }

 // 2. GESTION DES L'AFFICHAGE DES REGLAGES
 
    // 2.1 SUPPRESSION DE LA PREMIERE LIGNE
 lcd.setCursor(0, 0);
 lcd.print("                ");// Effacement des 16 caractères de la première ligne

   // 2.2 AFFICHAGE DU REGLAGE DE FREQUENCE
 lcd.setCursor(0, 0);
 lcd.print("Fr:");
 lcd.print(Fr);

   //2.3 AFFICHAGE DU REGLAGE I:E
 lcd.setCursor(8,0);
lcd.print("IE:1:");
 
   if ( IE == 1)
 {
   lcd.print("1.0");
 } 
    if ( IE == 2)
 {
lcd.print("1.5");
 } 
    if ( IE == 3)
 {
  lcd.print("2.0");
 } 


   //2.4 SUPPRESSION DE LA SECONDE LIGNE
 lcd.setCursor(0,1);
 lcd.print("                ");// Effacement des 16 caractères de la seconde ligne

   //2.5 AFFICHAGE DU REGLAGE DU VT
 lcd.setCursor(0,1);
 
 lcd.print("Vt:");
 lcd.print(Vt);


    //2.6 AFFICHAGE DU REGLAGE FiO2
 lcd.setCursor(8,1);
 lcd.print("FiO2:");
 lcd.print(FiO2);
 

 //3. COMMANDE DU CYCLE RESPIRATOIRE

   //3.1 CALCUL DES PARAMETRES DE CYCLE
   
 Tot= (60000/Fr)- CorTot; // Durée totale de cycle
 I=2*Tot/(IE+3);// Durée de l'inspiration
 E=Tot-I;// Durée de l'expiration
 
 x1=((PAI)* 0.01);
 x2=E*x1;
 E2=x2;// Durée expiration avec pompe (effet compresseur)
 E1 = E- E2;// Durée expiration sans pompe
 

 x3=((EPI)* 0.01);
 x4=I*x3;
 I1=x4;// Durée d'inspiraration avec vanne de vidange des brassards fermée
 I2 = I- I1;// Durée d'inspiraration avec vanne de vidange des brassards ouverte
 
 x5=((PPI)* 0.01);
 x6=I*x5;
 IP=x6;// Durée expiration avec pompe (effet compresseur)

 if ( IP <= I1)
 {
  IP11=IP;
  IP12=I1-IP11;
  IP21=0;
  IP22=I2;
 }
 if ( IP > I1)
 {
  IP11=I1;
  IP12=0;
  IP22=I-IP;
  IP21=I2-IP22;
 }

 
   //3.2 COMMANDE EXPIRATION SEULE
 digitalWrite(Pompe,LOW);
 digitalWrite(EVanI,HIGH);
 digitalWrite(EVanE,LOW);
 
 delay(E1); 

  //3.3 COMMANDE EXPIRATION +POMPE
 digitalWrite(Pompe,HIGH);
 digitalWrite(EVanI,HIGH);
 digitalWrite(EVanE,LOW);

 delay(E2); 
 
 

   //3.4 COMMANDE INSPIRATION AVEC POMPE EVANE FERMEE
 digitalWrite(Pompe,HIGH);
 digitalWrite(EVanI,LOW);
 digitalWrite(EVanE,HIGH);
 
 delay(IP11); 
 
    //3.5 COMMANDE INSPIRATION SANS POMPE EVANE FERMEE
 digitalWrite(Pompe,LOW);
 digitalWrite(EVanI,LOW);
 digitalWrite(EVanE,HIGH);
 
 delay(IP12); 

   //3.6 COMMANDE INSPIRATION AVEC POMPE EVANE OUVERTE
 digitalWrite(Pompe,HIGH);
 digitalWrite(EVanI,LOW);
 digitalWrite(EVanE,LOW);

 delay(IP21);
    //3.7 COMMANDE INSPIRATION SANS POMPE EVANE OUVERTE
 digitalWrite(Pompe,LOW);
 digitalWrite(EVanI,LOW);
 digitalWrite(EVanE,LOW);

 delay(IP22);


 
}
