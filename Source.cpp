#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include "SerialClass.cpp" //#include "SerialClass/SerialClass.h"
#include "SerialClass.h"
#include <conio.h>

#define MAX_BUFFER 200
#define PAUSA_MS 200

const int EchoPin = 5;
const int TriggerPin = 6;
String mensaje_entrada;
String mensaje_salida;

int menu(void);
void verifica_sensores(Serial*, char*); //void verifica_sensores(Serial* Arduino, char* port)
float leer_sensor_distancia(Serial*); //float leer_sensor_distancia(Serial* Arduino)
float float_from_cadena(char); //float float_from_cadena(char cadena[100])
int Enviar_y_Recibir(Serial*, const char*, char*); //int Enviar_y_Recibir(Serial* Arduino, const char* mensaje_enviar, char* mensaje_recibir)
void setup();
void loop();
float ping(int, int); //float ping(int TriggerPin, int EchoPin)
void monitorizar_sensor_distancia(Serial*); //void monitorizar_sensor_distancia(Serial* Arduino)


int main(void)
{
	Serial* Arduino;
	char puerto[] = "COM6";
	int opcion_menu;
	setlocale(LC_ALL, "es-ES");
	Arduino = new Serial((char*)puerto);
	do
	{
		opcion_menu = menu();
		switch (opcion_menu)
		{
		case 1:
			verifica_sensores(Arduino, puerto);
			break;
		case 2:
			monitorizar_sensor_distancia(Arduino);
			break;
		case 3:
			break;
		default: printf("\nOpción incorrecta\n");
		}
	} while (opcion_menu != 3);
	return 0;

int menu(void)
{
	int opcion;
	printf("\n");
	printf("Menú Principal\n");
	printf("==============\n");
	printf("1 - Verificar sensores.\n");
	printf("2 - Monitorizar sensores.\n");
	printf("3 - Salir de la aplicación\n");
	printf("Opción:");
	scanf_s("%d", &opcion);
	return opcion;
}

void verifica_sensores(Serial* Arduino, char* port)
{
	float distancia;
	if (Arduino->IsConnected())
	{
		distancia = leer_sensor_distancia(Arduino);
		if (distancia != -1)
			printf("\nDistancia: %f\n", distancia);
	}
	else
	{
		printf("\nNo se ha podido conectar con Arduino.\n");
		printf("Revise la conexión, el puerto %s y desactive el monitor serie del
			IDE de Arduino.\n",port);
	}
}

float leer_sensor_distancia(Serial* Arduino)
{
	float distancia;
	int bytesRecibidos;
	char mensaje_recibido[MAX_BUFFER];

	bytesRecibidos = Enviar_y_Recibir(Arduino, "GET_DISTANCIA\n", mensaje_recibido);
	if (bytesRecibidos <= 0)
	{
		printf("\nNo se ha recibido respuesta a la petición\n");
		distancia = -1;
	}
	else
	{
		printf("\nLa respuesta recibida tiene %d bytes. Recibido=%s\n", bytesRecibidos,
			mensaje_recibido);
		distancia = float_from_cadena(mensaje_recibido);
	}
	return distancia;
}

float float_from_cadena(char cadena[100])
{
	int estado = 0, i;
	float numero, divisor;

	for(i=0;cadena[i]!='\0';i++)
	{ 
		switch (estado)
		{
		case 0:
			if (cadena[i] >= '0' && cadena[i] <= '9')
			{
				numero = cadena[i] - '0';
				estado = 1;
			}
			break;
		case 1:
			if (cadena[i] >= '0' && cadena[i] <= '9')
				numero = numero * 10 + cadena[i] - '0';
			else
				if (cadena[i] == '.' || cadena[i] == ',')
					estado = 2;
				else
					estado = 3;
			break;
		case 2: // Parte decimal
			if (cadena[i] >= '0' && cadena[i] <= '9')
			{
				numero = numero + (float)(cadena[i] - '0') / divisor;
				divisor *= 10;
			}
			else
				estado = 3;
			break;
		}
	}
	return numero;
}

int Enviar_y_Recibir(Serial* Arduino, const char* mensaje_enviar, char* mensaje_recibir)
{
	int bytes_recibidos = 0, total = 0;
	int intentos = 0, fin_linea = 0;
	Arduino->WriteData((char*)mensaje_enviar, strlen(mensaje_enviar));
	Sleep(PAUSA_MS);
	bytes_recibidos = Arduino->ReadData(mensaje_recibir, sizeof(char) * MAX_BUFFER - 1);
	while ((bytes_recibidos > 0 || intentos < 5) && fin_linea == 0)
	{
		if (bytes_recibidos > 0)
		{
			total += bytes_recibidos;
			if (mensaje_recibir[total - 1] == 13 || mensaje_recibir[total - 1] == 10)
				fin_linea = 1;
		}
		else
			intentos++;
		Sleep(PAUSA_MS);
		bytes_recibidos = Arduino->ReadData(mensaje_recibir + total, sizeof(char) * MAX_BUFFER - 1);
	}
	if (total > 0)
		mensaje_recibir[total - 1] = '\0';
	return total;
}

// VERSIÓN 2

void setup() {
	Serial.begin(9600);
	pinMode(TriggerPin, OUTPUT);
	pinMode(EchoPin, INPUT);
}

void loop()
{
	if (Serial.available() > 0)
	{
		mensaje_entrada = Serial.readStringUntil('\n');
		if (mensaje_entrada.compareTo("GET_DISTANCIA") == 0)
		{
			float cm = ping(TriggerPin, EchoPin);
			Serial.println(cm);
			mensaje_salida = String("DISTANCIA=" + String(cm, 3));
		}
		else
			mensaje_salida = "COMANDO DESCONOCIDO";
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

	distanceCm = duration * 10 / 292 / 2; // Convertimos a distancia en cm.
	return distanceCm;
}

void monitorizar_sensor_distancia(Serial* Arduino)
{
	float frecuencia, distancia;
	char tecla;
	do
	{
		printf("Establezca frecuencia de muestreo (0,5 Hz - 2,0 Hz):");
		scanf_s("%f", &frecuencia);
	} while (frecuencia < 0.5 || frecuencia>2.0);
	printf("Pulse una tecla para finalizar la monitorización\n");
	do
	{
		if (Arduino->IsConnected())
		{
			distancia = leer_sensor_distancia(Arduino);
			if (distancia != -1)
				printf("%.2f ", distancia);
			else
				printf("XXX ");
		}
		else
			printf("\nNo se ha podido conectar con Arduino.\n");
		if ((1 / frecuencia) * 1000 > PAUSA_MS)
			Sleep((1 / frecuencia) * 1000 - PAUSA_MS);
	} while (_kbhit() == 0);
	tecla = _getch();
	return;
}