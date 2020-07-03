
#include "HX711.h"

HX711 P1; //Déclaration du capteur de pression du circuit patient
HX711 P2;// Déclaration du cpateur de pression du circuit de gaz frais

// Déclaration des variables pour les capteurs de pression

long V1;//Valeur du capteur de pression du circuit patient
long V2;//Valeur du capteur de pression du circuit de gaz frais
long RefV1=2056441;//Valeur de référence à vide du capteur de pression du circuit patient
long RefV2=-2419013;//Valeur de référence à vide du capteur de pression du circuit de gaz frais
long EtatV1;// Différence entre la valeur du capteur du circuit patient et sa valeur à vide
long EtatV2;// Différence entre la valeur du capteur du circuit de gaz frais et sa valeur à vide
const int DebPat=2;// Sortie logique de l'information de débranchement patient
const int GazFrais=3;// Sortie logique de l'information d'absence de gaz frais
int DB=0;
int GF=0;

// Déclaration des variables pour la liaison Série

String TC1 = "";         // Chaine pour contenir les données séries entrantes
bool stringComplete = false;  // Indicateur chaine complète
int TI1 = 0; //Valeur numérique convertie de la chaine de données série



// Déclaration des variables pour la fonction Mélangeur

int RO2 = 4;// Relais d'alimentation O2
int RAIR = 5;// Relais d'alimentation Air médical
int Vt=0;// Volume de gaz frais
int Tt=0;// Temps total d'ouverture gaz frais
int FiO2=0;//Valeur réglée de FiO2
int TO2=0;//Temp d'ouverture arrivée O2
int TAir=0;//Temps d'ouverture arrivée Air médical


void setup() 
{
P1.begin(A1,A0);//Initialisation de la communication avec le capteur de pression du circuit patient
P2.begin(A3,A2);//Initialisation de la communication avec le capteur de pression gaz frais
pinMode(DebPat,OUTPUT);// initialisation de la sortie logique 2 pour l'information d'alarme de débranchement patient
pinMode(GazFrais,OUTPUT);// initialisation de la sortie logique 3 pour l'information d'alarme d'absence de gaz frais
pinMode(RO2,OUTPUT);// initialisation de la sortie logique 4 pour le relais de l'alimentation d'O2
pinMode(RAIR,OUTPUT);// initialisation de la sortie logique 5 pour le relais de l'alimentation en Air médical
//RefV1=P1.read_average(10);//Valeur à vide du capteur de pression du circuit patient
//RefV2=P2.read_average(10);//Valeur à vide du capteur de pression de gaz frais
digitalWrite (DebPat, LOW);// Etat initial de la sortie logique d'information d'alarme de débranchement patient
digitalWrite (GazFrais, LOW);// Etat initial de la sortie logique d'information d'alarme d'absence de gaz frais
digitalWrite(RO2,HIGH);// Etat initial de la sortie logique pour le relais de commande de l'électrovanne d'arrivée d'O2
digitalWrite(RAIR,HIGH);// Etat initial de la sortie logique pour le relais de commande de l'électrovanne d'arrivée d'Air médical
Serial.begin(9600);//Initialisation de la communication série avec le microcontroleur principal
TC1.reserve(200);// Dimensionnement de la chaine de réception

}

void loop(){

//Mélangeur

  if (stringComplete) {  // A reception des réglages via la liaison série depuis le microcontrôleur principal

  
    
    //Alarme de débranchement du patient

    V1=P1.read();// Lecture de la valeur du capteur de pression du circuit patient
    EtatV1 = V1 - RefV1;//Calcul de la différence entre la valeur du capteur de pression du circuit patient et sa valeur de référence
    if (EtatV1 >= 10000)// Si la différence est supérieure ou égale à 10000
    {
    digitalWrite (DebPat, LOW);// Alors l'indicateur d'alarme de débranchement patient est à l'état haut
    DB=0;
    }
    else
    {
    digitalWrite (DebPat, HIGH);// Sinon l'indicateur d'alarme de débranchement patient est à l'état bas
    DB=1;
    }

    // Calcul des paramères de commande du mélangeur
    
    TI1 = TC1.toInt();// Conversion en numérique de la chaine de caractère en provenance du microcontroleur principal
    Vt= (TI1/1000);// Le réglage de volume tidal correspond à l'inité des milliers
    Tt=1500 *(0.4+(Vt*0.05));// Calcul du temps d'ouverture des gaz frais, 1500ms * Volume tidal
    FiO2=(TI1 - (Vt*1000));// La valeur de FiO2 de 60 à 100% correspond au trois derniers chiffres de TI1
    TO2=((((FiO2 -21)/0.79)*Tt)/100);// Calcul de la fraction d'ouverture de l'électrovanne d'O2
    TAir=Tt-TO2;// Calcul de la fraction d'ouverture de l'électrovanne d'air médical  

    //Commande du mélangeur
    
    digitalWrite (RO2, LOW); //Commande en ouverture du relais de l'électrovanne d'arrivée d'O2
    delay(TO2);// Délai du temps calculé d'ouverture de l'électrovanne d'arrivée d'O2
    
    //Alarme de gaz frais
    
    V2=P2.read();// Lecture de la valeur du capteur de pression du circuit de gaz frais
    EtatV2 = V2 - RefV2;//Calcul de la différence entre la valeur du capteur de pression de gaz frais et sa valeur de référence
    if (EtatV2 >= 5000)// Si la différence est supérieure ou égale à 5000
    {
    digitalWrite (GazFrais, LOW);// Alors l'indicateur d'alarme d'absence de gaz frais est à l'état haut
    GF=0;
    }
    else
    {
    digitalWrite (GazFrais, HIGH);//Sinon l'indicateur d'alarme d'absence de gaz frais est à l'état bas
    GF=1;
    }
    
    digitalWrite (RO2, HIGH);// Commande en fermeture du relais de l'électrovanne d'arrivée d'O2
    
    digitalWrite (RAIR, LOW);// Commande en ouverture du relais de l'électrovanne d'arrivée d'air médical
    delay(TAir);// Délai du temps calculé d'ouverture de l'électrovanne d'arrivé&e d'air médical
    digitalWrite (RAIR, HIGH);  // Commande en fermeture du relais de l'électrovanne d'arrivée d'air médical



    Serial.print("P1 Patient:");
    Serial.print(RefV1);
    Serial.print("  ");
    Serial.print(V1);
    Serial.print("  ");
    Serial.print(EtatV1);
    Serial.print("  ");
    Serial.print(DB);
    Serial.println();
    Serial.print("P2 Gaz Frais:");
    Serial.print(RefV2);
    Serial.print("  ");
    Serial.print(V2);
    Serial.print("  ");
    Serial.print(EtatV2);
    Serial.print("  ");
    Serial.print(GF);
    Serial.println();
    Serial.println();

    //Mise en attente de réception des nouveaux paramètres
  
    TC1 = "";// Vidage de la chaine de caractère transmise en liaison série depuis le microcontrôleur principal
    stringComplete = false;// Indicateur "Chaine complète" à l'état bas , en attente de nouveau caractères
  }
 
}
void serialEvent() {// En cas d'arrivée de données série depuis le microcontrôleur principal
  while (Serial.available()) {  
    char inChar = (char)Serial.read(); // Lire chaque nouveau caractère
    TC1 += inChar;// Et l'ajouter à la chaine de caractère
    if (inChar == '\n') {// Si le caractère est "/n" alors la chaine est complète
      stringComplete = true;// Indicateur "Chaine complète" à l'état haut
    }
  }
}
