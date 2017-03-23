// Escribimos un valor en el display por IRQ.
// Utiliza funciones previas como "lcd" y "lcd2".
// Recibe:  nada
// Retorna: nada

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include "SimulaMotor.h"

// Variables globales
extern float _yk_gra_o;			

//Prototipos
void lcd(int);

void display_pos_irq(void)
{
	//Escribimos en el display de 7 segmentos
	lcd((int)_yk_gra_o);
}


