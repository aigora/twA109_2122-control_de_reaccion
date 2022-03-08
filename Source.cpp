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
		case 7:

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
		printf("\n\n\tCONTROL DE REACCIÓN");
		printf("\n\t===================\n");
		printf("\n 1 - Configurar proceso químico");
		printf("\n 2 - Comezar proceso químico");
		printf("\n 3 - Temperatura");
		printf("\n 4 - Presión");
		printf("\n 5 - Volumen");
		printf("\n 6 - Vaciado de emergencia");
		printf("\n 7 - Acabar proceso químico");
		printf("\n\n\tIntroduzca opción:");
		scanf_s("%d", &elegido);

		if (elegido < 1 || elegido >6)
			printf("\nOpción no válida");

	} while (elegido < 1 || elegido >6);

	return elegido;
}