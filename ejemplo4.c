// Test de la cola FIFO.
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
float ProdEsc(float*, float*, int);
float GdeZ(float*, float*, float*, float*, int);


void main(void)
{
	int i;
	float stack[N];
	float *p;
	float dato_para_insertar=777.777;

	p=&stack[0];		// El puntero "p" queda apuntando al stack
			

	printf("S T A R T :Test de la cola FIFO. \n");

	// Inicializo el stack con numeros consecutivos 0,1,2,3,...
	for(i=0;i<N;i++)
	{
		stack[i]=i;    
		//*(p+i)=i;		// otra forma de direccionar usando punteros
	}
	
	// Imprimimos el vector en pantalla
	printf("Vector original. \n");
	for(i=0;i<N;i++)
	{
		printf("dato: %f \n",stack[i]);
	}


	// Insertamos "dato_para_insertar"
	cola(p, N, dato_para_insertar);


	// Imprimimos el nuevo vector 
	printf("\n\nVector con dato nuevo. \n");
	for(i=0;i<N;i++)
	{
		printf("dato: %f \n",stack[i]);
	}
	

	// Finalizacion
	printf("\n\n\nPulse una tecla para finalizar.\n");
	fflush(stdin);
	getchar();
}