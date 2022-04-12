#include <OneWire.h>
#include <DallasTemperature.h>

const byte pinDatosDQ = 9; //Pin de temperatura
const int EchoPin = 5; //pin de ultrasonido receptor
const int TriggerPin = 6; //pin de ultrasonido salida

String mensaje_entrada;
String mensaje_salida;

OneWire oneWireObjeto(pinDatosDQ); //variables temp
DallasTemperature sensorDS18B20(&oneWireObjeto); //variables temp

void setup() {
   Serial.begin(9600);
   pinMode(TriggerPin, OUTPUT); //iniciamos ultra
   pinMode(EchoPin, INPUT); //iniciamos ultra

   sensorDS18B20.begin(); //iniciamos temp
}

void loop ()
{ 
  if( Serial.available()> 0)  
     {
         mensaje_entrada = Serial.readStringUntil('\n');
        if (mensaje_entrada.compareTo("GET_DISTANCIA")==0) //ultrasonido
        {
                  float cm = ping(TriggerPin, EchoPin);
                  Serial.println(cm);
                  mensaje_salida=String("DISTANCIA="+String(cm,3));
        }
        if (mensaje_entrada.compareTo("GET_TEMPERATURA")==0) //temperatura
        {
            sensorDS18B20.requestTemperatures();
            float temp = sensorDS18B20.getTempCByIndex(0); //Se obtiene la temperatura en ºC
       
            Serial.println(temp);
            Serial.print("ºC ");
            mensaje_salida=String("TEMPERATURA="+String(temp,3));
        }
             Serial.println(mensaje_salida);
     }
}

float ping(int TriggerPin, int EchoPin) //funcion ultra
{
   long duration, distanceCm;
    
   digitalWrite(TriggerPin, LOW); // Para generar un pulso limpio ponemos a LOW 4us.
   delayMicroseconds(4);
   digitalWrite(TriggerPin, HIGH); // Generamos Trigger (disparo) de 10us.
   delayMicroseconds(10);
   digitalWrite(TriggerPin, LOW);
    
   duration = pulseIn(EchoPin, HIGH); // Medimos el tiempo entre pulsos, en us.
    
   distanceCm = duration * 10 / 292/ 2; // Convertimos a distancia en cm.
   return distanceCm;
}
