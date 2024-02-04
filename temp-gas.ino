#include <MKRWAN.h>
#include "arduino_secrets.h" 
#include <DHT.h>

#define DHTPIN 2        // Pin de datos del sensor DHT-11
#define DHTTYPE DHT11   // Tipo de sensor DHT 
#define BANDA EU868     // Define la banda de frecuencia utilizada 868 MHz 
#define GASPIN A0       // Pin del sensor de gas
#define M -0.263        //Pendiente
#define B 0.42          //Ordenada 
#define R0 3.5          //Resistencia del sensor en aire fresco

#define minutos 1       // Cada cuanto mandamos informacion
const long periodo = minutos*60*1000;

LoRaModem modem(Serial1);
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);
  dht.begin();
  connect();
}

void loop() {
  //Lectura de temperatura y humedad
  float temperatura = dht.readTemperature();
  float humedad = dht.readHumidity();
  int temp = temperatura * 100;
  int hum = humedad * 100;

  //Lectura del sensor de gases
  float sensorValue = analogRead(GASPIN);
  float sensor_volt = sensorValue * (5.0 / 1023.0); 
  float RS_gas = ((5.0 * 10.0) / sensor_volt) - 10.0; 
  float ratio = RS_gas / R0; 
  double ppm_log = (log10(ratio) - B) / M; 
  double ppm_double = pow(10, ppm_log);
  int ppm = ppm_double * 100;

  //Umbral a partir del cual decimos que hemos detectado limoneno
  int lemo = 0;
  if (ppm > 100) {
    lemo = 1; 
    byte detec[7];
    detec[0] = temp >> 8;
    detec[1] = temp;
    detec[2] = hum >> 8;
    detec[3] = hum;
    detec[4] = ppm >> 8;
    detec[5] = ppm;
    detec[6] = lemo;

    modem.beginPacket();
    modem.write(detec, sizeof(byte) * 7);
    modem.endPacket();
  }

  byte bytes[7];
  bytes[0] = temp >> 8;
  bytes[1] = temp;
  bytes[2] = hum >> 8;
  bytes[3] = hum;
  bytes[4] = ppm >> 8;
  bytes[5] = ppm;
  bytes[6] = lemo;

  //Envio a TTN
  modem.beginPacket();
  modem.write(bytes, sizeof(byte) * 7);
  modem.endPacket();

  delay(periodo);  
}

//Conexion con TTN
void connect() {
  if (!modem.begin(BANDA)) {
    Serial.println("Error al iniciar el modulo");
    while (1) {}
  }

  Serial.print("Tu EUI es: ");
  Serial.println(modem.deviceEUI());

  int connected = modem.joinOTAA(appEui, appKey);
  if (!connected) {
    Serial.println("Error, prueba a estar cerca de una ventana y vuelve a intentarlo.");
    while (1) {}
  }

  Serial.println("Conexion exitosa");

  modem.setADR(true);
  modem.dataRate(5);
}
