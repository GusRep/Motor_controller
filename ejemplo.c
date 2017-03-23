// NO COMPILAR, ES SOLO PARA REFERENCIA DE "PRINTF" Y "SCANF"
// Recibe: nada
// Retorna: nada

#include<stdio.h>
#include<stdlib.h>

#pragma warning (disable:4996)

// Constantes
#define N 3		

// Prototipos
void InputData(float*);
void InputVec(float*, int);
float ProdEsc(float*, float*, int);
float GdeZ(float*, float*, float*, float*, int);

void main(void)
{
	float dato;
	float *p;

	p=&dato;		// El puntero "p" queda apuntando a la variable dato
	

	printf("S T A R T : REFERENCIA DE --PRINTF-- Y --SCANF-- \n");

	// Ingresamos un dato en la tabla
	InputData(p);

	printf("dato: %f \n",dato);


	// Finalizacion
	printf("\n\n\nPulse una tecla para finalizar.\n");
	fflush(stdin);
	getchar();
}


