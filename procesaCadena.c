//NO implementada

// procesaCadena: guarda en unas variables globales los datos ingresados en un string.
// EJ.: tipeamos "data1=55.7 data3=77<ENTER>" y guarda en las variables globales 
// corresponsientes los datos ingresados en forma de numeros.
// Recibe: nada  (actua con globales)
// Retorna: nada (actua sobre globales)

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <windows.h>
#include "SimulaMotor.h"

//Variables globales
extern char _buffer[100];
extern float _data1, _data2, _data3;


void procesaCadena (void)
{
	int largo;
	largo=strlen(_buffer);

