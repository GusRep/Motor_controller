// Test del InputVec.
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


void main(void)
{
	int i;
	float vec[N];
	float *p;

	p=&vec[0];		// El puntero "p" queda apuntando al vector
	

	printf("S T A R T :Test del InputVec. \n");

	// Ingresamos un dato en la tabla
	InputVec(p,N);

	for(i=0;i<N;i++)
	{
		printf("dato: %f \n",vec[i]);
	}


	// Finalizacion
	printf("\n\n\nPulse una tecla para finalizar.\n");
	fflush(stdin);
	getchar();
}