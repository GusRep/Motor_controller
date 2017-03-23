// InputData: solicita ingresar un dato por consola.
// Recibe: un puntero a la variable donde guardar el dato
// Retorna: nada (almacena el dato en la variable)

#include<stdio.h>
#include<stdlib.h>

#pragma warning (disable:4996)


void InputData(float *pointer)
{
	printf("Ingrese un dato:\n");  // guardo en lo apuntado por "pointer"
	scanf("%f",pointer);
}
