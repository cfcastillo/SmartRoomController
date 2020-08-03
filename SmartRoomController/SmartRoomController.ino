/*
 * Project: Smart Room Controller
 * Description: Reads data from humidity sensor to detect water leak.
 *      If leak detected, makes sound and flashes lights. Turns on fan
 *      to dry out the room.
 * Author: Cecilia Castillo
 * Date: 31-Jul-2020
 */
#include <Ethernet.h>
#include <OneButton.h>
#include "mac.h"
#include "hue.h"
#include "Wemo.h"

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
const int SCREEN_WIDTH = 128; // OLED display width, in pixels
const int SCREEN_HEIGHT = 32; // OLED display height, in pixels
// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
const int OLED_RESET = 4; // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#include <SimpleDHT.h>
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
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x32
    Serial.println(F("SSD1306 allocation failed"));
  }
  display.clearDisplay();
  display.display();
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
    for(int i = 1; i < 5; i++){
      setHue(i, false, 0, 0);
    }
    
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

  //print humidity to OLED
  displayHumidity(lastGoodHumidity);
  
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

void displayHumidity(int value){
  display.clearDisplay();
  //0 - default - landscape to top
  //1 - portrait to right
  //2 - landscape to bottom
  //3 - portrait to left
  display.setRotation(0);
  display.setTextSize(2);             // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);        // Draw white text
  display.setCursor(0,0); 
  display.printf("Humidity: %i", value);
  display.display();
}
