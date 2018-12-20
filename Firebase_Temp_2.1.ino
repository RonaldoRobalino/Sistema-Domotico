#include <ESP8266WiFi.h>
#include "DHT.h"
#define DHTTYPE DHT11
#include <FirebaseArduino.h>

#define FIREBASE_HOST "domotica-pro.firebaseio.com"
#define FIREBASE_AUTH "BBUyVOIHQxvnO4M7cAxZHEv3iX8QlJgWMKZeYb7P"
#define WIFI_SSID "iCarly"
#define WIFI_PASSWORD "carly1998"

const int LED1 = 4; //lo conectamos a D2
const int DHTPin = 14; // lo conectamos a D5
const int pirpin = 12; // lo conectamos a D6
const int MagnetoPin = 13; // lo conectamos a D7
const int EchoPin = 0; // lo conectamos a D3
const int TriggerPin = 2; // lo conectamos a D4
const int LED2 = 5; // lo conectamos a D1 
const int LDRPin = 17;//lo conectamos a A0 
const int rango = 150;// Valor para LDR en la noche 
const int Buzzerpin=15 ;// lo conectamos a D8
static char celsiusTemp[7];
static char fahrenheitTemp[7];
static char humidityTemp[7];
double hic;
int pirVal = 0;
int MagnetoVal = 0;
const float VelSon = 34000.0;
float distancia;


WiFiServer server(55000);

// Initialize DHT sensor.
DHT dht(DHTPin, DHTTYPE);


void iniciarTrigger()
{
  // Ponemos el Triiger en estado bajo y esperamos 2 ms
  digitalWrite(TriggerPin, LOW);
  delayMicroseconds(2);
  
  // Ponemos el pin Trigger a estado alto y esperamos 10 ms
  digitalWrite(TriggerPin, HIGH);
  delayMicroseconds(10);
  
  // Comenzamos poniendo el pin Trigger en estado bajo
  digitalWrite(TriggerPin, LOW);
}


void setup() {

Serial.begin(9600);
 WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
 Serial.print("Conectando a red WIFI");
 while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("Conectado OK");
  Serial.println(WiFi.localIP());
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  dht.begin();
  pinMode(pirpin,INPUT);
  pinMode(MagnetoPin, INPUT_PULLUP);
  pinMode(TriggerPin, OUTPUT);
  pinMode(EchoPin, INPUT);
  pinMode(LED1, OUTPUT);
  Firebase.setFloat("LED",0);
  pinMode(Buzzerpin, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LDRPin, INPUT);
    
}

void loop() {
  iniciarTrigger();
  unsigned long tiempo = pulseIn(EchoPin, HIGH);
  distancia = tiempo * 0.000001 * VelSon / 2.0;
  Firebase.setFloat("Distancia",distancia);
  if(distancia<10){
      tone(Buzzerpin, 700);
  }
  else{
      noTone(Buzzerpin);
  }
  
  pirVal = digitalRead(pirpin);
  Firebase.setFloat("PIR",pirVal);

  int MagnetoVal = digitalRead(MagnetoPin);
  if (MagnetoVal == LOW) {
    Serial.println(" Puerta Cerrada ");
  }
  else {
    Serial.println(" Puerta ABIERTA ");
  }
  Firebase.setFloat("Puerta",MagnetoVal);
  
  
 float h = dht.readHumidity();
 float t = dht.readTemperature();
 float f = dht.readTemperature(true);
 if (isnan(h) || isnan(t) ) {
              Serial.println("Failed to read from DHT sensor!");
              strcpy(celsiusTemp,"Failed");
              strcpy(fahrenheitTemp, "Failed");
              strcpy(humidityTemp, "Failed");  
              return;       
 }
 else{
              double hic = dht.computeHeatIndex(t, h, false);       
              dtostrf(hic, 6, 2, celsiusTemp);             
              double hif = dht.computeHeatIndex(f, h);
              dtostrf(hif, 6, 2, fahrenheitTemp);         
              dtostrf(h, 6, 2, humidityTemp);
              Firebase.setFloat("Temperatura", hic);
              Firebase.setFloat("Humedad",h);
              if (Firebase.failed()) {
                  Serial.print("setting /number failed:");
                  Serial.println(Firebase.error());  
                  return;
              }
}

String ledstatus=Firebase.getString("LED");
int ledint=ledstatus.toInt();
   if (ledint == 1 )  {
   digitalWrite(LED1,HIGH);    
  }
  else{
   digitalWrite(LED1,LOW);   
  }

int input = analogRead(LDRPin);
   if (input > rango) {
      digitalWrite(LED2, LOW);
      //Serial.print(input);
      Firebase.setInt("Nivel de Luz",input);
   }
   else {
      digitalWrite(LED2, HIGH);
      //Serial.print(input);
      Firebase.setInt("Nivel de Luz",input);
   }
   
String alarmstatus=Firebase.getString("Alarma");
int alarmaint= alarmstatus.toInt();
  
 if(alarmaint == 1){
  if(MagnetoVal == HIGH){
      tone(Buzzerpin, 1000);
      delay(1000);
      tone(Buzzerpin, 400);
      delay(250);
  }
 }
 else{
    noTone(Buzzerpin);
  }


//Impresion de Datos
  Serial.print("Distancia: ");
  Serial.print(distancia);
  Serial.println(" cm");
  
   Serial.print("Temperatura: ");            
   Serial.print(hic);
   Serial.println("   °C");
   Serial.print("Humedad: ");
   Serial.println(h);
   Serial.print("Se activo la alarma");
delay(100);        // retraso entre lecturas para la estabilidad
}
