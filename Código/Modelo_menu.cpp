#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <locale.h>

int menu_ppal(void);

int main(void)
{
	int opcion;
	opcion = menu_ppal();

	switch (opcion)
	{ 
		case 1:
			
			break;

		case 2:

			break;

		case 3:

			break;

		case 4:

			break;

		case 5:

			break;

		case 6:

			break;
	}

		



return 0;
}

int menu_ppal(void)
{
	setlocale(LC_ALL, "es-ES");
	int elegido;
	do
	{
	printf("\n\n\tCONTROL DE REACCI�N");
	printf("\n\t===================\n");
	printf("\n 1 - Configurar proceso");
	printf("\n 2 - Temperatura");
	printf("\n 3 - Presi�n");
	printf("\n 4 - Volumen");
	printf("\n 5 - Vaciado de emergencia");
	printf("\n 6 - Acabar proceso");
	printf("\n\n\tIntroduzca opci�n:");
	scanf_s("%d", &elegido);

	if (elegido < 1 || elegido >6)
		printf("\nOpci�n no v�lida");

	} while (elegido < 1 || elegido >6);

return elegido;
}