// Test de la GdeZ.
// Recibe: nada
// Retorna: nada

#include<stdio.h>
#include<stdlib.h>

#pragma warning (disable:4996)

// Constantes
#define N	10
#define TS	200

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
	float rta_int, rta_der;
	float a_int[1];
	float b_int[1];
	float *p_a_int;
	float *p_b_int;
	float u_int[N];
	float y_int[N];
	float *p_u_int;
	float *p_y_int;

	a_int[0]=-1.0;
	b_int[0]=TS;

	p_a_int=&a_int[0];
	p_y_int=&y_int[0];

	p_u_int=&u_int[0];
	p_y_int=&y_int[0];

	printf("S T A R T : GdeZ - integrador. \n");
	
	rta_int=GdeZ(p_b_int, p_a_int, float *u, float *y, int m, int n)

	printf("S T A R T : GdeZ - derivador. \n");


	// Finalizacion
	printf("\n\n\nPulse una tecla para finalizar.\n");
	fflush(stdin);
	getchar();
}