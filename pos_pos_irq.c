// Sistema de control de Posicion SISO por IRQs
// Unicamicamente realimentamos posicion angular.  (yk_grad_o = tita)
// Recibe:  nada
// Retorna: nada

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include "SimulaMotor.h"

// Constantes
#define M	1						// Grado del polinomio u[m]
#define N	0						// Grado del polinomio y[n]
#define TS	200						// Time Sampling 200 [ms]
#define NO_VALIDO		 7777
#define TIMER0			 0
#define	TIMER1			 1

// Variables globales
float _yk_gra_o;
				// Coeficientes del controlador <<Velocidad>>:
float _a[N+1];	// float a[0]=0;		//a[0]<-a[1], a[1]<-a[2]... Porque la ecuación implementeda 
				//							es en diferencias, no en Z.
float _b[M+1];
float _ek_gra[M+1];
float _uk_v[N+1]; // uk[0] tension de exitacion del motor (OUTPUT CONTROLER)

// Prototipos
void loop_irq_pos(void);
void display_pos_irq(void);
void ini_cola(float*, int);



void main(void)
{
	_a[0]=(float)1.0*0;			// Coeficientes para GdeZ
	_a[1]=(float)0.0;
	_b[0]=(float)0.1265;
	_b[1]=(float)-0.1035;

	//Inicializamos las colas:
	ini_cola(_ek_gra,M+1);
	ini_cola(_uk_v,N+1);
	
	CrearSimulador();
	LanzarSimulador();
	Sleep(TS);			// Requerimiento del simulador

	printf("Sistema de control de Posicion SISO por IRQs.\n");

	EstablecerISRTemporizador (TIMER0,TS,loop_irq_pos);
	EstablecerISRTemporizador (TIMER1,TS/10,display_pos_irq);
	
	
	Idle();

}