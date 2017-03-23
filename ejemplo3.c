// Test del ProdEsc.
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
	int i;
	float vec_A[N];
	float vec_B[N];
	float *pA;
	float *pB;
	float rta=0;

	pA=&vec_A[0];		// El puntero "pA" queda apuntando al vec_A
	pB=&vec_B[0];
		

	printf("S T A R T :Test del ProdEsc. \n");

	// Ingresamos datos en los vectores
	for(i=0;i<N;i++)
	{
		vec_A[i]=i;
		vec_B[i]=100*i;
	}
	
	rta=ProdEsc(pA, pB, N);
	
	printf("rta: %f \n",rta);
	


	// Finalizacion
	printf("\n\n\nPulse una tecla para finalizar.\n");
	fflush(stdin);
	getchar();
}