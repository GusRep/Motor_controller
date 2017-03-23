// EjemploFP: ejemplo de uso de macros de Fixed Point (Punto fijo o Coma fija)
// ========== Tambien se hace uso de estructuras para variables.

#include <stdio.h>
#include <stdlib.h>

#define Q   8                 //Resolucion de FixedPoint = 2^Q
#define M   1                  // Grado del polinomio u[m] Vel
#define N   1                  // Grado del polinomio y[n] Vel
#define MM   1                  // Grado del polinomio u[m] Pos
#define NN   0                  // Grado del polinomio y[n] Pos

//Macros
#define FP(x,q)			((x)*(1<<(q)))          // FixedPoint (Res: 2^q)
#define FPtoFLOAT(x,q)	((float)(x)/(1<<(q)))
#define PROD_FP(a,b,q)  (((a)*(b))>>(q))
#define DIV_FP(a,b,q)	(((a)<<(q))/(b))


//Estructuras
struct _GdeZvel
   {int b[M+1];
    int a[N+1];
    int n, m;
   }_GdeZvel;
struct _GdeZpos
   {int bb[MM+1];
    int aa[NN+1];
    int nn, mm;
   }_GdeZpos;

void main(void)
{
 float num1=1.234567;
 float num2=2.5;
 float rta=0.0;
 float rtaConvert=0.0;
 int rta_FP;
 int num1_FP, num2_FP;
 float rta_div=0.0;			//division
 float rtaConvert_div=0.0;
 int rta_div_FP;
 
 num1_FP=FP(num1,Q);
 num2_FP=FP(num2,Q);

 // Coeficientes para el control de Velocidad
 _GdeZvel.a[0]=(int)FP(0.0,Q)*0;        
 _GdeZvel.a[1]=(int)FP(-1.0,Q);
 _GdeZvel.b[0]=(int)FP(0.6482,Q);
 _GdeZvel.b[1]=(int)FP(-0.5304,Q);
 
 printf("\n\n// Coeficientes para el control de Velocidad:\n");
 printf("a[0]=(0.0,Q)*0		-->num*(2^Q) -->	%d \n",_GdeZvel.a[0]);
 printf("a[1]=(-1.0,Q)		-->num*(2^Q) -->	%d \n",_GdeZvel.a[1]);
 printf("b[0]=(0.6482,Q)	-->num*(2^Q) -->	%d \n",_GdeZvel.b[0]);
 printf("b[1]=(-0.5304,Q)	-->num*(2^Q) -->	%d \n",_GdeZvel.b[1]);


 // Coeficientes para el control de Posicion
 _GdeZpos.aa[0]=(int)FP(1.0,Q)*0;        
 _GdeZpos.aa[1]=(int)FP(0.0,Q);
 _GdeZpos.bb[0]=(int)FP(0.1265,Q);
 _GdeZpos.bb[1]=(int)FP(0.1035,Q);

 printf("\n\n// Coeficientes para el control de Velocidad:\n");
 printf("aa[0]=(1.0,Q)*0	-->num*(2^Q) -->		%d \n",_GdeZpos.aa[0]);
 printf("aa[1]=(0.0,Q)		-->num*(2^Q) -->		%d \n",_GdeZpos.aa[1]);
 printf("bb[0]=(0.1265,Q)	-->num*(2^Q) -->		%d \n",_GdeZpos.bb[0]);
 printf("bb[1]=(0.1035,Q)	-->num*(2^Q) -->		%d \n",_GdeZpos.bb[1]);
 
 
 printf("\nProbamos el producto: Fixed Point con incertidumbre (+/-)1/2^Q\n");
 rta=num1*num2;						            // producto en float
 rta_FP=PROD_FP(num1_FP,num2_FP,Q);     		// producto en int
 rtaConvert=FPtoFLOAT(rta_FP,Q);
 printf("%f x %f = %f ----> In Fixed Point: %f \n", num1, num2, rta, rtaConvert);

 printf("\nProbamos la division: Fixed Point con incertidumbre (+/-)1/2^Q\n");
 rta_div=num1/num2;						            // producto en float
 rta_div_FP=DIV_FP(num1_FP,num2_FP,Q);     		// producto en int
 rtaConvert_div=FPtoFLOAT(rta_div_FP,Q);
 printf("%f / %f = %f ----> In Fixed Point: %f \n", num1, num2, rta_div, rtaConvert_div);

 // Finalizacion
   printf("\n\n\nPulse una tecla para finalizar.\n");
   fflush(stdin);
   getchar();
}
