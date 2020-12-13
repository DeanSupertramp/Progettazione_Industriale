#include <Adafruit_ST7735.h>
#include <SPI.h>
#include <EEPROM.h>
#include "DHT.h"

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

#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

#define DHTTYPE DHT22

int calibrationTime = 5;

volatile bool show = false;

float t = 0;
int h = 0;
float t_mean = 0;
float h_mean = 0;
float tmax;
float tmin = 50.0;
int hmax;
int hmin = 100.0;

float R0 = 16981.25; //Resistenza ricavata dalla fase di calibrazione
float sensorValue = 0.0;
float sensor_volt = 0.0;
int campioni = 30;
int i = 0;
int j = 1;
int k = 0;

float Vout = 0.0;
float RS_gas = 0.0;
float ratio = 0.0;
double ppm_log = 0;
double ppm = 0;
float percentuale = 0;

float CO[2] =      { -0.1547, 0.9143};    //data format:{ m, q};
float Alcohol[2] = { -0.2617, 1.113};     //data format:{ m, q};
float H2[2] =      { -0.2428, 0.7474};    //data format:{ m, q};
float Metano[2] =  { -0.3958, 0.8973};    //data format:{ m, q};
float GPL[2] =     { -0.4132, 0.7928};    //data format:{ m, q};
String GAS[6] = {"undefined", "CO", "Alcohol", "H2", "Metano", "GPL"};

int stato = 0;
int tStatus = 0;
int hStatus = 0;
int gStatus = 0;

DHT dht(DHTPIN, DHTTYPE);
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);

void setup() {
  noInterrupts();
  PCICR |= 0b00000111; //abilito le porte B, C e D
  //PCMSK2 |= 0b00010000; // PCINT20 (D4)
  PCMSK1 |= 0b00010000; // PCINT12 (A4)
  //PCMSK1 |= 0b00010100; // PCINT12 (A4) e PCINT10 (A2)
  interrupts();

  beep(50);
  beep(50);

  Serial.begin(9600);
  attachInterrupt(digitalPinToInterrupt(BUTTON), intShow, RISING);
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
    //digitalWrite(B, HIGH);
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

  Serial.println("");
  Serial.println("##############################################################################");
  Serial.println("");

  getGas(sensorValue, campioni);

  tStatus = gettStatus(t);
  hStatus = gethStatus(h);
  gStatus = getgStatus(ppm);

  stato = setStatus(tStatus, hStatus, gStatus);
  setLed(stato);

  // QUI QUI QUI QUI QUI QUI QUI QUI QUI QUI QUI QUI QUI QUI QUI QUI QUI QUI QUI QUI QUI QUI QUI QUI QUI QUI QUI QUI QUI QUI QUI QUI QUI QUI QUI
  //  if (digitalRead(PIR) == HIGH) {
  //    Serial.println("MOVIMENTO");
  //    intShow();
  //  }
  // QUI QUI QUI QUI QUI QUI QUI QUI QUI QUI QUI QUI QUI QUI QUI QUI QUI QUI QUI QUI QUI QUI QUI QUI QUI QUI QUI QUI QUI QUI QUI QUI QUI QUI QUI

}


void getGas(float sensorValue, int campioni) {
  for (i = 0 ; i < campioni ; i++) {
    sensorValue += analogRead(MQ5);
    delay(10);
  }
  sensorValue = sensorValue / campioni;
  sensor_volt = sensorValue / 1024 * 5;
  RS_gas = (5.0 - sensor_volt) * 10000 / sensor_volt;
  ratio = RS_gas / R0;
}

void conversione(float ratio, float m, float q) {
  Serial.println("-------------------------------------------------------------------------------");
  String wGas = GAS[j];
  ppm_log = (log10(ratio) - q) / m;
  ppm = pow(10, ppm_log);
  percentuale = ppm / 10000; // *100 / 1 milione
  Serial.print("GAS = ");
  Serial.println(wGas);
  if (ppm > 1000) {
    Serial.print(" Attenzione, "); Serial.print(wGas); Serial.println(" in eccesso!!!");
  }
  Serial.print("ppm_log = "); Serial.print("\t"); Serial.print(ppm_log); Serial.print("\t");
  Serial.print("ppm = "); Serial.print("\t"); Serial.print(ppm); Serial.print("\t");
  Serial.print("percentuale = "); Serial.print("\t"); Serial.print(percentuale); Serial.print("\t"); Serial.print("%");
  ////////////////////////////////////////////////////////////////////////
  textGas(wGas, percentuale);
  ////////////////////////////////////////////////////////////////////////
}

void rect(int pos1, int pos2, int pos3, int pos4, int color) {
  tft.fillRect(pos1, pos2, pos3, pos4, color);
}



int gettStatus(float t) {
  if (t > 18.0 && t < 22.0) {
    tStatus = 0;
  } else  if ((t > 15.0 && t < 18.0) || (t > 22 && t < 30)) {
    tStatus = 1;
  } else {
    tStatus = 2;
  }
  Serial.print("tStatus = "); Serial.println(tStatus);
  return tStatus;
}

int gethStatus(int h) {
  if (h > 40 && h < 60) {
    hStatus = 0;
  } else  if ((h > 30 && h < 75) || (h > 75 && h < 80)) {
    hStatus = 1;
  } else {
    hStatus = 2;
  }
  Serial.print("hStatus = "); Serial.println(hStatus);
  return hStatus;
}


int getgStatus(float ppm) {
  if (ppm > 2000.0) {
    gStatus = 2;
  } else  if (ppm > 1000.0 && ppm < 2000.0) {
    gStatus = 1;
  } else {
    gStatus = 0;
  }
  Serial.print("gStatus = "); Serial.println(gStatus);
  return gStatus;
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

void beep(unsigned char delayms) {                       // Created a function for beep
  analogWrite(BUZZER, 150);                                      // This will set pin 11 to high
  delay(delayms);                                                             // Giving a delay
  analogWrite(BUZZER , 0);                                       // This will set pin 11 to LOW
  delay(delayms);                                                             // Giving a delay
}

void intShow() {
  show = true;
}

ISR(PCINT2_vect) { // Port D, PCINT16 - PCINT23
  show = true;
}

ISR(PCINT1_vect) { // // Port C, PCINT8 - PCINT14
  show = true;
}

ISR(PCINT0_vect) { // // Port B, PCINT0 - PCINT7
  show = true;
}
