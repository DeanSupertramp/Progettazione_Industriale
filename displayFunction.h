#include <Adafruit_ST7735.h>
#include <SPI.h>

//pin definition
#define TFT_CS        7
#define TFT_RST        8 // Or set to -1 and connect to Arduino RESET pin
#define TFT_DC         9
#define TFT_MOSI 11  // Data out
#define TFT_SCLK 13  // Clock out

//color definition
#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);

void rect(int pos1, int pos2, int pos3, int pos4, int color) {
  tft.fillRect(pos1, pos2, pos3, pos4, color);
}

void textGas(String wGas, float percentuale) {
  tft.setCursor(5, 80);
  tft.setTextColor(YELLOW, BLACK);
  tft.setTextSize(2);
  tft.println(wGas);
  tft.setCursor(95, 80);
  tft.setTextColor(WHITE, BLACK);
  tft.setTextSize(2);
  tft.print(percentuale);
  tft.println("%");
}

void textTemp(float t, float tmax, float tmin) {
  tft.setCursor(10, 33);
  tft.setTextColor(YELLOW);
  tft.setTextSize(1);
  tft.println("Temperatura");
  tft.setCursor(5, 45);
  tft.setTextColor(WHITE, BLACK);
  tft.setTextSize(3);
  tft.println(t);

  tft.setCursor(5, 107);
  tft.setTextColor(YELLOW);
  tft.setTextSize(1);
  tft.println("T.Max");
  tft.setCursor(5, 120);
  tft.setTextColor(WHITE, BLACK);
  tft.setTextSize(1);
  tft.println(tmax);

  tft.setCursor(45, 107);
  tft.setTextColor(YELLOW);
  tft.setTextSize(1);
  tft.println("T.Min");
  tft.setCursor(45, 120);
  tft.setTextColor(WHITE, BLACK);
  tft.setTextSize(1);
  tft.println(tmin);
}

void textHum(int h, int hmax, int hmin) {
  tft.setCursor(110, 33);
  tft.setTextColor(YELLOW);
  tft.setTextSize(1);
  tft.println("Umidita'");
  tft.setCursor(115, 45);
  tft.setTextColor(WHITE, BLACK);
  tft.setTextSize(3);
  tft.println(h);

  tft.setCursor(90, 107);
  tft.setTextColor(YELLOW);
  tft.setTextSize(1);
  tft.println("H.Max");
  tft.setCursor(100, 120);
  tft.setTextColor(WHITE, BLACK);
  tft.setTextSize(1);
  tft.println(hmax);

  tft.setCursor(130, 107);
  tft.setTextColor(YELLOW);
  tft.setTextSize(1);
  tft.println("H.Min");
  tft.setCursor(140, 120);
  tft.setTextColor(WHITE, BLACK);
  tft.setTextSize(1);
  tft.println(hmin);
}

void eraseGas() {
  tft.setCursor(5, 80);
  tft.setTextColor(YELLOW, BLACK);
  tft.setTextSize(2);
  tft.println("        ");
  tft.setCursor(95, 80);
  tft.setTextColor(WHITE, BLACK);
  tft.setTextSize(2);
  tft.println("     ");
}
