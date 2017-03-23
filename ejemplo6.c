// Test de servTeclado. Cada 2 seg muestra un buffer string q almacena las
// teclas pulsadas.
// Recibe: nada  
// Retorna: nada 

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <windows.h>
#include "SimulaMotor.h"

//Variables globales
char _buffer[100];
float _data1, _data2, _data3;

#pragma warning (disable:4996)

//Prototipos
void servTeclado (void);

void main (void)
{
	_buffer[0]=0;		// Inicializo vacio al buffer
	printf("Start.\n");

	CrearSimulador();
	LanzarSimulador();
	EstablecerISRTeclado(servTeclado);			// IRQ
	
	
	while(1)
	{
		EstablecerISRTeclado(servTeclado);
		printf("%s\n",_buffer);
		Sleep(2000);
	}
}