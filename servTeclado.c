// servTeclado: (NO BLOQUEANTE) mira las teclas pulsadas y las almacena en un Buffer global.
// Recibe: nada  (actua con globales)
// Retorna: nada (actua sobre globales)

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <windows.h>
#include "SimulaMotor.h"

//Variables globales
extern char _buffer[100];


#pragma warning (disable:4996)

//Prototipos
void procesaCadena (void);

void servTeclado (void)
{
	char ult_caracter[2];

	// Detectamos si se presiono ENTER (retorno de carro)
	if( _RX_SerialComm==13 || _RX_SerialComm==10 )
	{
		//procesaCadena();			//////////////
	}
	else
	{
		ult_caracter[0]=_RX_SerialComm;
		ult_caracter[1]=0;					// null
		strcat(_buffer,ult_caracter);
	}
}
