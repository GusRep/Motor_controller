// InputVec: solicita ingresar los datos de un vector por consola.
// Recibe: un puntero al vector, largo del vector
// Retorna: nada (almacena los datos en el vector)

#include<stdio.h>
#include<stdlib.h>

#pragma warning (disable:4996)

// Prototipos
void InputData(float*);


void InputVec(float *pointer, int l)
{
	int i;
	for (i=0;i<l;i++)
	{
		InputData((pointer+i));
	}
	printf("Se ingresaron %d datos en el vector.\n",l);
}

