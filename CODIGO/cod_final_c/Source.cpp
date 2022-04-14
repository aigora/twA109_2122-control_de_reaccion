
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include <conio.h>
#include "SerialClass/SerialClass.h"
#include<math.h>

#define MAX_BUFFER 200
#define PAUSA_MS 200
#define TAM 20
#define RADIO 3
#define ALTURA 6
struct nodo
{
	char nombre[TAM];
	int temperatura;
	int volmax;
	struct nodo* siguiente;
};
typedef struct nodo PROCESO;

int menu_ppal(void);
void medicion_unica_ultra(Serial* Arduino, char* port);
float leer_sensor_distancia(Serial* Arduino);
float float_from_cadena(char* cadena);
int Enviar_y_Recibir(Serial* Arduino, const char* mensaje_enviar, char* mensaje_recibir);
void monitorizar_sensor_distancia(Serial* Arduino);
void monitorizar_sensor_temperatura(Serial* Arduino);
void medicion_unica_temp(Serial* Arduino);
float leer_sensor_temperatura(Serial* Arduino);
void activar_rele(Serial* Arduino);
void apagar_rele(Serial* Arduino);
float volumen(float);
void iniciar_pro_automatico(int temperatura, int volumen);
PROCESO* eliminar_proceso(char seleccionada[], PROCESO* pro, PROCESO* cab);

//CONEXIÓN
int main(void)
{
	int flag = 0;
	Serial* Arduino;
	char puerto[] = "COM5";
	int opcion_menu, opcion;
	char fallo, seleccionada[TAM];
	int temperaturaselec, volumenselec;
	PROCESO* pro;
	PROCESO* cab = NULL;

	setlocale(LC_ALL, "es-ES");
	Arduino = new Serial((char*)puerto);
	do
	{
		opcion_menu = menu_ppal();
		switch (opcion_menu)
		{
		case 1:
			scanf_s("%c", &fallo);
			pro = (PROCESO*)malloc(sizeof(PROCESO));
			pro->siguiente = cab;
			cab = pro;
			printf("\n");
			printf("\t==================================\n");
			printf("\t   CONFIGURAR NUEVA DESTILACIÓN\n");
			printf("\t==================================\n");
			printf("  Introduzca nombre de destilación: ");
			fgets(cab->nombre, TAM, stdin);
			printf("  Introduzca temperatura deseada (Cº): ");
			scanf_s("%d", &cab->temperatura);
			printf("  Introduzca volumen máximo de destilado (ml): ");
			scanf_s("%d", &cab->volmax);
			printf("\n  Destilación a %dCº y %dml de volumen objetivo se ha guardado como: ", (*cab).temperatura, (*cab).volmax);
			puts(cab->nombre);
			break;
		case 2:
			if (cab == NULL)
				printf("\n  No hay ninguna destilación definida\n");
			else
			{
				printf("\n");
				printf("\t==================================\n");
				printf("\t  DESTILACIONES PRECONFIGURADAS\n");
				printf("\t==================================\n");

				pro = cab;
				printf("\n  - Destilación a %dCº y %dml de volumen objetivo con nombre ", (*pro).temperatura, (*pro).volmax);
				puts(pro->nombre);
				while (pro->siguiente != NULL)
				{
					pro = pro->siguiente;
					printf("\n  - Destilación a %dCº y %dml de volumen objetivo con nombre: ", (*pro).temperatura, (*pro).volmax);
					puts(pro->nombre);
				}
				scanf_s("%c", &fallo);
				do
				{
					printf("  Seleccione destilación escribiendo el nombre de la deseada: ");
					fgets(seleccionada, TAM, stdin);

					pro = cab;
					do
					{
						if (strcmp(seleccionada, (*pro).nombre) == 0)
							flag = 1;
						pro = pro->siguiente;
					} while (pro != NULL);

					if (flag == 0)
						printf("\n  El nombre seleccionado no corresponde con ninguna destilación guardada\n");
				} while (flag == 0);
				for (pro = cab; strcmp(seleccionada, (*pro).nombre) != 0; pro = pro->siguiente)
				{}
				printf("\n  Ha seleccionado la destilación ");
				puts(pro->nombre);
				do
				{
					printf(" OPCIONES");
					printf("\n==========");
					printf("\n 1 - Iniciar proceso");
					printf("\n 2 - Eliminar proceso");
					printf("\n 3 - Volver a menú principal");
					printf("\n\n Escoja opción: ");
					scanf_s("%d", &opcion);
					if (opcion != 1 || opcion != 2 || opcion != 3)
						printf("\n Opción no valida\n\n");
				} while (opcion != 1 || opcion != 2 || opcion != 3);
				switch (opcion)
				{
				case 1:
					temperaturaselec = (*pro).temperatura;
					volumenselec = (*pro).volmax;
					iniciar_pro_automatico(temperaturaselec, volumenselec);
					break;
				case 2: 
					eliminar_proceso(seleccionada, pro, cab);
					break;
				case 3:
					break;
				}
			}

			break;
		case 3:

			break;
		case 4:
			printf("\n");
			printf("\t==================================\n");
			printf("\t    PRUEBA DE FUNCIONAMIENTO\n");
			printf("\t==================================\n");

			float distancia, temperatura;

			void medicion_unica_ultra(Serial * Arduino, char* port);
			{
				float distancia;
				distancia = leer_sensor_distancia(Arduino);
				if (distancia != -1)
					printf("\nDistancia: %f\n", distancia);
				return distancia;
			}

			void medicion_unica_temp(Serial * Arduino);
			{
				float temperatura;
				temperatura = leer_sensor_temperatura(Arduino);
				printf("\nTemperatura: %f\n", temperatura);
				return temperatura;
			}


			if (distancia <= 0)
				printf("El sensor de distancia da error\n");
			else
				printf("El sensor de temperatura funciona\n");

			if (temperatura <= 0 || temperatura > 150)
				printf("El sensor de temperatura da error\n");
			else
				printf("El sensor de temperatura funciona\n");

			break;

		case 5:

			break;
		case 6:
			break;
		default: printf("\nOpción incorrecta\n");
		}
	} while (opcion_menu != 6);
	return 0;
}

//MENÚ PRINCIPAL
int menu_ppal(void)
{
	int opcion;
	printf("\n");
	printf("\t==================================\n");
	printf("\t\t  MENÚ PRINCIPAL\n");
	printf("\t==================================\n");
	printf("\t1 - Configurar nueva destilación\n");
	printf("\t2 - Destilaciones preconfiguradas\n");
	printf("\t3 - Proceso manual\n");
	printf("\t4 - Prueba funcionamiento\n");
	printf("\t5 - Guía uso\n");
	printf("\t6 - Salir\n");
	printf("\n\t  ELIGE OPCIÓN: ");

	scanf_s("%d", &opcion);
	return opcion;
}

//LEER SENSOR ULTRASONIDO
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
		distancia = float_from_cadena(mensaje_recibido);
	}
	return distancia;
}

//LEER SENSOR TEMPERATURA
float leer_sensor_temperatura(Serial* Arduino)
{
	float temperatura;
	int bytesRecibidos;
	char mensaje_recibido[MAX_BUFFER];

	bytesRecibidos = Enviar_y_Recibir(Arduino, "GET_TEMPERATURA\n", mensaje_recibido);

	temperatura = float_from_cadena(mensaje_recibido);

	return temperatura;
}

//CALCULA EL VOLUMEN A PARTIR DE LA DISTANCIA
float volumen(float distancia)
{
	float vol;
	float pi = acos(-1.0);
	float h;

	h = ALTURA - distancia;
	vol = (float)(h * pi * RADIO * RADIO);

	return vol;
}

//ACTIVAR RELÉ
void activar_rele(Serial* Arduino)
{
	int bytesRecibidos;
	char mensaje_recibido[MAX_BUFFER];

	bytesRecibidos = Enviar_y_Recibir(Arduino, "CIERRA_RELE\n", mensaje_recibido);
	printf("\n");
	puts(mensaje_recibido);
}

//APAGAR RELÉ
void apagar_rele(Serial* Arduino)
{
	int bytesRecibidos;
	char mensaje_recibido[MAX_BUFFER];

	bytesRecibidos = Enviar_y_Recibir(Arduino, "ABRE_RELE\n", mensaje_recibido);
	printf("\n");
	puts(mensaje_recibido);
}

//SEPARAR NÚMEROS DE CADENA DE CARACTERES
float float_from_cadena(char* cadena)
{
	int i;
	int estado = 0;
	float numero = 0, divisor = 10;
	for (i = 0; cadena[i] != '\n'; i++)
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

//ENVIAR Y RECIBIR MENSAJES
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

//MONITORIZAR SENSOR DISTANCIA
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

//MONITORIZAR SENSOR TEMPERATURA
void monitorizar_sensor_temperatura(Serial* Arduino)
{
	float frecuencia, temperatura;
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
			temperatura = leer_sensor_temperatura(Arduino);
			if (temperatura != -1)
				printf("%.2f ", temperatura);
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

//MIDE ULTRASONIDO UNA VEZ
void medicion_unica_ultra(Serial* Arduino, char* port)
{
	float distancia;
	distancia = leer_sensor_distancia(Arduino);
	if (distancia != -1)
		printf("\nDistancia: %f\n", distancia);
}

//MIDE TEMPERATURA UNA VEZ
void medicion_unica_temp(Serial* Arduino)
{
	float temperatura;
	temperatura = leer_sensor_temperatura(Arduino);
	printf("\nTemperatura: %f\n", temperatura);
}

//ELIMINAR PROCESO DE LISTA
PROCESO* eliminar_proceso(char seleccionada[], PROCESO* pro, PROCESO* cab)
{
	PROCESO* control;
	pro = cab;
	control = cab;
	while (strcmp(seleccionada, (*pro).nombre) != 0)
	{
		control = pro;
		pro = pro->siguiente;
	}
	if (pro->siguiente == NULL || pro != cab)
	{
		control->siguiente = NULL;
		free(pro);
		pro = cab;
	}
	if (pro == cab)
	{
		cab = pro->siguiente;
		free(pro);
	}
	if (pro->siguiente != NULL || pro != cab)
	{
		control->siguiente = pro->siguiente;
		free(pro);
		pro = cab;
	}
	return cab;
}

//INICIAR PROCESO AUTOMÁTICO
void iniciar_pro_automatico(int temperatura, int volumen) 
{

}

