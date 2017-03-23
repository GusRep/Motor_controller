// Sistema de control de Velocidad/Posicion SISO por IRQs
// Modo de uso: 
// -tipeando "vel"  = selecciona el control de velocidad
// -tipeando "pos"  = selecciona el control de posicion
// -tipeando "potoff" = ingresar referencia por teclado
// -tipeando "poton" = ingresar referencia con el mouse
// -tipeando "REFn" siendo "n" un valor numerico
//
// Recibe:  nada
// Retorna: nada
// Nota: soluciona el descontrol con la ref +/- 180 grados
// =====
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include "SimulaMotor.h"

// Constantes
#define M   1                   // Grado del polinomio u[m] Vel
#define N   1                   // Grado del polinomio y[n] Vel
#define MM   1                  // Grado del polinomio u[m] Pos
#define NN   0                  // Grado del polinomio y[n] Pos
#define TS   200                  // Time Sampling 200 [ms]
#define NO_VALIDO       7777
#define TIMER0          0
#define TIMER1          1

// Variables globales
float _yk_rpm_o;
float _yk_gra_o;
float _yk_gra_medido_o;      //para enviar al display
float _yk_gra_o_ant=0;      //valor anterior

            // Coeficientes del controlador <<Velocidad>>:
float _a[N+1];   // float a[0]=0;      //a[0]<-a[1], a[1]<-a[2]... Porque la ecuación implementeda 
            //                     es en diferencias, no en Z.
float _b[M+1];
            // Coeficientes del controlador <<Posicion>>:
float _aa[NN+1];   // float a[0]=0;      //a[0]<-a[1], a[1]<-a[2]... Porque la ecuación implementeda 
            //                     es en diferencias, no en Z.
float _bb[MM+1];

float _ek_rpm[M+1];
float _ek_gra[MM+1];

float _uk_v[N+1]; // uk[0] tension de exitacion del motor (OUTPUT CONTROLER) Vel
float _ukuk_v[NN+1];   // idem Pos

char _vectString[11];  // Buffer de hasta 10 caracteres + NULL
int _ControlType=0;      //Velocidad = 0   /   Posicion = 1
int _Pote_active=1;      // 1 = Pote (mouse)  /  0 = Numero (teclado)
float _ref;            // Referencia

// Prototipos
void loop_VelPos_irq(void);
void display_VelPos_irq(void);
void ini_cola(float*, int);
void LecturaPantalla(void);
int AdquiereValor(char*, char*);


void main(void)
{
   _a[0]=(float)1.0*0;         // Coeficientes para Vel
   _a[1]=(float)-1.0;
   _b[0]=(float)0.6482;
   _b[1]=(float)-0.5304;

   _aa[0]=(float)1.0*0;         // Coeficientes para Pos
   _aa[1]=(float)0.0;
   _bb[0]=(float)0.1265;
   _bb[1]=(float)-0.1035;

   //Inicializamos las colas:  (Vel)
   ini_cola(_ek_rpm,M+1);
   ini_cola(_uk_v,N+1);
   
   //Inicializamos las colas:  (Pos)
   ini_cola(_ek_gra,MM+1);
   ini_cola(_ukuk_v,NN+1);

   //Inicializamos el vectString
   _vectString[0]=0;

   CrearSimulador();
   LanzarSimulador();
   Sleep(TS);         // Requerimiento del simulador

   //Mensajes:
   printf("Sistema de control de Velocidad/Posicion SISO por IRQs\n");
   printf("Modo de uso:\n");
   printf("-tipeando < vel >  = selecciona el control de velocidad\n");
   printf("-tipeando < pos >  = selecciona el control de posicion\n");
   printf("-tipeando < potoff > = ingresar referencia por teclado\n");
   printf("-tipeando < poton > = ingresar referencia con el mouse\n");
   printf("-tipeando < REFn > siendo < n > un valor numerico\n");
   printf("\n");

   EstablecerISRTeclado(LecturaPantalla);
   EstablecerISRTemporizador (TIMER0,TS,loop_VelPos_irq);
   EstablecerISRTemporizador (TIMER1,TS/10,display_VelPos_irq);
   
   
   Idle();

}
