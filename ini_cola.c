// ini_cola: inicializa arrays tipo folat
// 
// Recibe: puntero al vector de Stack previamente definido, largo del Stack
// Retorna: nada

#include<stdio.h>
#include<stdlib.h>

#pragma warning (disable:4996)


void ini_cola(float *tabla, int l)
{
	int i;
	for (i=0;i<l;i++)
	{
		tabla[i]=0.0;
	}
}