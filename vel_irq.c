// Sistema de control de Velocidad SISO por IRQs
// Unicamicamente realimentamos velocidad.  (yk_rpm_o = tita1punto)
// Recibe:  nada
// Retorna: nada

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include "SimulaMotor.h"

// Constantes
#define M	1						// Grado del polinomio u[m]
#define N	1						// Grado del polinomio y[n]
#define TS	200						// Time Sampling 200 [ms]
#define NO_VALIDO		 7777
#define TIMER0			 0
#define	TIMER1			 1

// Variables globales
float _yk_rpm_o;
				// Coeficientes del controlador <<Velocidad>>:
float _a[N+1];	// float a[0]=0;		//a[0]<-a[1], a[1]<-a[2]... Porque la ecuación implementeda 
				//							es en diferencias, no en Z.
float _b[M+1];
float _ek_rpm[M+1];
float _uk_v[N+1]; // uk[0] tension de exitacion del motor (OUTPUT CONTROLER)
char _vectString[10];  // Buffer de 10 caracteres
int _ControlType=0;  //Velocidad = 0   /   Posicion = 1
int _Pote_active=1;		// 1 = Pote (mouse)  /  0 = Numero (teclado)
int _ref;				// Referencia

// Prototipos
void loop_vel_irq(void);
void display_vel_irq(void);
void ini_cola(float*, int);
void LecturaPantalla(void);
int AdquiereValor(char*, char*);


void main(void)
{
	_a[0]=(float)0.0*0;			// Coeficientes para GdeZ
	_a[1]=(float)-1.0;
	_b[0]=(float)0.6482;
	_b[1]=(float)-0.5304;

	//Inicializamos las colas:
	ini_cola(_ek_rpm,M+1);
	ini_cola(_uk_v,N+1);
	
	//Inicializamos el vectString
	_vectString[0]=0;

	CrearSimulador();
	LanzarSimulador();
	Sleep(TS);			// Requerimiento del simulador

	printf("Sistema de control de Velocidad SISO por IRQs\n");

	EstablecerISRTeclado(LecturaPantalla);
	EstablecerISRTemporizador (TIMER0,TS,loop_vel_irq);
	EstablecerISRTemporizador (TIMER1,TS/10,display_vel_irq);
	
	
	Idle();

}