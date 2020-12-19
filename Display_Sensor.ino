#include <Adafruit_ST7735.h>
#include <SPI.h>
#include <EEPROM.h>
#include "DHT.h"
#include "melody.h"
#include <NewTone.h>
#include "displayFunction.h"
#include "getDHT22.h"
#include "getMQ5.h"
#include "interruptFunction.h"

//pin definition
#define MQ5 A0
//#define POWER_STATE A1
//#define SOFTSERIAL_RX A2
//#define SOFTSERIAL_TX A3
#define PIR A4
#define DHTPIN A5
//#define SLEEP_SIM A6
#define LEDpin A7
#define BUTTON 2
//#define INT_SIM 3
#define R 4 
#define B 5
#define G 6
#define TFT_CS        7
#define TFT_RST        8 // Or set to -1 and connect to Arduino RESET pin
#define TFT_DC         9
#define DISPLAY_BACKLIGHT 10
#define TFT_MOSI 11  // Data out
#define BUZZER 12
#define TFT_SCLK 13  // Clock out

uint8_t calibrationTime = 5;

float t = 0;
int h = 0;
float tmax;
float tmin = 50.0;
uint8_t hmax;
uint8_t hmin = 100.0;

uint8_t k = 0;

uint8_t stato = 0;

void setup() {
  noInterrupts();
  PCICR |= 0b00000111; //abilito le porte B, C e D
  //PCMSK2 |= 0b00010000; // PCINT20 (D4)
  PCMSK1 |= 0b00010000; // PCINT12 (A4)
  //PCMSK1 |= 0b00010100; // PCINT12 (A4) e PCINT10 (A2)
  interrupts();

  beepOpen();

  Serial.begin(9600);
  attachInterrupt(digitalPinToInterrupt(BUTTON), intShow, FALLING);
  pinMode(MQ5, INPUT);
  //  pinMode(POWER_STATE, INPUT);
  pinMode(R, OUTPUT);
  pinMode(G, OUTPUT);
  pinMode(B, OUTPUT);
  //pinMode(DHTPIN, INPUT);
  //  pinMode(SLEEP_SIM, OUTPUT);
  pinMode(LEDpin, OUTPUT);
  pinMode(BUTTON, INPUT);
  //pinMode(INT_SIM, INPUT);
  pinMode(PIR, INPUT);
  //pinMode(SOFTSERIAL_RX, INPUT);
  //pinMode(SOFTSERIAL_TX, OUTPUT);
  //pinMode(TFT_DC, INPUT);
  pinMode(DISPLAY_BACKLIGHT, OUTPUT);
  //pinMode(TFT_MOSI, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  //pinMode(TFT_SCLK, OUTPUT);
  tft.initR(INITR_BLACKTAB);
  tft.setRotation(1);
  tft.fillScreen(ST77XX_BLACK);
  digitalWrite(LEDpin, LOW);
  digitalWrite(DISPLAY_BACKLIGHT, LOW);

  Serial.print("Calibrazione  sensore DHT22");
  for (int i = 0; i < calibrationTime; i++) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println(" Fatto");
  Serial.println("SENSORE DHT22 ATTIVO");
  delay(50);
  Serial.println("Avvio programma: ");
  dht.begin();
}

void loop() {
  float t = dht.readTemperature();
  int h = dht.readHumidity();

  if (show == true) {
    digitalWrite(DISPLAY_BACKLIGHT, HIGH);
    textTemp(t, tmax, tmin);
    textHum(h, hmax, hmin);
    tft.fillRect(0, 27, 160, 2, BLUE);
    tft.fillRect(0, 70, 160, 2, BLUE);
    tft.fillRect(0, 100, 160, 2, BLUE);
    Serial.print("sensVolt = "); Serial.print("\t"); Serial.print(sensor_volt); Serial.print("\t");
    Serial.print("RS_gas = "); Serial.print(RS_gas); Serial.print("\t");
    Serial.print("R0 = "); Serial.print("\t"); Serial.print(R0); Serial.print("\t");
    Serial.print("ratio = "); Serial.print(ratio); Serial.println("\t");
    Serial.println("");

    eraseGas();

    switch (j) {
      case 1:
        conversione(ratio, CO[0], CO[1]);             // Serial.println(" CO");
        break;
      case 2:
        conversione(ratio, Alcohol[0], Alcohol[1]);   // Serial.println(" Alcohol");
        break;
      case 3:
        conversione(ratio, H2[0], H2[1]);             // Serial.println(" H2");
        break;
      case 4:
        conversione(ratio, Metano[0], Metano[1]);     // Serial.println(" Metano");
        break;
      case 5:
        conversione(ratio, GPL[0], GPL[1]);           // Serial.println(" GPL");
        break;
      default:
        break;
    }
    j++;
    if (j > 5) {
      eraseGas();
      j = 1;
    }
    k++;

    if (k >= 10) {
      k = 0;
      show = false;
      //digitalWrite(B, LOW);
      digitalWrite(DISPLAY_BACKLIGHT, LOW);
      tft.fillScreen(ST77XX_BLACK);
    }
  }

  if (t > tmax) {
    tmax = t;
    EEPROM.write(0, t);
  }
  if (t < tmin) {
    tmin = t;
    EEPROM.write(4, t);
  }
  if (h > hmax) {
    hmax = h;
    EEPROM.write(8, h);
  }
  if (h < hmin) {
    hmin = h;
    EEPROM.write(10, h);
  }

  getGas(sensorValue, campioni);
  tStatus = gettStatus(t);
  hStatus = gethStatus(h);
  gStatus = getgStatus(ppm);
  stato = setStatus(tStatus, hStatus, gStatus);
  setLed(stato);
  setBuzzer(stato);
}

int setStatus(int tStatus, int hStatus, int gStatus) {
  int sum = tStatus + hStatus + gStatus;
  if (sum <= 1) {
    stato = 1;
  }
  if (sum > 1 && sum <= 2 ) {
    stato = 2;
  }
  if (sum >= 3) {
    stato = 3;
  }
  return stato;
}

void setLed(int stato) {
  switch (stato) {
    case 0: //stato zero
      digitalWrite(R, HIGH);
      digitalWrite(G, HIGH);
      digitalWrite(B, HIGH);
      break;
    case 1: //stato ok verde
      digitalWrite(R, LOW);
      digitalWrite(G, HIGH);
      digitalWrite(B, LOW);
      break;
    case 2: //stato a rischio blu
      digitalWrite(R, LOW);
      digitalWrite(G, LOW);
      digitalWrite(B, HIGH);
      break;
    case 3: //stato critico rosso
      digitalWrite(R, HIGH);
      digitalWrite(G, LOW);
      digitalWrite(B, LOW);
      break;
    default: //stato default
      digitalWrite(R, LOW);
      digitalWrite(G, LOW);
      digitalWrite(B, LOW);
      break;
  }
}

void setBuzzer(int stato) {
  switch (stato) {
    case 0: //stato zero
      break;
    case 1: //stato ok verde
      break;
    case 2: //stato a rischio blu
      beepOpen();
      break;
    case 3: //stato critico rosso
      break;
      beepWarning();
    default: //stato default
      break;
  }
}
