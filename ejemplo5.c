// Test de la conversion lineal.
// Recibe: nada
// Retorna: nada

#include<stdio.h>
#include<stdlib.h>

#pragma warning (disable:4996)

// Constantes
#define N 8		

// Prototipos
void InputData(float*);
void InputVec(float*, int);
void cola(float*, int, float);
float lineal (float, float, float, float, float);
float ProdEsc(float*, float*, int);
float GdeZ(float*, float*, float*, float*, int);


void main(void)
{
	int i;
	float valor=0;
	float *p;
	float convertido=0;

	float X_MAX=100;
	float X_MIN=-100;
	float Y_MAX=200;
	float Y_MIN=-200;

	p=&valor;

	printf("S T A R T : Test de la conversion lineal. \n");
	
	for(i=0;i<4;i++)
	{
		InputData(p);
		convertido=lineal(valor,Y_MAX,Y_MIN,X_MAX,X_MIN);
		printf("convertido: %f \n\n",convertido);
	}


	// Finalizacion
	printf("\n\n\nPulse una tecla para finalizar.\n");
	fflush(stdin);
	getchar();
}