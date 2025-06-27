#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <Adafruit_BME280.h>
#include <RTClib.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

Adafruit_BME280 bme;
RTC_DS3231 rtc;

// Pini
const int touchPin = 4;
const int pinPloaie = 35;
const int pinLDR = 34;

const int pragPloaie = 2000;
const int pragLumina = 2000;

int pagina = 0;
unsigned long lastTouch = 0;
const unsigned long debounce = 300;

void setup() {
  Serial.begin(115200);
  Wire.begin();

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();

  if (!bme.begin(0x76)) {
    Serial.println("BME280 nu a fost gasit!");
    while (1);
  }

  rtc.begin();
  if (rtc.lostPower()) {
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }

  pinMode(touchPin, INPUT);
  pinMode(pinPloaie, INPUT);
  pinMode(pinLDR, INPUT);
}

void loop() {
  if (digitalRead(touchPin) == HIGH && millis() - lastTouch > debounce) {
    pagina = (pagina + 1) % 6;
    lastTouch = millis();
  }

  display.clearDisplay();
  display.setTextColor(WHITE);

  if (pagina == 0) {
    // Pagina 0: STATIE METEO + DATA/ORA
    DateTime now = rtc.now();

    display.setTextSize(2);
    display.setCursor(0, 0);
    display.println("STATIE");
    display.setCursor(0, 20);
    display.println("METEO");

    display.setTextSize(1);
    display.setCursor(0, 45);
    if (now.day() < 10) display.print("0");
    display.print(now.day()); display.print("/");
    if (now.month() < 10) display.print("0");
    display.print(now.month()); display.print("/");
    display.print(now.year());

    display.setCursor(0, 56);
    if (now.hour() < 10) display.print("0");
    display.print(now.hour()); display.print(":");
    if (now.minute() < 10) display.print("0");
    display.print(now.minute());

  } else if (pagina == 1) {
    // Pagina 1: Temperatura
    float temp = bme.readTemperature();
    display.setTextSize(2);
    display.setCursor(0, 20);
    display.println("Temp:");
    display.setCursor(0, 45);
    display.print(temp, 1); display.println(" C");

  } else if (pagina == 2) {
    // Pagina 2: Umiditate
    float hum = bme.readHumidity();
    display.setTextSize(2);
    display.setCursor(0, 20);
    display.println("Umid:");
    display.setCursor(0, 45);
    display.print(hum, 1); display.println(" %");

  } else if (pagina == 3) {
    // Pagina 3: Presiune
    float pres = bme.readPressure() / 100.0F;
    display.setTextSize(2);
    display.setCursor(0, 20);
    display.println("Pres:");
    display.setCursor(0, 45);
    display.print(pres, 0); display.println(" hPa");

  } else if (pagina == 4) {
    // Pagina 4: Ploaie
    int val = analogRead(pinPloaie);
    display.setTextSize(2);
    display.setCursor(0, 30);
    if (val < pragPloaie) {
      display.println("PLOUA");
    } else {
      display.println("NU PLOUA");
    }

  } else if (pagina == 5) {
    // Pagina 5: Lumina
    int lum = analogRead(pinLDR);
    display.setTextSize(2);
    display.setCursor(0, 30);
    if (lum < pragLumina) {
      display.println("INTUNERIC");
    } else {
      display.println("LUMINA");
    }
  }

  display.display();
  delay(100);
}
