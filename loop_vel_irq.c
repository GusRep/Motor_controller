// Lazo de control de Velocidad SISO en una IRQ
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
#define NO_VALIDO		 7777
#define TACOM_X_MAX_v	 10.0
#define TACOM_Y_MAX_rpm  100.0
#define TACOM_X_MIN_v    -10.0
#define TACOM_Y_MIN_rpm  -100.0
#define AD_X_MAX_c       1023
#define AD_Y_MAX_v       5.0
#define AD_X_MIN_c       0
#define AD_Y_MIN_v       0.0
#define ADAPT_X_MAX_v    5.0
#define ADAPT_Y_MAX_v    10.0
#define ADAPT_X_MIN_v    0.0
#define ADAPT_Y_MIN_v    -10.0
#define DUTY_MAX		 100.0		// [%]
#define DUTY_MIN		 0.0		// [%]
#define PWM_X_MAX_v      10.0		
#define PWM_Y_MAX_c      4095		// al tun-tun  Por ej. 2^12bits
#define PWM_X_MIN_v      -10.0		
#define PWM_Y_MIN_c      0			// al tun-tun

// Variables globales
extern float _yk_rpm_o;
extern float _a[N+1];
extern float _b[M+1];
extern float _ek_rpm[M+1];
extern float _uk_v[N+1]; 
extern int _Pote_active;
extern int _ref;

// Prototipos
void cola(float*, int, float);
void ini_cola(float*, int);
float lineal (float, float, float, float, float);
float GdeZ(float*, float*, float*, float*, int, int);

void loop_vel_irq(void)
{
	int valorLeido_c, u_PWM_c, consig_c;

	float consigna_rpm_i;				// ref. de velocidad (IN)
	
	float yma_k_v, ym_k_v, duty_porcentual;
	
	float ek_actual_rpm=0;       // Signal Error (INPUT CONTROLER)


		/////// 1 - Leemos valores
		valorLeido_c=LeerEntradaAnalogica(0);			// Entrada 0
		//printf("Valor de Velocidad = %d \n",valorLeido);
		
		if(_Pote_active)  // 1 = Pote (mouse)  /  0 = Numero (teclado)
		{
			consig_c=LeerEntradaAnalogica(2);				// Entrada 2
			consigna_rpm_i=lineal((float)consig_c, TACOM_Y_MAX_rpm, TACOM_Y_MIN_rpm, AD_X_MAX_c, AD_X_MIN_c);	// ref.con el pote
		}
		else
		{
			consigna_rpm_i=_ref;
		}
		

			
		/////// 2 - Obtenemos yk_rpm_o (OUT)
		// Convertimos de A/D a V analog adaptada (0V a 5V)
		yma_k_v=lineal((float)valorLeido_c, AD_Y_MAX_v, AD_Y_MIN_v, AD_X_MAX_c, AD_X_MIN_c);

		// Convertimos de V analog adaptada (0V a 5V) a V medida (-10V a 10V)
		ym_k_v=lineal(yma_k_v, ADAPT_Y_MAX_v, ADAPT_Y_MIN_v, ADAPT_X_MAX_v, ADAPT_X_MIN_v);

		// Convertimos de V medida (-10V a 10V) a Velocidad de salida [rpm]
		_yk_rpm_o=lineal(ym_k_v, TACOM_Y_MAX_rpm, TACOM_Y_MIN_rpm, TACOM_X_MAX_v, TACOM_X_MIN_v);
		

		/////// 3 - Sumador --> Signal Error (ek_rpm)
		ek_actual_rpm=consigna_rpm_i-_yk_rpm_o;


		/////// 4 - Desplaza tablas temporales 
		cola(_ek_rpm, M+1,ek_actual_rpm);
		cola(_uk_v, N+1, NO_VALIDO);	// NO_VALIDO será sobreescrito por GdeZ (uk_actual_v)	


		/////// 5 - Regulador (GdeZ)
		// Prototipo: float GdeZ(float *b, float *a, float *u, float *y, int m, int n)
		//											  ek_rpm    uk_v
		_uk_v[0]=GdeZ(_b, _a, _ek_rpm, _uk_v, M+1, N+1);
		

		/////// 6 - Convertimos de Vmotor deseada [-10V a +10V] a Duty Cycle[%]
		duty_porcentual=lineal(*_uk_v, DUTY_MAX, DUTY_MIN, TACOM_X_MAX_v, TACOM_X_MIN_v);
		
		if(duty_porcentual>100)		// Acotamos la salida del PWM
		{
			//printf("duty_porcentual = %f\n",duty_porcentual);
			duty_porcentual=100;
		}
		if(duty_porcentual<0)
		{
			//printf("duty_porcentual = %f\n",duty_porcentual);
			duty_porcentual=0;
		}

		/////// 7 - PWM
		u_PWM_c=(int)lineal(duty_porcentual, PWM_Y_MAX_c, PWM_Y_MIN_c, DUTY_MAX, DUTY_MIN);
		EscribirSalidaPwm((int)u_PWM_c, PWM_Y_MAX_c);
}
