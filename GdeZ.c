// GdeZ: Función transferencia en el dominio Z  <<< G[z] >>>.
// Recibe: un vector de coeficientes		 b[m]		Long: M
//		   un vector de coeficientes         a[n]		Long: N
//		   un vector de entradas temporales  u			
//		   un vertor de salidas anteriores   y			
//		   largo de los vectores (valor de M)
//		   largo de los vectores (valor de N)
// Retorna: salida del sistema
//------------------------------------------------------------------------------------
// En n (tiempo discreto):
//
//  y[k] = b[0]*u[k] + b[1]*u[k-1] + ... + b[m]*u[k-m]-a[1]*y[k-1] - ... - a[n]*y[k-n]
//
//  Siendo: u[m] las entradas temporales
//			y[n] las salidas temporales
//
//
// En Z (plano Z):
//
//          Y[z]    b0 + b1.z^-1 + b2.z^-2 + ... + bm.z^-m
//  G[z] = ----- = ----------------------------------------
//          U[z]    1 + a1.z^-1 + a2.z^-2 + ... + an.z^-n
//
//  Siendo: U[z] las entradas transformadas
//			Y[z] las salidas transformadas
//


#include<stdio.h>
#include<stdlib.h>

#pragma warning (disable:4996)

// Prototipos
float ProdEsc(float*,float*,int);


float GdeZ(float *b, float *a, float *u, float *y, int m, int n)
{
	float primero,segundo,out;
	
	primero=ProdEsc(b,u,m);
	segundo=ProdEsc(a,y,n);  // los coeficientes de segundo, se restan
	out=primero-segundo;
	return out;
}


