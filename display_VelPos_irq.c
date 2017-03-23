// Escribimos un valor en el display por IRQ.
// Utiliza funciones previas como "lcd" y "lcd2".
// Recibe:  nada
// Retorna: nada

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include "SimulaMotor.h"

// Variables globales
extern float _yk_rpm_o;
extern float _yk_gra_medido_o;		//para enviar al display
extern int _ControlType;


//Prototipos
void lcd_centenaMod(int);


void display_VelPos_irq(void)
{
	if(_ControlType==1)
	{
		//Escribimos en el display de 7 segmentos
		lcd_centenaMod((int)_yk_gra_medido_o);
	}
	else
	{
		//Escribimos en el display de 7 segmentos
		lcd_centenaMod((int)_yk_rpm_o);
	}
}


