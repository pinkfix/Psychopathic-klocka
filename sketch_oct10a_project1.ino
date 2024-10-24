/*
* Name: clock and temp project
* Author: Victor Huke
* Date: 2024-10-10
* Description: This project uses a ds3231 to measure time and displays the time to an 1306 oled display,
* Further, it measures temprature with a analog temprature module and displays a mapped value to a 9g-servo-motor
*/

// Include Libraries
#include <Servo.h>
#include <RTClib.h>
#include <Wire.h>
#include "U8glib.h"
#include "LedControl.h"

// Init constants
const int temppin = A0;
const int servopin = 8;
unsigned long delaytime = 100;
// Init global variables

// Init Scuffed shajt i need but dont understand and cant categorize
RTC_DS3231 rtc;
U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NO_ACK);
Servo myservo;
LedControl lc=LedControl(12,13,10,2);


void setup() {
  // init communication
  Serial.begin(9600);
  Wire.begin();

  rtc.begin();
  // Init Hardware
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  u8g.setFont(u8g_font_fub17n);
  myservo.attach(servopin);

  // 7SD Hardware
  lc.shutdown(0,false);
  lc.setIntensity(0,8);
  lc.clearDisplay(0);

}

void loop() {

  oledWrite(getTime()); 
  servoWrite(getTemp());
  writeMarkersonSSD();
  writeHourOnSSD(getHours());
  writeMinOnSSD(getMins());
  writeSecOnSSD(getSecs());
  Serial.print(getTime());
  Serial.print(" ");
  Serial.println(getTemp());
  delay(1000);
}


/*
*This function reads time from an ds3231 module and package the time as a String
*Parameters: Void
*Returns: time in hh:mm:ss as String
*/
String getTime() {
  DateTime now = rtc.now();
  return String(now.hour()) + ":" + String(now.minute()) + ":" + String(now.second());
}

 int getHours(){
  DateTime now = rtc.now();
  return now.hour();
}

int getMins(){
  DateTime now = rtc.now();
  return now.minute();
}

int getSecs(){
  DateTime now = rtc.now();
  return now.second();
}

/*
* This function reads an analog pin connected to an analog temprature sensor and calculates the corresponding temp
*Parameters: Void
*Returns: temprature as float
*/
float getTemp() {
  float temp = 0;
  float R1 = 10000;  // value of R1 on board
  float logR2, R2, T;
  float c1 = 0.001129148, c2 = 0.000234125, c3 = 0.0000000876741;  //steinhart-hart coeficients for thermistor

  int Vo = analogRead(temppin);
  R2 = R1 * (1023.0 / (float)Vo - 1.0);  //calculate resistance on thermistor
  logR2 = log(R2);
  temp = (1.0 / (c1 + c2 * logR2 + c3 * logR2 * logR2 * logR2));  // temperature in Kelvin
  temp = temp - 273.15;                                           //convert Kelvin to Celcius

  return temp;
}

/*
* This function takes a string and draws it to an oled display
*Parameters: - text: String to write to display
*Returns: void
*/
void oledWrite(String text) {
  u8g.firstPage();

  do {
    u8g.drawStr(((128 - u8g.drawStr(-10,-10,text.c_str()))/2), 42, text.c_str());
  } while (u8g.nextPage());
}
//
/*
* takes a temprature value and maps it to corresppnding degree on a servo
*Parameters: - value: temprature
*Returns: void
*/
void servoWrite(float value) {
  if (0 <= value <= 40) {
    myservo.write(value*4);
  } else {
    myservo.write(0);
  }
}

void writeMarkersonSSD(){
  lc.setRow(0,2,1);
  lc.setRow(0,5,1);
}

void writeHourOnSSD(int time){
  int ones;
  int tens;
   ones=time%10;  
    time=time/10;  
    tens=time%10;  
  lc.setDigit(0, 7, (byte)tens,false);
  lc.setDigit(0, 6, (byte)ones,false);
}

void writeMinOnSSD(int time){
  int ones;
  int tens;
   ones=time%10;  
    time=time/10;  
    tens=time%10;  
  lc.setDigit(0, 4, (byte)tens,false);
  lc.setDigit(0, 3, (byte)ones,false);
}

void writeSecOnSSD(int time){
  int ones;
  int tens;
   ones=time%10;
    time=time/10;
    tens=time%10;
  lc.setDigit(0, 1, (byte)tens,false);
  lc.setDigit(0, 0, (byte)ones,false);
}
