/*  blynk_PanasonicAC
 *  Author: Eason Chai
    Github: https://github.com/easonchai/blynk_PanasonicAC
    Latest Update: V1.4 (17 Sep. 2019)
*/
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
boolean autoOn = false; //Automatically turns on the AC
float autoOnTemp = 27.0; //Default

BLYNK_WRITE(V3){
  pressButton();
}

BLYNK_WRITE(V4){ //Reset Button
  if(param.asInt() == HIGH){ //Only activate this once on button press
    if(acOn){
      acOn = false;
      Blynk.virtualWrite(V3, LOW);
    } else {
      acOn = true;
      Blynk.virtualWrite(V3, HIGH);
    }
  }
}

BLYNK_WRITE(V5){ //Auto On AC button
  if(autoOn){
    autoOn = false;
  } else {
    autoOn = true;
  }
}

BLYNK_WRITE(V7){ //Step widget
  autoOnTemp = param.asFloat(); //Updates the autoOnTemp with widget input
}

void setup(){
  // Debug console
  Serial.begin(9600);

  Blynk.begin(auth, ssid, pass);
  Blynk.virtualWrite(V7, autoOnTemp); //Make sure app is updated to default temp

  //External modules
  servo.attach(5); //Servo
  pinMode(touchPin, INPUT); //Touch
  dht.begin();
  timer.setInterval(10000L, sendTemp);
}

void loop(){
  Blynk.run();
  timer.run();
  if(digitalRead(touchPin) == 1){ //When touched
    changeState();
    delay(300);
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
  Blynk.syncVirtual(V3); //Emulate a press

  //Whenever user activates the AirConditioner, or autoMode triggers, the auto mode is instantly disabled
  autoOn = false;
}

void pressButton(){
  servo.write(180);
  delay(200);
  servo.write(90);
}

void sendTemp(){
 float t = dht.readTemperature(); 
 Blynk.virtualWrite(V6, t); // Temperature for gauge
 Blynk.virtualWrite(V8, t); // Temperature for graph
 if(autoOn){ //If autoOn is true, then send the temperature to autoMode
  autoMode(t);
 } //Else nothing
}

void autoMode(float temperature){
  /*Serial.println("Mode ON");
  Serial.println(autoOnTemp);
   Serial.print("Auto On Mode: ");
    Serial.println(autoOn); DEBUG ONLY*/
   if(!acOn){ //Since AC not on yet, this mode can be activated
     if(temperature >= autoOnTemp){ //Turn on ac
      changeState();
      Blynk.virtualWrite(V5, LOW);
      Blynk.virtualWrite(V3, HIGH); //Press ON
    }
  }
}
