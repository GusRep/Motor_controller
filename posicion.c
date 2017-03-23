// Sistema de control de Posicion SISO
// Unicamicamente realimentamos posicion angular. (yk_grad_o = tita)
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include "SimulaMotor.h"

// Constantes
#define L	8
#define TS	200						// Time Sampling 200 [ms]
#define NO_VALIDO		 7777
#define TACOM_X_MAX_v	 10.0
#define TACOM_Y_MAX_rpm  100.0
#define TACOM_X_MIN_v    -10.0
#define TACOM_Y_MIN_rpm  -100.0
#define TITA_Y_MAX_gra	 180.0
#define TITA_Y_MIN_gra	 -180.0
#define TITA_X_MAX_v	 10.0
#define TITA_X_MIN_v    -10.0
#define ADAPT_X_MAX_v    5.0
#define ADAPT_Y_MAX_v    10.0
#define ADAPT_X_MIN_v    0.0
#define ADAPT_Y_MIN_v    -10.0
#define AD_X_MAX_c       1023
#define AD_Y_MAX_v       5.0
#define AD_X_MIN_c       0
#define AD_Y_MIN_v       0.0
#define PWM_X_MAX_v      10.0		
#define PWM_Y_MAX_c      255		// al tun-tun  Por ej. 2^8bits
#define PWM_X_MIN_v      -10.0		
#define PWM_Y_MIN_c      0			// al tun-tun
#define DUTY_MAX		 100.0		// [%]
#define DUTY_MIN		 0.0		// [%]
  
// Prototipos
void lcd(int);
void cola(float*, int, float);
float lineal (float, float, float, float, float);
float GdeZ(float*, float*, float*, float*, int, int);


main()
{
	float consigna_gra_i;				// ref. angular en Grados Sexagesimales (IN)
	
	int i, valorLeido_c, u_PWM_c, consig_c;
	
	float yma_k_v, ym_k_v, yk_gra_o, duty_porcentual;

	// Signal Error (INPUT CONTROLER)
	float ek_actual_gra=0;
	float ek_gra[L];				
	float *p_ek_gra;
	// uk[0] tension de exitacion del motor (OUTPUT CONTROLER)
	float uk_v[L];				
	float *p_uk_v;

	float *p_a;
	float *p_b;


	// Coeficientes del controlador <<Posicion>>:
	int m=2;				// sizeof(b[]);
	int n=2;				// sizeof(a[]);	// El primer coef. debe ser 0 porque es en diferencias

	float a[2];				//float a[0]=0;		//a[0]<-a[1], a[1]<-a[2]... Porque la ecuación implementeda 
							       	    //							es en diferencias, no en Z.
	float b[2];

	a[0]=(float)1.0*0;			// Coeficientes para GdeZ
	a[1]=(float)0.0;
	b[0]=(float)0.1265;
	b[1]=(float)0.1035;

	
	// punteros a colas
	p_ek_gra=&ek_gra[0];			
	p_uk_v=&uk_v[0];

	// punteros a los coeficientes
	p_a=&a[0];
	p_b=&b[0];

	//Inicializamos las colas:
	for(i=0;i<L;i++)
	{
		ek_gra[i]=0.0;
		uk_v[i]=0.0;
	}


	CrearSimulador();
	LanzarSimulador();
	Sleep(TS);			// Requerimiento del simulador

	////EscribirSalidaPwm(cuentaPWMdeseada,cuentaPWMmax);
	//EscribirSalidaPwm(1000,4000);

	

	while(1)
	{
		/////// 1 - Leemos valores
		valorLeido_c=LeerEntradaAnalogica(1);	// Entrada 1
		//printf("Valor de Posicion = %d \n",valorLeido);
		
		consig_c=LeerEntradaAnalogica(2);
		consigna_gra_i=lineal((float)consig_c, TITA_Y_MAX_gra, TITA_Y_MIN_gra, AD_X_MAX_c, AD_X_MIN_c);	// ref.con el pote

		//Escribimos en el display de 7 segmentos
		lcd(valorLeido_c);
		

		/////// 2 - Obtenemos yk_gra_o (OUT)
		// Convertimos de A/D a V analog adaptada (0V a 5V)
		yma_k_v=lineal((float)valorLeido_c, AD_Y_MAX_v, AD_Y_MIN_v, AD_X_MAX_c, AD_X_MIN_c);

		// Convertimos de V analog adaptada (0V a 5V) a V medida (-10V a 10V)
		ym_k_v=lineal(yma_k_v, ADAPT_Y_MAX_v, ADAPT_Y_MIN_v, ADAPT_X_MAX_v, ADAPT_X_MIN_v);

		// Convertimos de V medida (-10V a 10V) a Posicion de salida [grados]
		yk_gra_o=lineal(ym_k_v, TITA_Y_MAX_gra, TITA_Y_MIN_gra, TITA_X_MAX_v, TITA_X_MIN_v);


		/////// 3 - Sumador --> Signal Error (ek_rpm)
		ek_actual_gra=consigna_gra_i-yk_gra_o;


		/////// 4 - Desplaza tablas temporales 
		cola(p_ek_gra, L,ek_actual_gra);
		cola(p_uk_v, L, NO_VALIDO);	// NO_VALIDO será sobreescrito por GdeZ (uk_actual_v)	


		/////// 5 - Regulador (GdeZ)
		// Prototipo: float GdeZ(float *b, float *a, float *u, float *y, int m, int n)
		//											 p_ek_rpm  p_uk_v
		uk_v[0]=GdeZ(p_b, p_a, p_ek_gra, p_uk_v, m, n);
		

		/////// 6 - Convertimos de Vmotor deseada [-10V a +10V] a Duty Cycle[%]
		duty_porcentual=lineal(*p_uk_v, DUTY_MAX, DUTY_MIN, TITA_X_MAX_v, TITA_X_MIN_v);
		
		if(duty_porcentual>100)		// Acotamos la salida del PWM
		{
			printf("duty_porcentual = %f\n",duty_porcentual);
			duty_porcentual=100;
		}
		if(duty_porcentual<0)
		{
			printf("duty_porcentual = %f\n",duty_porcentual);
			duty_porcentual=0;
		}

		/////// 7 - PWM
		u_PWM_c=(int)lineal(duty_porcentual, PWM_Y_MAX_c, PWM_Y_MIN_c, DUTY_MAX, DUTY_MIN);
		EscribirSalidaPwm((int)u_PWM_c, PWM_Y_MAX_c);

		Sleep(TS);

	}

	fflush(stdin);
	getchar();

}