// ProdEsc: realiza el producto escalar entre dos vectores.
// Recibe: dos vectores y la longitud de los mismos
// Retorna: un resultado escalar

#include<stdio.h>
#include<stdlib.h>

#pragma warning (disable:4996)


float ProdEsc(float *vec1, float *vec2, int l)
{
	float acc=0;
	float acc2;
	int i;
	for(i=0;i<l;i++)
	{
		acc2=vec1[i]*vec2[i];
		acc=acc+acc2;
	}
	return acc;
}

