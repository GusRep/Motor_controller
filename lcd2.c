// Convierte un numero decimal de 0 a 9 en codigo 7 segmentos 
// El display es del tipo 7 segmentos
// Posee DP:punto decimal, APAGADO del digito y ERROR
// Recibe: un numero en INT
// Retorna: un cofigo 7 seg en INT
/*-----------------------------------------------------------------
Nota: el apagado del los digitos y el punto decimal no estan implementados !
------------------------------------------------------------------*/

#include<stdio.h>
#include<stdlib.h>

#pragma warning (disable:4996)

// Constantes
#define N0 0x3F
#define N1 0x06
#define N2 0x5B
#define N3 0x4F
#define N4 0x66
#define N5 0x6D
#define N6 0x7D
#define N7 0x07
#define N8 0x7F
#define N9 0x67
#define COMA 0x80
#define N_OFF 0x00
#define ERROR 0x40


int lcd2(int n)
{
	int code[10];
	*(code+0)=N0;
	*(code+1)=N1;
	*(code+2)=N2;
	*(code+3)=N3;
	*(code+4)=N4;
	*(code+5)=N5;
	*(code+6)=N6;
	*(code+7)=N7;
	*(code+8)=N8;
	*(code+9)=N9;

	return code[n];
}


