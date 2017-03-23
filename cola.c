// cola: cola FIFO.
// 
// Recibe: puntero al vector de Stack previamente definido, largo del Stack, dato a ingresar
// Retorna: nada

#include<stdio.h>
#include<stdlib.h>

#pragma warning (disable:4996)


void cola(float *tabla, int l, float dataNew)
{
	int i;
	// SHIFT los datos de la cola FIFO (poco eficiente con muchos datos)
	for (i=l-1;i>0;i--)
	{
		tabla[i]=tabla[i-1];
	}
	// Insertamos el dataNew
	tabla[0]=dataNew;

}