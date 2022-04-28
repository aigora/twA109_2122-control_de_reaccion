#include <OneWire.h>
#include <DallasTemperature.h>

const byte pinDatosDQ = 9; //Pin de temperatura
const int EchoPin = 5; //pin de ultrasonido receptor
const int TriggerPin = 6; //pin de ultrasonido salida
int relay = 11; //pin de relé

String mensaje_entrada;
String mensaje_salida;

OneWire oneWireObjeto(pinDatosDQ); //variables temp
DallasTemperature sensorDS18B20(&oneWireObjeto); //variables temp

void setup() {
   Serial.begin(9600);
   pinMode(TriggerPin, OUTPUT); //iniciamos ultra
   pinMode(EchoPin, INPUT); //iniciamos ultra

   sensorDS18B20.begin(); //iniciamos temp

   pinMode(relay, OUTPUT); //iniciamos relé
}

void loop ()
{ 
  if( Serial.available()> 0)  
     {
         mensaje_entrada = Serial.readStringUntil('\n');
        if (mensaje_entrada.compareTo("GET_DISTANCIA")==0) //ultrasonido
        {
                  long mm = ping(TriggerPin, EchoPin);// Función para calcular la distancia
                   // Y la mostramos por el puerto serie
                  
                  mensaje_salida=String("DISTANCIA="+String(mm));
                  Serial.print("mm");
        }
        if (mensaje_entrada.compareTo("GET_TEMPERATURA")==0) //temperatura
        {
            sensorDS18B20.requestTemperatures();
            float temp = sensorDS18B20.getTempCByIndex(0); //Se obtiene la temperatura en ºC
       
            Serial.println(temp);
            Serial.print("ºC ");
            mensaje_salida=String("TEMPERATURA="+String(temp,3));
        }
        if (mensaje_entrada.compareTo("CIERRA_RELE")==0) //encender placa calentadora
        {
          digitalWrite(relay, HIGH);
          mensaje_salida=String("PLACA CALENTADORA ENCENDIDA");
          delay(500);
        }
        if (mensaje_entrada.compareTo("ABRE_RELE")==0) //apagar placa calentadora
        {
          digitalWrite(relay, LOW);
          mensaje_salida=String("PLACA CALENTADORA APAGADA");
          delay(500);
        }
             Serial.println(mensaje_salida);
     }
}

long ping(int TriggerPin, int EchoPin) //funcion ultra
{
   long Duration, distance_mm;
    
   digitalWrite(TriggerPin, LOW); // Para generar un pulso limpio ponemos a LOW 4us.
   delayMicroseconds(4);
   digitalWrite(TriggerPin, HIGH); // Generamos Trigger (disparo) de 10us.
   delayMicroseconds(10);
   digitalWrite(TriggerPin, LOW);
    
   Duration = pulseIn(EchoPin, HIGH); // Medimos el tiempo entre pulsos, en us.
    
   long Distancia_mm = fDistancia(Duration); // Función para calcular la distancia
   return Distancia_mm;
}

// Función para calcular la distancia
long fDistancia(long tiempo)
{
// Calculamos la distancia en mm
// ((tiempo)*(Velocidad del sonido)/ el camino se hace dos veces) 

long DistanceCalc; // Variable para los cálculos
DistanceCalc = (tiempo /2.9) / 2; // Cálculos en milímetros
return DistanceCalc;
}
