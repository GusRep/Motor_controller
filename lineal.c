// Convierte un dato de entrada, en otro de salida a traves de una funcion lineal.
// Recibe: dato de entrada_X , Y_max, Y_min, X_max, X_min  <<floats>>
// Retorna: dato convertido_Y (float)

#include<stdio.h>
#include<stdlib.h>

#pragma warning (disable:4996)

float lineal (float Xin, float Ymax, float Ymin, float Xmax, float Xmin)
{
	float Yout;

	//if((Xin>Xmax)|(Xin<Xmin))		//comprobacion de error
	//	printf("\nLa entrada esta fuera de rango!!! %f\n",Xin);

	Yout=Ymin+(Xin-Xmin)*(Ymax-Ymin)/(Xmax-Xmin);

	return Yout;
}