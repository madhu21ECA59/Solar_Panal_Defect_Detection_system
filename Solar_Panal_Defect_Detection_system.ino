#include <WiFi.h>
#include <HTTPClient.h>
#include <LiquidCrystal_I2C.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include "DHT.h"
#include <GP2Y1010AU0F.h>
#include <Wire.h>
#include <BH1750.h>
#include <Adafruit_INA219.h>
const char* ssid = "iot";
const char* password = "qwertyuiop!";
const char* tsServer = "http://api.thingspeak.com";
const char* key1 = "Q4OY5UQY8KF9TGKI";
const char* key2 = "983AF7004DIL4NQN";
DHT dht(19, DHT22);
OneWire oneWire(15);
DallasTemperature sensors(&oneWire);
BH1750 lightMeter;
Adafruit_INA219 ina219;
GP2Y1010AU0F dustSensor(12, 33);
LiquidCrystal_I2C lcd(0x27, 16, 2);
int buz = 4, tv, hv, stv, lv;
float dv = 0, vv, iv, pv, efficiency = 0, fault_probability = 0;
String fault_message = "System Normal";
unsigned long lastTime = 0;
float mf(float x, float a, float b, float c = -1) {
  if (c == -1) return x <= a ? 1 : x >= b ? 0 : (b - x) / (b - a);
  if (x <= a || x >= c) return 0;
  if (x <= b) return (x - a) / (b - a);
  return (c - x) / (c - b);
}
void setup() {
  Serial.begin(9600);
 pinMode(buz, OUTPUT);
  dht.begin();
 sensors.begin();
 dustSensor.begin(); Wire.begin(); lightMeter.begin(); ina219.begin();
  lcd.init(); lcd.backlight(); WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) delay(500);
}
void loop() {
  readSensors(); fuzzyLogic();
  if (millis() - lastTime > 20000) {
    if (tv > -40 && tv < 126 && hv > 20 && hv < 81) sendToThingSpeak();
    lastTime = millis();
  }
void readSensors() {
  hv = dht.readHumidity(); tv = dht.readTemperature();
  sensors.requestTemperatures(); stv = sensors.getTempCByIndex(0);
  dv = max(20, dustSensor.read() / 10.0); lv = lightMeter.readLightLevel();
  iv = ina219.getCurrent_mA();
  if (iv > 100) {
    vv = ina219.getBusVoltage_V(); pv = ina219.getPower_mW() / 1000.0;
    pv = constrain(pv, 0, 5);
  } else vv = iv = pv = 0;
  calcEfficiency();
}
void calcEfficiency() {
  float irradiance = lv / 100.0, area = 0.053;
  float maxPower = constrain(irradiance * area, 0, 5);
  efficiency = maxPower > 0 ? (pv / maxPower) * 100.0 : 0;
  if (stv > 45) efficiency *= 0.95;
  if (dv > 150) efficiency *= 0.90;
  if (hv > 80) efficiency *= 0.98;
}
void fuzzyLogic() {
  float r1 = min(mf(stv, 45, 65), mf(pv, 1, 3));
  float r2 = min(mf(dv, 150, 250), mf(pv, 1, 3));
  float r3 = min(mf(stv, 25, 45, 65), mf(dv, 50, 150, 250));
  float r4 = min(mf(lv, 10000, 20000), mf(pv, 0.5, 1.0));
  fault_probability = max(max(r1, max(r2, r3)), r4) * 100;
  if (r4 > 0.7) { fault_message = "DUST ACCUMULATION!"; buzz(2, 200); }
  else if (fault_probability > 70) { fault_message = "CRITICAL FAULT!"; digitalWrite(buz, HIGH); }
  else if (fault_probability > 40) { fault_message = "Warning"; buzz(1, 500); }
  else { fault_message = "System Normal"; digitalWrite(buz, LOW); }
  lcd.clear(); lcd.setCursor(0, 0);
  lcd.print("Fault:"); lcd.print(fault_probability, 0); lcd.print("%");
  lcd.setCursor(0, 1); lcd.print(fault_message);
  delay(2000);
}
void buzz(int times, int delayTime) {
  for (int i = 0; i < times; i++) {
    digitalWrite(buz, HIGH); delay(delayTime);
    digitalWrite(buz, LOW); delay(delayTime);
  }
void sendToThingSpeak() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String url1 = String(tsServer) + "/update?api_key=" + key1 + 
                  "&field1=" + tv + "&field2=" + hv + "&field3=" + stv +
                  "&field4=" + dv + "&field5=" + lv + "&field6=" + vv +
                  "&field7=" + iv + "&field8=" + pv;
    http.begin(url1); http.GET(); http.end();
    String url2 = String(tsServer) + "/update?api_key=" + key2 + 
                  "&field1=" + efficiency + "&field2=" + fault_probability +
                  "&field3=" + (fault_message == "DUST ACCUMULATION!") +
                  "&field4=" + (fault_message == "CRITICAL FAULT!") +
                  "&field5=" + (fault_message == "Warning");
    http.begin(url2); http.GET(); http.end();
  }
