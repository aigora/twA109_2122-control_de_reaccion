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
		printf("\n\n\tCONTROL DE DESTILACIÓN");
		printf("\n\t===================\n");
		printf("\n 1 - Configurar destilación");
		printf("\n 2 - Comezar destilación");
		printf("\n 3 - Datos temperatura");
		printf("\n 4 - Datos presión");
		printf("\n 5 - Datos volumen");
		printf("\n 6 - Apagar calentador");
		printf("\n 7 - Forzar fin de proceso");
		printf("\n\n\tIntroduzca opción:");
		scanf_s("%d", &elegido);

		if (elegido < 1 || elegido >6)
			printf("\nOpción no válida");

	} while (elegido < 1 || elegido >6);

	return elegido;
}
