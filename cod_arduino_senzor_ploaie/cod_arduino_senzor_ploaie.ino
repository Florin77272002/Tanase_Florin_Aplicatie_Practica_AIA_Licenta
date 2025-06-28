#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const int pinSenzorPloaie = 36;
int valoareCitita = 0;
const int prag = 2000;

void setup() {
  Serial.begin(115200);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
}

void loop() {
  valoareCitita = analogRead(pinSenzorPloaie);
  Serial.print("Valoare senzor: ");
  Serial.println(valoareCitita);

  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(10, 20);

  if (valoareCitita < prag) {
    display.println("PLOUA");
  } else {
    display.println("NU PLOUA");
  }

  display.display();
  delay(1000);
}
