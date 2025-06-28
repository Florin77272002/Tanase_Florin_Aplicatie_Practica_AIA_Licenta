#include <Wire.h>
#include <RTClib.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <Adafruit_BME280.h>
#include <LiquidCrystal_I2C.h>
#include <WiFi.h>
#include <WebServer.h>
#include "htmlpage.h"

Adafruit_SSD1306 display(128, 64, &Wire, -1);
Adafruit_BME280 bme;
RTC_DS3231 rtc;
LiquidCrystal_I2C lcd(0x27, 16, 2);

const char* ssid = "StatieMeteo";
const char* password = "12345678";
WebServer server(80);

const int touchPin = 4;
const int pinPloaie = 35;
const int pinLDR = 34;
const int pragPloaie = 2000;
const int pragNoapte = 1500;
const int pragLuminaSlaba = 3000;

int pagina = 0;
unsigned long lastTouch = 0;
const unsigned long debounce = 300;

void setup() {
  Wire.begin();
  lcd.begin(16, 2);
  lcd.backlight();
  rtc.begin();

   //rtc.adjust(DateTime(2025, 6, 28, 13, 31, 10)); 

  if (rtc.lostPower()) rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  bme.begin(0x76);
  pinMode(touchPin, INPUT);
  pinMode(pinPloaie, INPUT);
  pinMode(pinLDR, INPUT);

  WiFi.softAP(ssid, password);
  IPAddress IP = WiFi.softAPIP();
  Serial.begin(115200);
  Serial.print("Accesează: http://"); Serial.println(IP);

  server.on("/", []() {
    server.send(200, "text/html", htmlPage);
  });

  server.on("/valori", []() {
    DateTime now = rtc.now();
    float t = bme.readTemperature();
    float h = bme.readHumidity();
    float p = bme.readPressure() / 100.0F;
    int pl = analogRead(pinPloaie);
    int lum = analogRead(pinLDR);
    String json = "{";
    json += "\"temperatura\":" + String(t, 1) + ",";
    json += "\"umiditate\":" + String(h, 1) + ",";
    json += "\"presiune\":" + String(p, 0) + ",";
    json += "\"ploaie\":" + String(pl < pragPloaie ? "true" : "false") + ",";
    json += "\"lumina\":" + String(lum > pragLuminaSlaba ? "true" : "false") + ",";
    json += "\"ora\":\"" + zfill(now.hour()) + ":" + zfill(now.minute()) + "\"";
    json += "}";
    server.send(200, "application/json", json);
  });
  server.begin();
}

void loop() {
  server.handleClient();
  DateTime now = rtc.now();
  lcd.setCursor(0, 0);
  lcd.print("STATIE METEO  ");
  lcd.setCursor(0, 1);
  if (now.day() < 10) lcd.print("0"); lcd.print(now.day()); lcd.print("/");
  if (now.month() < 10) lcd.print("0"); lcd.print(now.month()); lcd.print(" ");
  if (now.hour() < 10) lcd.print("0"); lcd.print(now.hour()); lcd.print(":");
  if (now.minute() < 10) lcd.print("0"); lcd.print(now.minute());

  if (digitalRead(touchPin) == HIGH && (millis() - lastTouch > debounce)) {
    pagina = (pagina + 1) % 5;
    lastTouch = millis();
  }

  display.clearDisplay();
  display.setTextColor(WHITE);

  if (pagina == 0) {
    float temp = bme.readTemperature();
    display.setTextSize(2);
    display.setCursor(0, 20);
    display.println("Temp:");
    display.setCursor(0, 45);
    display.print(temp, 1); display.println(" C");
  } else if (pagina == 1) {
    float hum = bme.readHumidity();
    display.setTextSize(2);
    display.setCursor(0, 20);
    display.println("Umid:");
    display.setCursor(0, 45);
    display.print(hum, 1); display.println(" %");
  } else if (pagina == 2) {
    float pres = bme.readPressure() / 100.0F;
    display.setTextSize(2);
    display.setCursor(0, 20);
    display.println("Pres:");
    display.setCursor(0, 45);
    display.print(pres, 0); display.println("hPa");
  } else if (pagina == 3) {
    int val = analogRead(pinPloaie);
    display.setTextSize(2);
    display.setCursor(0, 30);
    display.println(val < pragPloaie ? "PLOUA" : "NU PLOUA");
  } else if (pagina == 4) {
    int lum = analogRead(pinLDR);
    display.setTextColor(WHITE);

    display.setTextSize(2);  
    display.setCursor(25, 0);  

    if (lum < pragNoapte) {
      display.println("NOAPTE");
    } else {
      display.println("ZI");
    }

    display.setTextSize(1);  
    display.setCursor(0, 50);  

    if (lum < pragNoapte) {
    } else if (lum < pragLuminaSlaba) {
      display.println("LUMINA SLABA");
    } else {
      display.println("LUMINA PUTERNICA");
    }
  }

  display.display();
  delay(100);
}

String zfill(int val) {
  return (val < 10) ? "0" + String(val) : String(val);
}
