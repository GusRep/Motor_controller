// Imprime en el display LCD de 3 digitos (requiere de "lcd2.c")
// El display es del tipo 7 segmentos
// Recibe: un INT de un registro
// Retorna: nada
/*-----------------------------------------------------------------
Nota: el apagado del los digitos y el punto decimal no estan implementados !
------------------------------------------------------------------*/

#include<stdio.h>
#include<stdlib.h>
#include "SimulaMotor.h"
#include <windows.h>

#pragma warning (disable:4996)

// Constantes
#define CENTENA 0x01
#define DECENA  0x02
#define UNIDAD  0x04
#define DIG_OFF 0x00		// Apagado del digito LCD
#define ERROR 0x40

// Prototipos
int lcd2(int);


void lcd(int reg)
{
	int c,d,u;

	if(reg>999||reg<(-99))
	{
		printf("\nSupera la cantidad de digitos del display.\n");
		EscribirEnDisplay(CENTENA,ERROR);
		EscribirEnDisplay(DECENA,ERROR);
		EscribirEnDisplay(UNIDAD,ERROR);
	}
	else
	{
		if(reg<0)
		{
			reg=reg*(-1);
			EscribirEnDisplay(CENTENA,ERROR);	// ERROR: -
		}
		else
		{
			c=reg/100;	// Como son INT, pierdo la parte fraccionaria
			c=lcd2(c);
			EscribirEnDisplay(CENTENA,c);
		}

		d=reg/10;
		while(d>9)
		{
			d=d-10;
		}
		d=lcd2(d);
		EscribirEnDisplay(DECENA,d);

		u=reg;
		while(u>99)
		{
			u=u-100;
		}
		while(u>9)
		{
			u=u-10;
		}
		u=lcd2(u);
		EscribirEnDisplay(UNIDAD,u);
		// aqui ya tenemos el digito de las centenas, decena y unidades en el LCD
	}
}

		

