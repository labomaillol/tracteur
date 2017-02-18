/* Contrôle du tracteur par bluetooth
   Module bluetooth HC-06
*/
//Assignation des pins de l'Arduino
#define motorPin1a 3 // Marche avant du premier moteur
#define motorPin1b 4 // Marche arrière du premier moteur
#define speedPin1 5 // L293D enable pin pour le premier moteur
#define motorPin2a 7 // Marche avant du deuxième moteur
#define motorPin2b 8 // Marche arrière du deuxième moteur
#define speedPin2 6 // L293D enable pin pour le deuxième moteur
#define Led1 2
#define Led2 9
#define Led3 10
#define echoPin 11
#define trigPin 12

// jusqu'à nouvel ordre, la vitesse du moteur est nulle
int Mspeed1= 255;
int Mspeed2= 255;

char instruction; // le message recu par bluetooth;
int mesure = 0;
long total = 0;
int vitesse=340;
void setup()
{
  Serial.begin(9600);
  //On initialise les LED du tracteur
  pinMode(Led1, OUTPUT);
  pinMode(Led2, OUTPUT);
  pinMode(Led3, OUTPUT);
  //On définit les pins du moteur 1
  digitalWrite(Led3, HIGH);
  digitalWrite(Led2, HIGH);
  pinMode(motorPin1a, OUTPUT);
  pinMode(motorPin1b, OUTPUT);
  pinMode(speedPin1, OUTPUT);
  //On s'assure que le moteur 1 est éteint
  digitalWrite(motorPin1a, LOW); 
  digitalWrite(motorPin1b, LOW);
  Serial.println("Moteur 1 Initialise");
  digitalWrite(Led3, LOW);
  digitalWrite(Led2, LOW);
  
  //On définit les pins du moteur 2
  pinMode(motorPin2a, OUTPUT);
  pinMode(motorPin2b, OUTPUT);
  pinMode(speedPin2, OUTPUT);
  //On s'assure que le moteur 2 est éteint
  digitalWrite(motorPin2a, LOW); 
  digitalWrite(motorPin2b, LOW);
  Serial.println("Moteur 2 Eteint");
  digitalWrite(Led3, HIGH);
  digitalWrite(Led2, HIGH);
  //Initialisation du moteur 2, tout à droite
  digitalWrite(speedPin2, HIGH); 
  digitalWrite(motorPin2a, HIGH);
  //Temps nécessaire pour arriver tout à droite
  //quelque soit la position initiale
  digitalWrite(Led3, HIGH);
  digitalWrite(Led2, LOW);
  delay(1000);
  digitalWrite(Led3, LOW);
  digitalWrite(Led2, HIGH);
  delay(1000);
  digitalWrite(Led1, HIGH);
  digitalWrite(Led2, LOW);
  delay(1000);
  digitalWrite(Led2, HIGH);
  digitalWrite(Led1, LOW);
  delay(1000);
  digitalWrite(Led3, LOW);
  digitalWrite(Led2, LOW);

  //On éteint le moteur 2
  digitalWrite(motorPin2a, LOW);
  //On fait tourner le moteur vers la gauche
  digitalWrite(motorPin2b, HIGH);
  //Temps nécessaire pour retrouver la position centrale
  digitalWrite(Led3, HIGH);
  delay(2000);
  //On éteint à nouveau le moteur.
  digitalWrite(motorPin2b, LOW);
  Serial.println("Moteur 2 Initialisé");
  digitalWrite(Led3, LOW);
  digitalWrite(Led2, HIGH);
  
  pinMode(echoPin, INPUT);
  pinMode(trigPin, OUTPUT);
  digitalWrite(trigPin, LOW);
  //Lumiere rouge pour informer qu'on attend une commande
  digitalWrite(Led1, HIGH);
}


void loop()
{
  //On définit le temps et la distance pour le SR04
  long temps, distance, distance0;
  distance0=distance;
  distance=0;
  //variable servant à mesurer plusieurs fois la distance
  //avant d'en afficher la moyenne
  mesure=mesure+1; 
  //On allume l'émetteur ultrason
  digitalWrite(trigPin, HIGH);
  //Durée d'initialisation du capteur
  delayMicroseconds(10);
  //Pulsation ultrasonore émise, on éteint l'émetteur
  digitalWrite(trigPin, LOW);
  //On définit la pin Echo comme en attente de message
  pinMode(echoPin, INPUT);
  //On recoit l'impulsion sur le récepteur
  temps = pulseIn(echoPin, HIGH);
  //On définit la distance comme étant le produit de la 
  //vitesse par le temps divisé par deux pour l'A-R.
  //Le temps étant en microsec(10^-6) et pour avoir d 
  //en cm (10-2), on divise par 10 000 (10^-4)
  distance = temps * vitesse/(2*10000);
  long erreur;
  erreur=(distance-distance0)/100;
  if (-2 <erreur and erreur<2){
    //Serial.print("Distance ");
    //Serial.print(mesure);
    //Serial.print(" : ");
    //Serial.println(distance);
    //On créé une variable total qui sera la distance totale
    // pour 5 mesures.
    total=total+distance;
    //Si les 5 mesures sont faites :
    if (mesure>4){
      //On fait la moyenne des 10 mesures
      total=total/5;
      // on affiche l'information sur le moniteur série:
      Serial.println("///////////////////////////");
      Serial.print("Distance moyenne:  ");
      Serial.print(total);
      Serial.println(" cm");
      Serial.println("///////////////////////////");
      //On remet mesure à 0 pour recommencer une série
      mesure=0;
      total=0;
    if (0<distance & distance<5){ 
      digitalWrite(motorPin1a, LOW); 
      digitalWrite(motorPin1b, LOW);
      Serial.println("Trop près, Moteur 1 Stop");
      digitalWrite(motorPin2a, LOW); 
      digitalWrite(motorPin2b, LOW);
      Serial.println("Trop près, Moteur 1 Stop");
      delay(1000);
      }
    }
  }
  char message;
  if (Serial.available()) // réception d'un message
  {
    message = Serial.read(); // lecture du message reçu
  }
  if (message != instruction) { // alors c'est un nouveau message

    instruction = message;

    if (instruction == 'a')
    {
    analogWrite(speedPin1, Mspeed1); 
    digitalWrite(motorPin1a, LOW); 
    digitalWrite(motorPin1b, HIGH);
    Serial.print("Moteur 1 Avance a ");
    Serial.println(Mspeed1);
    digitalWrite(Led1, LOW);
    digitalWrite(Led2, HIGH);
    digitalWrite(Led3, LOW);
    }
    else if (instruction == 'b')
    {
    digitalWrite(speedPin2, HIGH); 
    digitalWrite(motorPin2a, LOW); 
    digitalWrite(motorPin2b, HIGH);
    Serial.print("Tracteur tourne à gauche");
    }
    else if (instruction == 'c')
    {
    analogWrite(speedPin1, Mspeed1); 
    digitalWrite(motorPin1a, HIGH); 
    digitalWrite(motorPin1b, LOW);
    Serial.print("Moteur 1 recule a ");
    Serial.println(Mspeed1);
    digitalWrite(Led1, LOW);
    digitalWrite(Led2, LOW);
    digitalWrite(Led3, HIGH);
    }
    else if (instruction == 'd')
    {
    digitalWrite(speedPin2, HIGH); 
    digitalWrite(motorPin2a, HIGH); 
    digitalWrite(motorPin2b, LOW);
    Serial.print("Tracteur tourne à droite");
    }
    else if (instruction == 'e')
    {
    analogWrite(speedPin1, Mspeed1); 
    digitalWrite(motorPin1a, LOW); 
    digitalWrite(motorPin1b, LOW);
    Serial.println("Moteur 1 Stop");
    digitalWrite(Led1, HIGH);
    digitalWrite(Led2, LOW);
    digitalWrite(Led3, LOW);
    }
    else if (instruction == 'f')
    {
    analogWrite(speedPin2, Mspeed2); 
    digitalWrite(motorPin2a, LOW); 
    digitalWrite(motorPin2b, LOW);
    Serial.println("Moteur 2 Stop");
    }
    else if (instruction == 'p')
    {
    if (Mspeed1<255){
      Mspeed1=Mspeed1+20 ;
      Serial.println("On accélère");  
    }
    }
    else if (instruction == 'm')
    {
      if (Mspeed1<255){
        Mspeed1=Mspeed1-20;
        Serial.println("On ralenti");
      }
    else if (instruction == 'v')
    {
      vitesse=vitesse-1;
      Serial.print("La vitesse du son a diminue : ");
      Serial.print(vitesse);
      Serial.println(" m/s");
      }
    else if (instruction == 'w')
    {
      vitesse=vitesse+1;
      Serial.print("La vitesse du son a augmente : ");
      Serial.print(vitesse);
      Serial.println(" m/s");
      }
    }
    }
    delay(100);
}
