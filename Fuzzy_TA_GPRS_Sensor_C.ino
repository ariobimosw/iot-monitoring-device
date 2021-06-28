#include <Fuzzy.h>
#include <String.h>
#include <SoftwareSerial.h>
#include "DHT.h"

SoftwareSerial mySerial (11,10);
#define DHTPIN 32
#define DHTTYPE DHT11
DHT dht (DHTPIN, DHTTYPE);
int Flame;
int ledPin = 36; 
int fire = 1;
int xfire= 0;
int peringatanAman= 1, peringatanWaspada= 2, peringatanPerlutindakan= 3;
const int flamePin = 34;
float temperature = 0.0, humidity = 0.0;

String str = "GET https://api.thingspeak.com/update?api_key=WB3Q9ZTZFAQGZVNM";
uint8_t GSM_Response = 0;
unsigned long previousMillis = 0;

void sendAT(char *command, unsigned long waitms)
{
  unsigned long PrevMillis = millis();
  mySerial.write(command);
  unsigned long currentMillis = millis();
  while (((currentMillis - PrevMillis) < waitms))
  {
    serialEvent();
    currentMillis = millis();
  }
}

// Fuzzy
Fuzzy *fuzzy = new Fuzzy();

// FuzzyInput
FuzzySet *dingin = new FuzzySet(0, 0, 20, 25);
FuzzySet *sejuk = new FuzzySet(20, 25, 25, 30);
FuzzySet *panas = new FuzzySet(25, 30, 90, 90);

// FuzzyInput
FuzzySet *kering = new FuzzySet(0, 0, 40, 50);
FuzzySet *normal = new FuzzySet(40, 50, 50, 60);
FuzzySet *lembab = new FuzzySet(50, 60, 100, 100);


// FuzzyInput
FuzzySet *ada = new FuzzySet(0, 0, 0, 0);
FuzzySet *tidakada = new FuzzySet(1, 1, 1, 1);


// FuzzyOutput
FuzzySet *aman = new FuzzySet(0, 15, 15, 30);
FuzzySet *waspada = new FuzzySet(30, 45, 45, 60);
FuzzySet *perlutindakan = new FuzzySet(60, 75, 75, 90);

void setup()
{
  mySerial.begin(9600);
  Serial.begin(9600);
  dht.begin();
  pinMode(flamePin, INPUT);
  pinMode(ledPin, OUTPUT);
  delay(1000);
  tcpInit();
  Serial.println();
  // Every setup must occur in the function setup()

  // FuzzyInput
  FuzzyInput *suhu = new FuzzyInput(1);

  suhu->addFuzzySet(dingin);
  suhu->addFuzzySet(sejuk);
  suhu->addFuzzySet(panas);
  fuzzy->addFuzzyInput(suhu);

  // FuzzyInput
  FuzzyInput *kelembaban = new FuzzyInput(2);

  kelembaban->addFuzzySet(kering);
  kelembaban->addFuzzySet(normal);
  kelembaban->addFuzzySet(lembab);
  fuzzy->addFuzzyInput(kelembaban);

  // FuzzyInput
  FuzzyInput *deteksiApi = new FuzzyInput(3);

  deteksiApi->addFuzzySet(tidakada);
  deteksiApi->addFuzzySet(ada);
  fuzzy->addFuzzyInput(deteksiApi);

  // FuzzyOutput
  FuzzyOutput *peringatan = new FuzzyOutput(1);

  peringatan->addFuzzySet(aman);
  peringatan->addFuzzySet(waspada);
  peringatan->addFuzzySet(perlutindakan);
  fuzzy->addFuzzyOutput(peringatan);

  FuzzyRuleConsequent *thenPeringatanAman = new FuzzyRuleConsequent();
  thenPeringatanAman->addOutput(aman);
  FuzzyRuleConsequent *thenPeringatanWaspada = new FuzzyRuleConsequent();
  thenPeringatanWaspada->addOutput(waspada);
  FuzzyRuleConsequent *thenPeringatanPerlutindakan = new FuzzyRuleConsequent();
  thenPeringatanPerlutindakan->addOutput(perlutindakan);
//Building BASIC FuzzyRule 
  //#1
  FuzzyRuleAntecedent *suhuDinginAndKelembabanLembab = new FuzzyRuleAntecedent();
  suhuDinginAndKelembabanLembab->joinWithAND(dingin, lembab);
  //#2
  FuzzyRuleAntecedent *suhuDinginAndKelembabanNormal = new FuzzyRuleAntecedent();
  suhuDinginAndKelembabanNormal->joinWithAND(dingin, normal);
  //#3
  FuzzyRuleAntecedent *suhuDinginAndKelembabanKering = new FuzzyRuleAntecedent();
  suhuDinginAndKelembabanKering->joinWithAND(dingin, kering);
  //#4
  FuzzyRuleAntecedent *suhuSejukAndKelembabanLembab = new FuzzyRuleAntecedent();
  suhuSejukAndKelembabanLembab->joinWithAND(sejuk, lembab);
  //#5
  FuzzyRuleAntecedent *suhuSejukAndKelembabanNormal = new FuzzyRuleAntecedent();
  suhuSejukAndKelembabanNormal->joinWithAND(sejuk, normal);
    //#6
  FuzzyRuleAntecedent *suhuSejukAndKelembabanKering = new FuzzyRuleAntecedent();
  suhuSejukAndKelembabanKering->joinWithAND(sejuk, kering);
    //#7
  FuzzyRuleAntecedent *suhuPanasAndKelembabanLembab = new FuzzyRuleAntecedent();
  suhuPanasAndKelembabanLembab->joinWithAND(panas, lembab);
    //#8
  FuzzyRuleAntecedent *suhuPanasAndKelembabanNormal = new FuzzyRuleAntecedent();
  suhuPanasAndKelembabanNormal->joinWithAND(panas, normal);
  //#9
  FuzzyRuleAntecedent *suhuPanasAndKelembabanKering = new FuzzyRuleAntecedent();
  suhuPanasAndKelembabanKering->joinWithAND(panas, kering);
 // Building FuzzyRule #1
FuzzyRuleAntecedent *deteksiApiTidakada = new FuzzyRuleAntecedent();
  deteksiApiTidakada->joinSingle(tidakada);
FuzzyRuleAntecedent *ifSuhuDinginAndKelembabanLembabAndDeteksiApiTidakada = new FuzzyRuleAntecedent();
  ifSuhuDinginAndKelembabanLembabAndDeteksiApiTidakada->joinWithAND(suhuDinginAndKelembabanLembab, deteksiApiTidakada);
FuzzyRule *fuzzyRule1 = new FuzzyRule(1, ifSuhuDinginAndKelembabanLembabAndDeteksiApiTidakada, thenPeringatanAman);
  fuzzy->addFuzzyRule(fuzzyRule1);
// Building FuzzyRule #2
  FuzzyRuleAntecedent *ifSuhuDinginAndKelembabanNormalAndDeteksiApiTidakada = new FuzzyRuleAntecedent();
  ifSuhuDinginAndKelembabanNormalAndDeteksiApiTidakada->joinWithAND(suhuDinginAndKelembabanNormal, tidakada);
  FuzzyRule *fuzzyRule2 = new FuzzyRule(2, ifSuhuDinginAndKelembabanNormalAndDeteksiApiTidakada, thenPeringatanAman);
  fuzzy->addFuzzyRule(fuzzyRule2);
// Building FuzzyRule #3
  FuzzyRuleAntecedent *ifSuhuDinginAndKelembabanKeringAndDeteksiApiTidakada = new FuzzyRuleAntecedent();
  ifSuhuDinginAndKelembabanKeringAndDeteksiApiTidakada->joinWithAND(suhuDinginAndKelembabanKering, tidakada);
  FuzzyRule *fuzzyRule3 = new FuzzyRule(3, ifSuhuDinginAndKelembabanKeringAndDeteksiApiTidakada, thenPeringatanAman);
  fuzzy->addFuzzyRule(fuzzyRule3);
    // Building FuzzyRule #4
  FuzzyRuleAntecedent *ifSuhuDinginAndKelembabanLembabAndDeteksiApiAda = new FuzzyRuleAntecedent();
  ifSuhuDinginAndKelembabanLembabAndDeteksiApiAda->joinWithAND(suhuDinginAndKelembabanLembab, ada);
  FuzzyRule *fuzzyRule4 = new FuzzyRule(4, ifSuhuDinginAndKelembabanLembabAndDeteksiApiAda, thenPeringatanAman);
  fuzzy->addFuzzyRule(fuzzyRule4);
// Building FuzzyRule #5
  FuzzyRuleAntecedent *ifSuhuDinginAndKelembabanNormalAndDeteksiApiAda = new FuzzyRuleAntecedent();
  ifSuhuDinginAndKelembabanNormalAndDeteksiApiAda->joinWithAND(suhuDinginAndKelembabanNormal, ada);
  FuzzyRule *fuzzyRule5 = new FuzzyRule(5, ifSuhuDinginAndKelembabanNormalAndDeteksiApiAda, thenPeringatanAman);
  fuzzy->addFuzzyRule(fuzzyRule5);
// Building FuzzyRule #6                            
  FuzzyRuleAntecedent *ifSuhuDinginAndKelembabanKeringAndDeteksiApiAda = new FuzzyRuleAntecedent();
  ifSuhuDinginAndKelembabanKeringAndDeteksiApiAda->joinWithAND(suhuDinginAndKelembabanKering, ada);
  FuzzyRule *fuzzyRule6 = new FuzzyRule(6, ifSuhuDinginAndKelembabanKeringAndDeteksiApiAda, thenPeringatanWaspada);
  fuzzy->addFuzzyRule(fuzzyRule6);
// Building FuzzyRule #7                            
  FuzzyRuleAntecedent *ifSuhuSejukAndKelembabanLembabAndDeteksiApiTidakada = new FuzzyRuleAntecedent();
  ifSuhuSejukAndKelembabanLembabAndDeteksiApiTidakada->joinWithAND(suhuSejukAndKelembabanLembab, tidakada);
  FuzzyRule *fuzzyRule7 = new FuzzyRule(7, ifSuhuSejukAndKelembabanLembabAndDeteksiApiTidakada, thenPeringatanAman);
  fuzzy->addFuzzyRule(fuzzyRule7);
// Building FuzzyRule #8                             
  FuzzyRuleAntecedent *ifSuhuSejukAndKelembabanNormalAndDeteksiApiTidakada = new FuzzyRuleAntecedent();
  ifSuhuSejukAndKelembabanNormalAndDeteksiApiTidakada->joinWithAND(suhuSejukAndKelembabanNormal, tidakada);
  FuzzyRule *fuzzyRule8 = new FuzzyRule(8, ifSuhuSejukAndKelembabanNormalAndDeteksiApiTidakada, thenPeringatanAman);
  fuzzy->addFuzzyRule(fuzzyRule8);
// Building FuzzyRule #9                            
  FuzzyRuleAntecedent *ifSuhuSejukAndKelembabanKeringAndDeteksiApiTidakada = new FuzzyRuleAntecedent();
  ifSuhuSejukAndKelembabanKeringAndDeteksiApiTidakada->joinWithAND(suhuSejukAndKelembabanKering, tidakada);
  FuzzyRule *fuzzyRule9 = new FuzzyRule(9, ifSuhuSejukAndKelembabanKeringAndDeteksiApiTidakada, thenPeringatanWaspada);
  fuzzy->addFuzzyRule(fuzzyRule9);
// Building FuzzyRule #10                           
  FuzzyRuleAntecedent *ifSuhuSejukAndKelembabanLembabAndDeteksiApiAda = new FuzzyRuleAntecedent();
  ifSuhuSejukAndKelembabanLembabAndDeteksiApiAda->joinWithAND(suhuSejukAndKelembabanLembab, ada);
  FuzzyRule *fuzzyRule10 = new FuzzyRule(10, ifSuhuSejukAndKelembabanLembabAndDeteksiApiAda, thenPeringatanAman);
  fuzzy->addFuzzyRule(fuzzyRule10);
// Building FuzzyRule #11                            
  FuzzyRuleAntecedent *ifSuhuSejukAndKelembabanNormalAndDeteksiApiAda = new FuzzyRuleAntecedent();
  ifSuhuSejukAndKelembabanNormalAndDeteksiApiAda->joinWithAND(suhuSejukAndKelembabanNormal, ada);
  FuzzyRule *fuzzyRule11 = new FuzzyRule(11, ifSuhuSejukAndKelembabanNormalAndDeteksiApiAda, thenPeringatanWaspada);
  fuzzy->addFuzzyRule(fuzzyRule11);
// Building FuzzyRule #12                             
  FuzzyRuleAntecedent *ifSuhuSejukAndKelembabanKeringAndDeteksiApiAda = new FuzzyRuleAntecedent();
  ifSuhuSejukAndKelembabanKeringAndDeteksiApiAda->joinWithAND(suhuSejukAndKelembabanKering, ada);
  FuzzyRule *fuzzyRule12 = new FuzzyRule(12, ifSuhuSejukAndKelembabanKeringAndDeteksiApiAda, thenPeringatanPerlutindakan);
  fuzzy->addFuzzyRule(fuzzyRule12);
// Building FuzzyRule #13                            
  FuzzyRuleAntecedent *ifSuhuPanasAndKelembabanLembabAndDeteksiApiTidakada = new FuzzyRuleAntecedent();
  ifSuhuPanasAndKelembabanLembabAndDeteksiApiTidakada->joinWithAND(suhuPanasAndKelembabanLembab, tidakada);
  FuzzyRule *fuzzyRule13 = new FuzzyRule(13, ifSuhuPanasAndKelembabanLembabAndDeteksiApiTidakada, thenPeringatanAman);
  fuzzy->addFuzzyRule(fuzzyRule13);
// Building FuzzyRule #14                            
  FuzzyRuleAntecedent *ifSuhuPanasAndKelembabanNormalAndDeteksiApiTidakada = new FuzzyRuleAntecedent();
  ifSuhuPanasAndKelembabanNormalAndDeteksiApiTidakada->joinWithAND(suhuPanasAndKelembabanNormal, tidakada);
  FuzzyRule *fuzzyRule14 = new FuzzyRule(14, ifSuhuPanasAndKelembabanNormalAndDeteksiApiTidakada, thenPeringatanWaspada);
  fuzzy->addFuzzyRule(fuzzyRule14);
// Building FuzzyRule #15                            
  FuzzyRuleAntecedent *ifSuhuPanasAndKelembabanKeringAndDeteksiApiTidakada = new FuzzyRuleAntecedent();
  ifSuhuPanasAndKelembabanKeringAndDeteksiApiTidakada->joinWithAND(suhuPanasAndKelembabanKering, tidakada);
  FuzzyRule *fuzzyRule15 = new FuzzyRule(15, ifSuhuPanasAndKelembabanKeringAndDeteksiApiTidakada, thenPeringatanWaspada);
  fuzzy->addFuzzyRule(fuzzyRule15);
// Building FuzzyRule #16                           
  FuzzyRuleAntecedent *ifSuhuPanasAndKelembabanLembabAndDeteksiApiAda = new FuzzyRuleAntecedent();
  ifSuhuPanasAndKelembabanLembabAndDeteksiApiAda->joinWithAND(suhuPanasAndKelembabanLembab, ada);
  FuzzyRule *fuzzyRule16 = new FuzzyRule(16, ifSuhuPanasAndKelembabanLembabAndDeteksiApiAda, thenPeringatanWaspada);
  fuzzy->addFuzzyRule(fuzzyRule16);
  // Building FuzzyRule #17                            
  FuzzyRuleAntecedent *ifSuhuPanasAndKelembabanNormalAndDeteksiApiAda = new FuzzyRuleAntecedent();
  ifSuhuPanasAndKelembabanNormalAndDeteksiApiAda->joinWithAND(suhuPanasAndKelembabanNormal, ada);
  FuzzyRule *fuzzyRule17 = new FuzzyRule(17, ifSuhuPanasAndKelembabanNormalAndDeteksiApiAda, thenPeringatanPerlutindakan);
  fuzzy->addFuzzyRule(fuzzyRule17);
// Building FuzzyRule #18                            
  FuzzyRuleAntecedent *ifSuhuPanasAndKelembabanKeringAndDeteksiApiAda = new FuzzyRuleAntecedent();
  ifSuhuPanasAndKelembabanKeringAndDeteksiApiAda->joinWithAND(suhuPanasAndKelembabanKering, ada);
  FuzzyRule *fuzzyRule18 = new FuzzyRule(18, ifSuhuPanasAndKelembabanKeringAndDeteksiApiAda, thenPeringatanPerlutindakan);
  fuzzy->addFuzzyRule(fuzzyRule18);
}

void loop()
{
  humidity = dht.readHumidity();
  temperature = dht.readTemperature();
  delay(2);
  Flame = digitalRead(flamePin);

  Serial.println("\n\n\nCurrent Condition: ");
  Serial.print("\t\t\tTemperature: ");
  Serial.print(temperature);
  Serial.print(", Humidity: ");
  Serial.print(humidity);
  Serial.println();

  fuzzy->setInput(1, temperature);
  fuzzy->setInput(2, humidity);
  fuzzy->setInput(3, Flame);

  fuzzy->fuzzify();
  delay(2000);

  Serial.println("Input: ");
  Serial.print("\tSuhu: Dingin-> ");
  Serial.print(dingin->getPertinence());
  Serial.print(", Normal-> ");
  Serial.print(sejuk->getPertinence());
  Serial.print(", Panas-> ");
  Serial.println(panas->getPertinence());

  Serial.print("\tKelembaban: Kering-> ");
  Serial.print(kering->getPertinence());
  Serial.print(",  Normal-> ");
  Serial.print(normal->getPertinence());
  Serial.print(",  Lembab-> ");
  Serial.println(lembab->getPertinence());


  Serial.print("\tDeteksi Api: Tidak ada-> ");
  Serial.print(tidakada->getPertinence());
  Serial.print(", Ada-> ");
  Serial.print(ada->getPertinence());
  Serial.println();

  float output1 = fuzzy->defuzzify(1);

  Serial.println("Output: ");
  Serial.print("\tPeringatan: Normal-> ");
  Serial.print(aman->getPertinence());
  Serial.print(", Waspada-> ");
  Serial.print(waspada->getPertinence());
  Serial.print(", Perlu tindakan-> ");
  Serial.println(perlutindakan->getPertinence());

  Serial.println("Result: ");
  Serial.print("\t\t\tPeringatan: ");
  Serial.print(output1);
  Serial.println();

  if ((output1>=0) && (output1<30) && (Flame==HIGH))
  {
    Serial.println("\t\t\t\tAman");
    Serial.println();
    digitalWrite(ledPin, HIGH);
    String StringToThingSpeak = "&field1=" +String(temperature) +  "&field2=" + String (humidity) + "&field3=" +String (xfire) + "&field4=" +String(peringatanAman);
    mySerial.println(StringToThingSpeak);
    mySerial.write(0x1A);
    delay(2000);
    serialEvent(); 
  }
  else if ((output1>=0) && (output1<30) && (Flame==LOW))
  {
    Serial.println("\t\t\t\tAman");
    Serial.println();
    digitalWrite(ledPin, LOW);
    String StringToThingSpeak = "&field1=" +String(temperature) +  "&field2=" + String (humidity) + "&field3=" +String (fire) + "&field4=" +String(peringatanAman);
    mySerial.println(StringToThingSpeak);
    mySerial.write(0x1A);
    delay(2000);
    serialEvent(); 
  }
  else if ((output1>=30) && (output1<60) && (Flame==HIGH))
  {
    Serial.println("\t\t\t\tWaspada");
    Serial.println();
    String StringToThingSpeak = "&field1=" +String(temperature) +  "&field2=" + String (humidity) + "&field3=" +String (xfire) + "&field4=" +String(peringatanWaspada);
    mySerial.println(StringToThingSpeak);
    mySerial.write(0x1A);
    delay(2000);
    serialEvent(); 
  }
  else if ((output1>=30) && (output1<60) && (Flame==LOW))
  {
    Serial.println("\t\t\t\tWaspada");
    Serial.println();
    String StringToThingSpeak = "&field1=" +String(temperature) +  "&field2=" + String (humidity) + "&field3=" +String (fire) + "&field4=" +String(peringatanWaspada);
    mySerial.println(StringToThingSpeak);
    mySerial.write(0x1A);
    delay(2000);
    serialEvent(); 
  }
  else if ((output1>=60) && (Flame==HIGH))
  {
    Serial.println("\t\t\t\tPerlutindakan");
    Serial.println();
    String StringToThingSpeak = "&field1=" +String(temperature) +  "&field2=" + String (humidity) + "&field3=" +String (xfire) + "&field4=" +String(peringatanPerlutindakan);
    mySerial.println(StringToThingSpeak);
    mySerial.write(0x1A);
    delay(2000);
    serialEvent(); 
  }
    else if ((output1>=60) && (Flame==LOW))
  {
    Serial.println("\t\t\t\tPerlutindakan");
    Serial.println();
    String StringToThingSpeak = "&field1=" +String(temperature) +  "&field2=" + String (humidity) + "&field3=" +String (fire) + "&field4=" +String(peringatanPerlutindakan);
    mySerial.println(StringToThingSpeak);
    mySerial.write(0x1A);
    delay(2000);
    serialEvent(); 
  }
  // wait 12 seconds
  delay(6000);
  
  sendAT("AT+CIPSPRT=0\r\n",4000);
  sendAT("AT+CIPSTART=\"TCP\",\"api.thingspeak.com\",\"80\"\r\n",5000);
  sendAT("AT+CIPSEND\r\n",2000);
  mySerial.print(str);

}

void tcpInit()
{
  sendAT("AT\r\n",1000);
  sendAT("AT+CPIN?\r\n",1000);
  sendAT("AT+CREG?\r\n",1000);
  sendAT("AT+CGATT?\r\n",1000);
  sendAT("AT+CIPSHUT\r\n",1000);
  sendAT("AT+CIPSTATUS\r\n",2000);
  sendAT("AT+CIPMUX=0\r\n",2000);
  sendAT("AT+CSTT=\"internet\"\r\n",1000);
  sendAT("AT+CIICR\r\n",3000);
  sendAT("AT+CIFSR\r\n",2000);
  sendAT("AT+CIPSPRT=0\r\n",4000);
  sendAT("AT+CIPSTART=\"TCP\",\"api.thingspeak.com\",\"80\"\r\n",5000);
  sendAT("AT+CIPSEND\r\n",2000);
  String str="GET https://api.thingspeak.com/update?api_key=WB3Q9ZTZFAQGZVNM&field1="+ String(temperature);
  mySerial.println(str);
  mySerial.write(0x1A);
  delay(5000);
  serialEvent();
}

void serialEvent()
{
  while(mySerial.available())
  Serial.write(mySerial.read());
}
