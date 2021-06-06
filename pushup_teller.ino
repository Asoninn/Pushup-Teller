#include <LiquidCrystal.h>
#include <EEPROM.h>


// RS = 2, EN = 3, D4 = 10, D5 = 11, D6 = 12, D7 = 13;
LiquidCrystal lcd(4, 5, 10, 11, 12, 13);
const int recordButton = 7;
const int soundButton = 8;
const int trigPin = 3;
const int echoPin = 2;
const int piezo = 6;
long duration; // variabel for hvor lang tid lydbølgen bruker frem og tilbake til sensoren
int distance; // variabel for ett objekts avstand fra sensoren
int record; // pushup rekord, volatile memory
int counter = 0; //antall pushups
bool pushUpNed; //
bool soundOn = true;

void setup() {
  record = EEPROM.read(0); // Leser fra posisjon 0 i arduinoens lokale minne, = rekord
  lcd.begin(16,2);  // Starter LCD og initialiserer pins og knapper
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(recordButton, INPUT_PULLUP);
  pinMode(soundButton, INPUT_PULLUP);
  pinMode(piezo, OUTPUT); 
  Serial.begin(9600);

  lcd.setCursor(0,0);
  lcd.print("Antall:");
  lcd.print(counter);
  lcd.setCursor(0,1);
  lcd.print("Rekord:");
  lcd.print(record);
}
void loop() {
  //måler avstand fra senosren
  measureLength();
  checkSound();
  
  //Hvis en er i nedre fase i pushuppen: pushUpNed = true.
  if (distance < 15) {
    pushUpNed = true;
  }

  //Hvis man er over 40cm og har tidligere vært nede. Altså gjort en repetisjon
  if ((distance > 40) &&(pushUpNed)){
    counter = counter + 1;
    makeSound();
    printCounter();
    pushUpNed = false;
  }
  //Setter ny rekord, hvis knappen er trykt
  if (digitalRead(recordButton) == LOW) {
    setRecord();
    printNewRecord();
  }
  //Totalt samlet delay på 1 sekund gjør at repitisjonene må gjøres langsomt for bedre treningsutbytte
  delay(300);
}

//Sjekker om lydknappen er trykket, og setter bool verdien sounOn til true eller false
void checkSound(){
  if (digitalRead(soundButton) == LOW){
    if (soundOn) {
      soundOn = false;
    } else {
      soundOn = true;
    }
  }
}
//Skriver over adressen 0 i arduino minnet, oppdaterer instansvaribelen i tillegg.
void setRecord(){
  EEPROM.write(0, counter);
  record = counter;
}
//Printer antall pushups
void printCounter(){
  lcd.setCursor(7,0);
  lcd.print(counter);
}
//Printer rekord
void printNewRecord(){
  //Sletter tdiligere rekord i tilfellet den er større enn den nye
  for (int i = 7; i < 11; i++){
    lcd.setCursor(i,1);
    lcd.print(" ");
  }
  lcd.setCursor(7,1);
  lcd.print(record);
}
//Sender siganl til piezo som lager lyd for å bekrefte en repitisjon om  sounOn = true
void makeSound(){
  if (soundOn) {
    tone(piezo, 300);
    delay(300);
    noTone(piezo);
  }
}

//måler avstand fra sensoren
void measureLength(){
   // Setter trigPin til 0
  digitalWrite(trigPin, LOW);
  delayMicroseconds(5);
  // Sette trigPin til 1,. Sender ut en lydbølge i 10 mikrosekunder
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  // Leser av echoPin og får tiden det tok for lydbølgen å reise fra sensoren til objektet og tilbake
  duration = pulseIn(echoPin, HIGH);
  //Setter lokal variabel avstand = tid * lydens hastighet / 2 ettersom bølgen må gå frem og tilbake
  distance = duration * 0.034 / 2;
}
