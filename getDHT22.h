#include "DHT.h"

#define DHTPIN A5

#define DHTTYPE DHT22

int tStatus = 0;
int hStatus = 0;

DHT dht(DHTPIN, DHTTYPE);

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
