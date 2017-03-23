// Lazo de control de Velocidad/Posicion SISO en una IRQ
// Unicamicamente realimentamos velocidad.  (yk_rpm_o = tita1punto)
// Recibe:  nada
// Retorna: nada

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include "SimulaMotor.h"

// Constantes
#define M   1                   // Grado del polinomio u[m] Vel
#define N   1                   // Grado del polinomio y[n] Vel
#define MM   1                  // Grado del polinomio u[m] Pos
#define NN   0                  // Grado del polinomio y[n] Pos
#define NO_VALIDO		 7777
#define TACOM_X_MAX_v	 10.0
#define TACOM_Y_MAX_rpm  100.0
#define TACOM_X_MIN_v    -10.0
#define TACOM_Y_MIN_rpm  -100.0
#define TITA_Y_MAX_gra    180.0
#define TITA_Y_MIN_gra    -180.0
#define TITA_X_MAX_v    10.0
#define TITA_X_MIN_v    -10.0
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
extern float _yk_gra_o;
extern float _yk_gra_medido_o;		//para enviar al display
extern float _yk_gra_o_ant;		//valor anterior
extern float _a[N+1];
extern float _b[M+1];
extern float _aa[NN+1];
extern float _bb[MM+1];
extern float _ek_rpm[M+1];
extern float _uk_v[N+1];
extern float _ek_gra[MM+1];
extern float _ukuk_v[NN+1]; 
extern int _Pote_active;
extern float _ref;
extern int _ControlType;  //Velocidad = 0   /   Posicion = 1

// Prototipos
void cola(float*, int, float);
void ini_cola(float*, int);
float lineal (float, float, float, float, float);
float GdeZ(float*, float*, float*, float*, int, int);

void loop_VelPos_irq(void)
{
	int valorLeido_c, u_PWM_c, consig_c;
	float signo=1;

	float consigna_rpm_i;				// ref. de velocidad (IN)
	float consigna_gra_i;				// ref. de posicion angular (IN)
	
	float yma_k_v, ym_k_v, duty_porcentual;
	
	float ek_actual_rpm=0;       // Signal Error (INPUT CONTROLER)
	float ek_actual_gra=0;       // Signal Error (INPUT CONTROLER)

		/////// 1 - Leemos valores de Entrada y Consigna
		// Entrada:
		if(_ControlType==0)	//Velocidad = 0   /   Posicion = 1
		{
			valorLeido_c=LeerEntradaAnalogica(0);			// Entrada 0
			
			//Consigna de Vel:
			if(_Pote_active)  // 1 = Pote (mouse)  /  0 = Numero (teclado)
			{
				consig_c=(int)LeerEntradaAnalogica(2);				// Entrada 2
				consigna_rpm_i=lineal((float)consig_c, TACOM_Y_MAX_rpm, TACOM_Y_MIN_rpm, AD_X_MAX_c, AD_X_MIN_c);	// ref.con el pote
			}
			else
			{
				consigna_rpm_i=(float)_ref;	//ingresada por teclado
			}

		}
		else
		{
			valorLeido_c=LeerEntradaAnalogica(1);			// Entrada 1
						
			//Consigna de Pos:
			if(_Pote_active)  // 1 = Pote (mouse)  /  0 = Numero (teclado)
			{
				consig_c=(int)LeerEntradaAnalogica(2);            // Entrada 2
				consigna_gra_i=lineal((float)consig_c, TITA_Y_MAX_gra, TITA_Y_MIN_gra, AD_X_MAX_c, AD_X_MIN_c);   // ref.con el pote
			}
			else
			{
				consigna_gra_i=(float)_ref;	//ingresada por teclado
			}
		}


////Trabajamos con un controlador o con el otro dependiendo del tipo de control deseado.

	if(_ControlType==0)	//Velocidad = 0   /   Posicion = 1
	{
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
	}
	else
	{
		/////// 2 - Obtenemos yk_gra_o (OUT)
      // Convertimos de A/D a V analog adaptada (0V a 5V)
      yma_k_v=lineal((float)valorLeido_c, AD_Y_MAX_v, AD_Y_MIN_v, AD_X_MAX_c, AD_X_MIN_c);

      // Convertimos de V analog adaptada (0V a 5V) a V medida (-10V a 10V)
      ym_k_v=lineal(yma_k_v, ADAPT_Y_MAX_v, ADAPT_Y_MIN_v, ADAPT_X_MAX_v, ADAPT_X_MIN_v);

      // Convertimos de V medida (-10V a 10V) a Velocidad de salida [rpm]
      _yk_gra_o=lineal(ym_k_v, TITA_Y_MAX_gra, TITA_Y_MIN_gra, TITA_X_MAX_v, TITA_X_MIN_v);
      

      /////// 3 - Sumador --> Signal Error (ek_gra)
      
	  // Adecuacion por limite en +/-180º
	  _yk_gra_medido_o=_yk_gra_o;
	  if((_yk_gra_o-_yk_gra_o_ant)<=-180)
	  {
		  _yk_gra_o=_yk_gra_o+360;
		 
	  }
	  if((_yk_gra_o-_yk_gra_o_ant)>=180)
	  {
		  _yk_gra_o=_yk_gra_o-360;
		  
	  }
	
	  ek_actual_gra=consigna_gra_i-_yk_gra_o;  //Senal de error adecuada
		_yk_gra_o_ant=_yk_gra_o;

      /////// 4 - Desplaza tablas temporales 
      cola(_ek_gra, MM+1,ek_actual_gra);
      cola(_uk_v, NN+1, NO_VALIDO);   // NO_VALIDO será sobreescrito por GdeZ (uk_actual_v)   


      /////// 5 - Regulador (GdeZ)
      // Prototipo: float GdeZ(float *b, float *a, float *u, float *y, int m, int n)
      //                                   ek_rpm    uk_v
      _uk_v[0]=GdeZ(_bb, _aa, _ek_gra, _uk_v, MM+1, NN+1);
      

      /////// 6 - Convertimos de Vmotor deseada [-10V a +10V] a Duty Cycle[%]
      duty_porcentual=lineal(*_uk_v, DUTY_MAX, DUTY_MIN, TITA_X_MAX_v, TITA_X_MIN_v);
      
      if(duty_porcentual>100)      // Acotamos la salida del PWM
      {
         //printf("duty_porcentual = %f\n",duty_porcentual);
         duty_porcentual=100;
      }
      if(duty_porcentual<0)
      {
         //printf("duty_porcentual = %f\n",duty_porcentual);
         duty_porcentual=0;
      }
	}


		/////// 7 - PWM
		u_PWM_c=(int)lineal(duty_porcentual, PWM_Y_MAX_c, PWM_Y_MIN_c, DUTY_MAX, DUTY_MIN);
		EscribirSalidaPwm((int)u_PWM_c, PWM_Y_MAX_c);
}
