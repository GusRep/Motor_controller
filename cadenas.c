// Cadenas: trabajo con strings.
// Recibe: nada
// Retorna: nada

#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#pragma warning (disable:4996)

// Constantes
#define N	10
#define TS	200

//void cadenas (void)
void main (void)
{
	char texto[100];
	char nomb[]="Gustavo";
	char space[]=" ";
	char app[]="Gil";
	char enter[]="\n";
	char frase[50]="Mi mama me mima.  :-P";

	float num;


	strcpy(texto,nomb);		// notar que es COPY
	strcat(texto,space);	//				CONCATENA
	strcat(texto,app);
	strcat(texto,enter);
	strcat(texto,frase);

	printf("%s",texto);


	printf("\n\n\nIngrese un número en flotante: ");
	scanf("%f",&num);
	printf("\nEl valor ingresado es: %f", num);


	

	// Finalizacion
	printf("\n\n\nPulse una tecla para finalizar.\n");
	fflush(stdin);
	getchar();


}
