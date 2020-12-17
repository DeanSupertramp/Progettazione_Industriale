#define MQ5 A0

float R0 = 16981.25; //Resistenza ricavata dalla fase di calibrazione
float sensorValue = 0.0;
float sensor_volt = 0.0;
int campioni = 30;

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

int gStatus = 0;

int i = 0;
int j = 1;

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
  textGas(wGas, percentuale);
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
