const int EchoPin = 5;
const int TriggerPin = 6;
String mensaje_entrada;
String mensaje_salida;
void setup() {
   Serial.begin(9600);
   pinMode(TriggerPin, OUTPUT);
   pinMode(EchoPin, INPUT);
}
void loop ()
{
     if( Serial.available()> 0)  
     {
         mensaje_entrada = Serial.readStringUntil('\n');
        if (mensaje_entrada.compareTo("GET_DISTANCIA")==0)
        {
                  float cm = ping(TriggerPin, EchoPin);
                  Serial.println(cm);
                  mensaje_salida=String("DISTANCIA="+String(cm,3));
        }
        else
          mensaje_salida="COMANDO DESCONOCIDO";
        Serial.println(mensaje_salida);
     }
}
float ping(int TriggerPin, int EchoPin)
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
