#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const int pinLDR = 34;
const int pragNoapte = 1500;      // Sub această valoare e NOAPTE (lanterna stinsă)
const int pragLuminaSlaba = 3000; // Între pragNoapte și pragLuminaSlaba = lumină slabă
// Peste pragLuminaSlaba = lumină puternică

void setup() {
  Serial.begin(115200);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
}

void loop() {
  int lum = analogRead(pinLDR);
  Serial.print("Valoare LDR: "); Serial.println(lum);

  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);

  if (lum < pragNoapte) {
    display.setCursor(10, 20);
    display.println("NOAPTE");
  } else {
    display.setCursor(30, 10);
    display.println("ZI");
    display.setTextSize(1);
    display.setCursor(20, 40);

    if (lum < pragLuminaSlaba) {
      display.println("LUMINA SLABA");
    } else {
      display.println("LUMINA PUTERNICA");
    }
  }
  display.display();
  delay(500);
}
