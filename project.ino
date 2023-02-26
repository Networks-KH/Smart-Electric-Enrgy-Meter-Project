//#include <LiquidCrystal.h>
#include <LiquidCrystal_I2C.h>
//LiquidCrystal lcd(13, 12, 14, 27, 26, 25);
#define BLYNK_PRINT Serial
#include "EmonLib.h"
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
LiquidCrystal_I2C lcd(0x3F, 16, 2);
EnergyMonitor emon;
#define vCalibration 83.3   //
#define currCalibration 0.50  //
BlynkTimer timer;
char auth[] = "tXVq_VqvrHFbHZEHm5OtX6Q3nLj8c0FR"; //Your auth token from email
char ssid[] = "Khalid";    //Type your WiFi name
char pass[] = "12345678";    //Type your WiFi password
float pwr;
float Tpwr;
float sval;
float ival;
float kWh = 0;
float cost = 0;
unsigned long lastmillis = millis();//
void myTimerEvent()
{
  float irmsReading = IrmsSensor();
  float pwrReading = PowerSensor();
  emon.calcVI(20, 2000);
  kWh = kWh + pwrReading * (millis() - lastmillis)/ 3600000000.0 ;  // Convert milli second to hour divided by 3600000, And divided by 1000 to convert watt to killo watt
  cost = kWh*0.18;
  yield();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Vrms:");
  Serial.print("Vrms: ");
  Serial.println(emon.Vrms, 2);
  Serial.print("Time: ");
  Serial.println(millis());
  Serial.print("Irms: ");
  Serial.println(irmsReading, 4);
  Serial.print("Power: ");
  Serial.println(pwrReading);
  Serial.print("Cost: ");
  Serial.println(cost);
  lcd.print(emon.Vrms, 2);
  lcd.print("V");
  lcd.setCursor(0, 1);
  lcd.print("Irms:");
  lcd.print(irmsReading, 4);
  lcd.print("A");
  delay(3000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Power:");
  lcd.print(pwrReading, 4);
  lcd.print("W");
  lcd.setCursor(0, 1);
  lcd.print("kWh:");
  lcd.print(kWh, 4);
  lcd.print("W");
   delay(3000);
   lcd.clear();
   lcd.setCursor(0, 0);
  lcd.print("cost:");
  lcd.print(cost, 4);
  lcd.print("R");
  lastmillis = millis();
  Blynk.virtualWrite(V0, emon.Vrms);
  Blynk.virtualWrite(V1, pwrReading);
  Blynk.virtualWrite(V2, irmsReading);
  Blynk.virtualWrite(V3, kWh);
  Blynk.virtualWrite(V4, cost);
}
void setup()
{
//  lcd.begin(16, 2);
  //lcd.init(); 
  Serial.begin(115200);
  lcd.begin();                     
  lcd.backlight();
   Blynk.begin(auth, ssid, pass,"Blynk.cloud",80);
  emon.voltage(35, vCalibration, 1.7); // Voltage: input pin, calibration, phase_shift
  emon.current(34, currCalibration);    // Current: input pin, calibration.
  timer.setInterval(5000L, myTimerEvent);
  lcd.setCursor(3, 0);
  lcd.print("Smart Energy");
  delay(1000);
  lcd.setCursor(5, 1);
  lcd.print("Monitor");
  delay(500);
  lcd.clear();
}
void loop()
{
  Blynk.run();
  timer.run(); 
}
float PowerSensor(){
int i;
pwr = 0;
Tpwr = Tpwr + 0;
for (i = 0; i < 5; i++){
  pwr = emon.apparentPower;
  Tpwr = Tpwr + pwr;
  delay (500);
  Blynk.run();
  }
   return (Tpwr/5);
}
 // Read sensor level
  float IrmsSensor(){
  int i;
  sval = 0;
  ival = 0;
  for (i = 0; i < 5; i++){
    ival = emon.Irms;
    sval = sval + ival;
    delay (500);
    Blynk.run();
     if (sval>3)
     {
    return (sval+4.5);
  } 
  }
  return (sval/5);
  }
