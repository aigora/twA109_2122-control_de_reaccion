#include <OneWire.h>
#include <DallasTemperature.h>

//Pin donde está conectado DQ
const byte pinDatosDQ = 9;

//Instancia a las clases OneWIre y DallasTemperature;
OneWire oneWireObjeto(pinDatosDQ);
DallasTemperature sensorDS18B20(&oneWireObjeto);


void setup() {
  //Iniciamos la comunicación serie
  Serial.begin(9600);
  //Iniciamos el bus 1-Wire (el sensor) 
  sensorDS18B20.begin();

}

void loop() {
  delay(1000);
  // Mandamos comandos para toma de temperatura al sensor
  Serial.println("Mandando comandos al sensor");
  sensorDS18B20.requestTemperatures();
  float temp = sensorDS18B20.getTempCByIndex(0); //Se obtiene la temperatura en ºC

  //Leemos y mostramos los datos del sensor DS18B20
  Serial.print("Temperatura: ");
  Serial.println(temp);
  Serial.print("ºC ");
  delay(500);
  

}
