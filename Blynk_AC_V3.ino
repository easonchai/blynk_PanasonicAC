/* Comment this out to disable prints and save space */
//#define BLYNK_PRINT Serial
const int touchPin = 4;
const int DHTPIN = 2;
const int DHTTYPE = 11;

#include <SPI.h>;
#include <ESP8266WiFi.h>;
#include <BlynkSimpleEsp8266.h>;
#include <SimpleTimer.h>;
#include <DHT.h>;
#include <Servo.h>
#include <Adafruit_Sensor.h>

// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "Fj7FrKYD0OJsPeIOAzByEnEqApmJMLcr";

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "bschai@unifi";
char pass[] = "JHchai2905";

Servo servo;
DHT dht(DHTPIN, DHTTYPE);
SimpleTimer timer;

//Stores state
boolean acOn = false; //AC is off at first (This will be handled by the pushButton() method)

BLYNK_WRITE(V3)
{
  pressButton();
}

void setup()
{
  // Debug console
  Serial.begin(9600);

  Blynk.begin(auth, ssid, pass);
  // You can also specify server:
  //Blynk.begin(auth, ssid, pass, "blynk-cloud.com", 80);
  //Blynk.begin(auth, ssid, pass, IPAddress(192,168,1,100), 8080);

  //External modules
  servo.attach(5); //Servo
  pinMode(touchPin, INPUT); //Touch
  dht.begin();
  timer.setInterval(15000L, sendTemp);
}

void loop()
{
  Blynk.run();
  timer.run();
  if(digitalRead(touchPin) == 1){ //When touched
    changeState();
    delay(500);
  } //Else nothing
}

void changeState(){
  if(acOn){ //If on, turn to off
    //Need to change two variables (Boolean variable and virtual button)
    acOn = false;
    Blynk.virtualWrite(V3, LOW);
  } else {
    acOn = true;
    Blynk.virtualWrite(V3, HIGH);
  }
  Blynk.syncVirtual(V3); //Sync with the app
}

void pressButton(){
  servo.write(180);
  delay(200);
  servo.write(90);
}

void sendTemp(){
 float t = dht.readTemperature(); // or dht.readTemperature(true) for Fahrenheit
 // You can send any value at any time.
 // Please don't send more that 10 values per second.
 //Blynk.virtualWrite(V5, h); // Humidity for gauge
 Blynk.virtualWrite(V6, t); // Temperature for gauge
 //Blynk.virtualWrite(V7, h); // Humidity for graph
 Blynk.virtualWrite(V8, t); // Temperature for graph
}
