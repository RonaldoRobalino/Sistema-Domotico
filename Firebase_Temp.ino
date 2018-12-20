#include <ESP8266WiFi.h>
#include "DHT.h"
#define DHTTYPE DHT11
#include <FirebaseArduino.h>

#define FIREBASE_HOST "domotica-pro.firebaseio.com"
#define FIREBASE_AUTH "BBUyVOIHQxvnO4M7cAxZHEv3iX8QlJgWMKZeYb7P"
#define WIFI_SSID "iCarly"
#define WIFI_PASSWORD "carly1998"

int LED1 = 4; //lo conectamos a D2
const int DHTPin = 14; // lo conectamos a D5
const int pirpin = 12; // lo conectamos a D6
const int MagnetoPin = 13; // lo conectamos a D7
const int EchoPin = 0; // lo conectamos a D3
const int TriggerPin = 2; // lo conectamos a D4

static char celsiusTemp[7];
static char fahrenheitTemp[7];
static char humidityTemp[7];
double hic;
int pirVal = 0;
int MagnetoVal = 0;
// Constante velocidad sonido en cm/s
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
    
}

void loop() {

  iniciarTrigger();
  unsigned long tiempo = pulseIn(EchoPin, HIGH);

  // Obtenemos la distancia en cm, hay que convertir el tiempo en segudos ya que está en microsegundos
  // por eso se multiplica por 0.000001
  distancia = tiempo * 0.000001 * VelSon / 2.0;
  Serial.print(distancia);
  Serial.print(" cm");
  Serial.println();
  delay(500);
  Firebase.setFloat("Distancia",distancia);


 
  pirVal = digitalRead(pirpin);
  if (pirVal == HIGH)  {
    Serial.println(" ******   Movimiento Detectado ******** ");
  }
  else  {
    Serial.println("  ");
    
  }
  Firebase.setFloat("PIR",pirVal);

  int MagnetoVal = digitalRead(MagnetoPin);
  if (MagnetoVal == LOW) {
    Serial.println(" Puerta Cerrada ");
    digitalWrite(LED1,LOW);
  }
  else {
    Serial.println(" Puerta ABIERTA ");
    digitalWrite(LED1,HIGH);
  }
  Firebase.setFloat("Puerta",MagnetoVal);
  
   
  
 
 // inicia lectura de temperatura
 float h = dht.readHumidity();
 // Read temperature as Celsius (the default)
 float t = dht.readTemperature();
 // Read temperature as Fahrenheit (isFahrenheit = true)
 float f = dht.readTemperature(true);
 // Check if any reads failed and exit early (to try again).
 if (isnan(h) || isnan(t) ) {
              Serial.println("Failed to read from DHT sensor!");
              strcpy(celsiusTemp,"Failed");
              strcpy(fahrenheitTemp, "Failed");
              strcpy(humidityTemp, "Failed");  
              return;       
 }
 else{
              // Computes temperature values in Celsius + Fahrenheit and Humidity
              double hic = dht.computeHeatIndex(t, h, false);       
              dtostrf(hic, 6, 2, celsiusTemp);             
              double hif = dht.computeHeatIndex(f, h);
              dtostrf(hif, 6, 2, fahrenheitTemp);         
              dtostrf(h, 6, 2, humidityTemp);
              Serial.print("Temperatura en grados Celsius...:");            
              Serial.println(hic);
              Firebase.setFloat("Temperatura", hic);
              Firebase.setFloat("Humedad",h);
              // handle error
              if (Firebase.failed()) {
                  Serial.print("setting /number failed:");
                  Serial.println(Firebase.error());  
                  return;
              }
}
// fin lectura temperatura

//ledval=digitalRead(LED1);
//
//if(ledval==LOW){
//
// Serial.print("LED ");
// Serial.println(ledval);
//  digitalWrite(LED1,HIGH);
// Firebase.setFloat("LED1 ",ledval);
//}
//else{
//   Serial.print("LED ");
// Serial.println(ledval);
// digitalWrite(LED1,LOW);
//    Firebase.setFloat("LED1 ",ledval);
//}
delay(50);        // retraso entre lecturas para la estabilidad
}
