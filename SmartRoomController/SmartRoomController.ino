/*
 * Project: Smart Room Controller
 * Description: Reads data from humidity sensor to detect water leak.
 *      If leak detected, makes sound and flashes lights. Turns on fan
 *      to dry out the room.
 * Author: Cecilia Castillo
 * Date: 31-Jul-2020
 */

#include <SimpleDHT.h>
int pinDHT11 = A0;
SimpleDHT11 dht11;

int pinBuzzer = 15;
int pinButton = 23;
int k = 0;
unsigned long button_time = 0;  
unsigned long last_button_time = 0;
bool buzzerOn = true;

void setup() {
  pinMode(pinBuzzer, OUTPUT);
  //attachInterrupt(digitalPinToInterrupt(pinButton), buzzerInterrupt, FALLING);
  Serial.begin(9600);
}

void loop() {
  if(buzzerOn){
    playBuzzer();
    buzzerOn = false;
  } else {
    stopBuzzer();
  }

//  if(getHumidity() > humidityThreshold){
//    playBuzzer();
//    startFans();
//    flashLights();
//  } else {
//    stopAlerts();
//  }
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

void stopBuzzer(){
  noTone(pinBuzzer);
}

void startFans(){
  return;
}

void flashLights(){
  return;
}

void stopAlerts(){
  return;
}

int getHumidity(){
  byte temperature = 0;
  byte humidity = 0;
  byte data[40] = {0};

  //If returns anything other than zero, there was an error.
  //Note, this sensor frequently returns errors inside valid reads.
  if(dht11.read(pinDHT11, &temperature, &humidity, data)){
//    Serial.println("Read DHT11 failed");
    return 0;
  }
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
