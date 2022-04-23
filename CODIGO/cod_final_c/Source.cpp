
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
float leer_sensor_distancia(Serial* Arduino);
float float_from_cadena(char* cadena);
int Enviar_y_Recibir(Serial* Arduino, const char* mensaje_enviar, char* mensaje_recibir);
void monitorizar_sensor_distancia(Serial* Arduino);
void monitorizar_sensor_temperatura(Serial* Arduino);
void medicion_unica_temp(Serial* Arduino);
float leer_sensor_temperatura(Serial* Arduino);
int activar_rele(Serial* Arduino);
int apagar_rele(Serial* Arduino);
float volumen(float);
void iniciar_pro_automatico(int temperatura, int volumen, char seleccionada[], Serial* Arduino);
void guia(void);
PROCESO* eliminar_proceso(char seleccionada[], PROCESO* pro, PROCESO* cab);
PROCESO* conf_nueva_destilacion(PROCESO* pro, PROCESO* cab);
PROCESO* destilaciones_preconfiguradas(PROCESO* pro, PROCESO* cab, Serial* Arduino);
void prueba_funcionamiento(Serial* Arduino);
void inicio_programa(void);
int p_rap_funcionamiento(Serial* Arduino);
void proceso_manual(void);

int main(void)
{
	Serial* Arduino;
	char puerto[] = "COM5";
	int opcion_menu, respuesta;
	char fallo;
	PROCESO* pro = NULL;
	PROCESO* cab = NULL;

	setlocale(LC_ALL, "es-ES");
	Arduino = new Serial((char*)puerto);
	inicio_programa();
	do
	{
		opcion_menu = menu_ppal();
		switch (opcion_menu)
		{
		case 1:
			scanf_s("%c", &fallo);
			system("cls");
			cab = conf_nueva_destilacion(pro, cab);
			pro = cab;
			break;
		case 2:  //inacabado
			if (cab == NULL)
				system("cls");
				printf("\n  No hay ninguna destilación definida\n");
			if (cab != NULL)
			{
				cab = destilaciones_preconfiguradas(pro, cab, Arduino);
				pro = cab;
			}
			break;
		case 3:
			system("cls");
			printf("\n\n\n\t\tCARGANDO...");
			respuesta = p_rap_funcionamiento(Arduino);
			system("cls");
			if (respuesta == 0)
			{
				system("cls");
				printf("\n\tERROR\n");
				printf("\n\tNo se puede comenzar el proceso debido a que no todos los componentes\n\testán conectados o alguno no funciona correctamente");
				printf("\n\n\tRealize una prueba de funcionamiento (Op.4) para localizar el fallo");
				printf("\n\n\n\n\n\n\t\t\t\t\t   PULSE <ENTER> ");
				scanf_s("%c", &fallo);
				scanf_s("%c", &fallo);
				system("cls");
			}
			else
			{
				proceso_manual();
			}
			break;
		case 4:
			prueba_funcionamiento(Arduino);
			break;
		case 5:
			guia();
			break;
		case 6:
			system("cls");
			printf("\n\n\n\n\t\t\t\tFIN DE PROGRAMA\n\n\n\n");
			break;
		default: 
			system("cls");
			printf("\n  Opción incorrecta\n");
			break;
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
	printf("\t==================================\n\n");
	printf("\t1 - Configurar nueva destilación\n");
	printf("\t2 - Destilaciones preconfiguradas\n");
	printf("\t3 - Proceso manual\n");
	printf("\t4 - Prueba funcionamiento\n");
	printf("\t5 - Guía uso\n");
	printf("\t6 - Salir\n");
	printf("\n\n\t  ELIGE OPCIÓN: ");

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
		distancia = -500;
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

	if (bytesRecibidos <= 0)
	{
		temperatura = -500;
	}

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
int activar_rele(Serial* Arduino)
{
	int bytesRecibidos;
	char mensaje_recibido[MAX_BUFFER];
	int funciona = 1;

	bytesRecibidos = Enviar_y_Recibir(Arduino, "CIERRA_RELE\n", mensaje_recibido);
	if (bytesRecibidos <= 0)
	{
		funciona = 0;
	}
	return funciona;
}

//APAGAR RELÉ
int apagar_rele(Serial* Arduino)
{
	int bytesRecibidos;
	char mensaje_recibido[MAX_BUFFER];
	int funciona = 1;

	bytesRecibidos = Enviar_y_Recibir(Arduino, "ABRE_RELE\n", mensaje_recibido);
	if (bytesRecibidos <= 0)
	{
		funciona = 0;
	}
	return funciona;
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

//PRUEBA FUCIONAMIENTO
void prueba_funcionamiento(Serial* Arduino)
{
	char fallo;
	float distancia, temperatura, funciona;

	system("cls");
	printf("\n\n\n\t\tCARGANDO...");
	
	distancia = leer_sensor_distancia(Arduino);
	temperatura = leer_sensor_temperatura(Arduino);
	funciona = activar_rele(Arduino);
	funciona = apagar_rele(Arduino);

	system("cls");
	printf("\n");
	printf("\t================================\n");
	printf("\t    PRUEBA DE FUNCIONAMIENTO\n");
	printf("\t================================\n");

	if (distancia == -500)
		printf("\n  Error en el sensor de distancia\n");
	else
		printf("\n  El sensor de temperatura funciona correctamente\n");

	if (temperatura == -500)
		printf("\n  Error en el sensor de temperatura\n");
	else
		printf("\n  El sensor de temperatura funciona correctamente\n");

	if (funciona == 0)
		printf("\n  Error en el relé que activa la placa calentadora\n");
	if (funciona == 1)
		printf("\n  El relé que activa la placa calentadora funciona correctamente\n");

	printf("\n\n\tPULSE <ENTER> ");
	scanf_s("%c", &fallo);
	scanf_s("%c", &fallo);
	system("cls");
}

//PRUEBA RÁPIDA DE SENSORES
int p_rap_funcionamiento(Serial* Arduino)
{
	float distancia, temperatura, funciona;
	int respuesta = 1;

	distancia = leer_sensor_distancia(Arduino);
	temperatura = leer_sensor_temperatura(Arduino);
	funciona = activar_rele(Arduino);
	funciona = apagar_rele(Arduino);

	if (distancia == -500)
		respuesta = 0;
	if (temperatura == -500)
		respuesta = 0;
	if (funciona == 0)
		respuesta = 0;

	return respuesta;
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

	if (cab == pro)
	{
		cab = pro->siguiente;
	}
	else
	{
		control->siguiente = pro->siguiente;
	}

	free(pro);
	return cab;
}

//GUÍA
void guia(void)
{
	char fallo;

	system("cls");
	printf("\t==================================\n");
	printf("\t\t  GUÍA DE USO\n");
	printf("\t==================================\n\n");
	printf(" Este programa de destilación monitorizada a tiempo real permite configurar nuevas destilaciones automáticas,\n");
	printf(" comenzar un proceso manual (controlando la temperatura y el volumen final deseado) o hacer pruebas del funcionamiento\n del mismo.\n\n");
	printf(" Para configurar nuevas destilaciones, elija la opción '1'. Después, escriba el nombre de la destilación deseada,\n");
	printf(" seguido de la temperatura de ebullición del líquido a destilar, y el volumen de destilado que busca obtener.\n Esta destilación se guardará en la memoria del programa.\n\n");
	printf(" Para acceder a destilaciones preconfiguradas, pulse '2'.Le aparecerán todas las destilaciones que usted haya\n");
	printf(" configurado previamente. Para seleccionar una destilación, escriba el nombre de la misma y posteriormente, pulse '1'\n");
	printf(" si desea comenzar el proceso de destilación, '2' en caso de que quiera borrarla de la memoria del programa, o '3' para\n volver al menú principal.\n\n");
	printf(" Para realizar un proceso manual, pulse '3'. Podrá activar y desactivar el calentador, y se le mostrará a tiempo real\n");
	printf(" la temperatura interna del líquido en proceso de destilación, además del volumen de destilado que se vaya obteniendo,\n pudiendo acabar el proceso cuando desee.\n\n");
	printf(" Para realizar una prueba del funcionamiento de los sensores y el relé, elija la opción '4'.\n\n");
	printf(" Para salir del programa, elija la opción '6'.\n\n\n");
	printf("\n\tPULSE <ENTER> ");
	scanf_s("%c", &fallo);
	scanf_s("%c", &fallo);
	system("cls");
}

//PANTALLA INICIO PROGRAMA
void inicio_programa(void)
{
	char fallo;
	
	printf("\n\n\n\n\t\t\t\t===== CONTROL DE DESTILACIONES =====\n\n\n\n\n\n\n");
	printf("\tMatias Lopez Viagel\n\tDaniel Olsson Andrés\n\tDavid Mendez Velasquez");
	printf("\n\n\n\n\n\n\t\t\t\t\t   PULSE <ENTER> ");
	scanf_s("%c", &fallo);
	system("cls");
}

//CONFIGURAR NUEVA DESTILACIÓN
PROCESO* conf_nueva_destilacion(PROCESO* pro, PROCESO* cab)
{
	char fallo;

	pro = (PROCESO*)malloc(sizeof(PROCESO));
	pro->siguiente = cab;
	cab = pro;
	system("cls");
	printf("\n");
	printf("\t==================================\n");
	printf("\t   CONFIGURAR NUEVA DESTILACIÓN\n");
	printf("\t==================================\n\n");
	printf("  Introduzca nombre de destilación: ");
	fgets(cab->nombre, TAM, stdin);
	printf("  Introduzca temperatura deseada (Cº): ");
	scanf_s("%d", &cab->temperatura);
	printf("  Introduzca volumen máximo de destilado (ml): ");
	scanf_s("%d", &cab->volmax);
	printf("\n  Destilación a %dCº y %dml de volumen objetivo se ha guardado como: ", (*cab).temperatura, (*cab).volmax);
	puts(cab->nombre);
	printf("\n\tPULSE <ENTER> ");
	scanf_s("%c", &fallo);
	scanf_s("%c", &fallo);
	system("cls");
	return cab;
}

//DESTILACIONES PRECONFIGURADAS 
PROCESO* destilaciones_preconfiguradas(PROCESO* pro, PROCESO* cab, Serial* Arduino)
{
	int opcion, flag = 0;
	char  seleccionada[TAM];
	int temperaturaselec, volumenselec;
	char fallo;
	int respuesta;

	system("cls");
	printf("\n");
	printf("\t==================================\n");
	printf("\t  DESTILACIONES PRECONFIGURADAS\n");
	printf("\t==================================\n");

	pro = cab;
	printf("\n  -> Destilación a %dCº y %dml de volumen objetivo con nombre: ", (*pro).temperatura, (*pro).volmax);
	puts(pro->nombre);
	while (pro->siguiente != NULL)
	{
		pro = pro->siguiente;
		printf("  -> Destilación a %dCº y %dml de volumen objetivo con nombre: ", (*pro).temperatura, (*pro).volmax);
		puts(pro->nombre);
	}
	printf("  _________________________________________________________________________________________________\n\n");
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
			printf("\n\tEl nombre seleccionado no corresponde con ninguna destilación guardada\n\n");
	} while (flag == 0);
	for (pro = cab; strcmp(seleccionada, (*pro).nombre) != 0; pro = pro->siguiente)
	{}
	system("cls");
	printf("\n  Ha seleccionado la destilación: ");
	puts(pro->nombre);
	do
	{
		printf("   OPCIONES");
		printf("\n  ==========");
		printf("\n  1 - Iniciar proceso");
		printf("\n  2 - Eliminar proceso");
		printf("\n  3 - Volver a menú principal");
		printf("\n\n\t  ELIGE OPCIÓN: ");
		scanf_s("%d", &opcion);
		if (opcion != 1 && opcion != 2 && opcion != 3)
		{
			system("cls");
			printf("\n Opción no valida\n\n");
		}
	} while (opcion != 1 && opcion != 2 && opcion != 3);
	switch (opcion)
	{
	case 1:
		temperaturaselec = (*pro).temperatura;
		volumenselec = (*pro).volmax;
		pro = cab;
		system("cls");
		printf("\n\n\n\t\tCARGANDO...");
		respuesta = p_rap_funcionamiento(Arduino);
		system("cls");
		if (respuesta == 0)
		{
			system("cls");
			printf("\n\tERROR\n");
			printf("\n\tNo se puede comenzar el proceso debido a que no todos los componentes\n\testán conectados o alguno no funciona correctamente");
			printf("\n\n\tRealize una prueba de funcionamiento (Op.4) para localizar el fallo");
			printf("\n\n\n\n\n\n\t\t\t\t\t   PULSE <ENTER> ");
			scanf_s("%c", &fallo);
			scanf_s("%c", &fallo);
			system("cls");
		}
		else
		{
		iniciar_pro_automatico(temperaturaselec, volumenselec, seleccionada, Arduino);
		}
		break;
	case 2:
		cab = eliminar_proceso(seleccionada, pro, cab);
		pro = cab;
		system("cls");
		printf("\n  Se ha eliminado la destilación con nombre: ");
		puts(seleccionada);
		break;
	case 3:
		system("cls");
		break;
	}

	return cab;
}

//PROCESO MANUAL (inacabada)
void proceso_manual(void)
{

}

//INICIAR  PROCESO AUTOMÁTICO (inacabada)
void iniciar_pro_automatico(int temperatura, int volumen, char seleccionada[], Serial* Arduino)
{
	
}


