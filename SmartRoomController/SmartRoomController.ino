/*
 * Project: Smart Room Controller
 * Description: Reads data from humidity sensor to detect water leak.
 *      If leak detected, makes sound and flashes lights. Turns on fan
 *      to dry out the room.
 * Author: Cecilia Castillo
 * Date: 31-Jul-2020
 */
#include <Ethernet.h>
#include "OneButton.h"
#include "mac.h"
#include "hue.h"
#include "Wemo.h"

#include "SimpleDHT.h"
int pinDHT11 = A0;
SimpleDHT11 dht11;

int pinBuzzer = 15;
int pinButton = 23;
int fans[] = {2, 3}; //fans are on wemos 2 and 3
int fansOn = false; //state variable so don't keep trying to start fans.
bool alertOn = false;
int humidityThreshold = 75;
IPAddress ip(192,168,1,20);  // My Teensy IP
Wemo myWemo;

void setup() {
  Serial.begin(9600);
  Ethernet.begin(mac,ip);
  delay(2000);              // Wait for Serial Monitor
  Serial.print("LinkStatus: ");
  Serial.println(Ethernet.linkStatus());
  pinMode(pinBuzzer, OUTPUT);
}

void loop() {
  if(getHumidity() > humidityThreshold){
    alertOn = true;
    playBuzzer();
    startFans();
    flashLights();
  } else {
    stopAlerts();
  }
}

void playBuzzer(){
  //arg 1, pin number
  //arg 2, frequency in Hz
  //arg 3, time period to play for each iteration
  int tones[] = {500, 1000, 1500, 2000};
  for (int i = 0; i < 4; i++){
    tone(pinBuzzer, tones[i]);
    delay(500);
  }
}

void startFans(){
  //todo: read length of fans array and use for loop.
  if (!fansOn){
    fansOn = true;
  
    Serial.println("turned fans ON");
    myWemo.switchON(fans[0]);
    myWemo.switchON(fans[1]);
  }
}

void flashLights(){
  for(int i = 1; i < 5; i++){
    setHue(i, true, HueRed, 200);
  }
  delay(1000);
  for(int i = 1; i < 5; i++){
    setHue(i, false, 0, 0);
  }
}

void stopAlerts(){
  //Only run setHue once when stopping alerts.
  if(alertOn){
    alertOn = false;
    
    //buzzer off.
    noTone(pinBuzzer);
    
    //lights off.
//    for(int i = 1; i < 5; i++){
//      setHue(i, false, 0, 0);
//    }
    
    //fans off.
    fansOn = false;
    Serial.println("turned OFF fan");
    myWemo.switchOFF(fans[0]);
    myWemo.switchOFF(fans[1]);
  }
}

int getHumidity(){
  byte temperature = 0;
  byte humidity = 0;
  byte data[40] = {0};
  static int lastGoodHumidity;  //hack. every 3 or 4 readings fail.

  //If returns anything other than zero, there was an error.
  //Note, this sensor frequently returns errors inside valid reads.
  if(dht11.read(pinDHT11, &temperature, &humidity, data)){
//    Serial.println("Read DHT11 failed");
    return lastGoodHumidity;
  }

  lastGoodHumidity = (int)humidity;
  
  //prints byte data
  for (int i=0; i<40; i++){
    Serial.print((int)data[i]);
    if(i>0 && ((i+1)%4) ==0) {
      Serial.print(' ');
    }
  }
  Serial.println("");

  //prints readings
  Serial.print("Sample OK: ");
  Serial.print((int)temperature);
  Serial.print(" *C, ");
  Serial.print((int)humidity);
  Serial.println(" %");

  //delay is needed because the DHT sampling rate is 1Hz. 
  delay(1000);

  //unfortunately values are integers only.
  return humidity;

}
