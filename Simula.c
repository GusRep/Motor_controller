// Ejemplo inicial de utilizacion del simulador.

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include "SimulaMotor.h"

main()
{
	int valorLeido;

	CrearSimulador();
	LanzarSimulador();

	//EscribirSalidaPwm(cuentaPWMdeseada,cuentaPWMmax);
	EscribirSalidaPwm(1000,4000);

	while(1)
	{
		valorLeido=LeerEntradaAnalogica(0);	// Entrada 0
		printf("Valor de Velocidad = %d \n",valorLeido);
		Sleep(2000);
	}

	fflush(stdin);
	getchar();

}