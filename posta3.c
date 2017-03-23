/* Archivo unico.							  AUTOR: GUSTAVO D. GIL
											  FECHA: 27/11/2010
Control de velocidad por IRQ en FixedPoint.

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
*/
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include "SimulaMotor.h"

#pragma warning (disable:4996)

// Constantes
#define MMAX	8				//Max cantidad de coeficientes de los controladores
#define NMAX	8

#define M    1                  // Grado del polinomio u[m] Vel
#define N    1                  // Grado del polinomio y[n] Vel
#define MM   1                  // Grado del polinomio u[m] Pos
#define NN   0                  // Grado del polinomio y[n] Pos
#define Q    8                 //Resolucion de FixedPoint = 2^Q
#define QQ	 256
#define TS   200                  // Time Sampling 200 [ms]
#define NO_VALIDO        7777

// Defines en Fixed Point		// En Flotante
#define TACOM_X_MAX_v		QQ*(10.0)	//(int)(10.0*(1<<Q))
#define TACOM_Y_MAX_rpm		QQ*(100.0)
#define TACOM_X_MIN_v		QQ*(-10.0)
#define TACOM_Y_MIN_rpm		QQ*(-100.0)
#define TITA_Y_MAX_gra		QQ*(180.0)
#define TITA_Y_MIN_gra		QQ*(-180.0)
#define TITA_X_MAX_v		QQ*(10.0)
#define TITA_X_MIN_v		QQ*(-10.0)
#define ADAPT_X_MAX_v		QQ*(5.0)
#define ADAPT_Y_MAX_v		QQ*(10.0)
#define ADAPT_X_MIN_v		QQ*(0.0)
#define ADAPT_Y_MIN_v		QQ*(-10.0)
#define AD_X_MAX_c			QQ*(1023)
#define AD_Y_MAX_v			QQ*(5.0)
#define AD_X_MIN_c			QQ*(0)
#define AD_Y_MIN_v			QQ*(0.0)
#define PWM_X_MAX_v			QQ*(10.0)      
#define PWM_Y_MAX_c			QQ*(4095)      // al tun-tun  Por ej. 2^12bits
#define PWM_X_MIN_v			QQ*(-10.0)      
#define PWM_Y_MIN_c			QQ*(0)         // al tun-tun
#define DUTY_MAX			QQ*(100.0)      // [%]
#define DUTY_MIN			QQ*(0.0)      // [%]

#define CENTENA 0x01
#define DECENA  0x02
#define UNIDAD  0x04
#define DIG_OFF 0x00      // Apagado del digito LCD
#define ERROR_LCD 0x40
#define N0 0x3F
#define N1 0x06
#define N2 0x5B
#define N3 0x4F
#define N4 0x66
#define N5 0x6D
#define N6 0x7D
#define N7 0x07
#define N8 0x7F
#define N9 0x67
#define TIMER0          0
#define TIMER1          1

//Macros
#define FP(x,q)			((x)*(1<<(q)))          // FixedPoint (Res: 2^q)
#define FPtoFLOAT(x,q)	((float)(x)/(1<<(q)))
#define FPtoINT(x,q)	((int)(x)/(1<<(q)))
#define PROD_FP(a,b,q)  (((a)*(b))>>(q))
#define DIV_FP(a,b,q)	(((a)<<(q))/(b))


//Estructuras
struct GdeZ
   {int b[MMAX];
    int a[NMAX];
    int n, m;
   }_GdeZvel,_GdeZpos;


// Variables globales
int _ControlType;     // tipo de controldador (en PIC es del tipo bit <<int1>>)
int _Pote_active=1;      // 1 = Pote (mouse)  /  0 = Numero (teclado)
int _ref;            // Referencia

int _yk_rpm_o;
int _yk_gra_o;
int _yk_gra_medido_o;      //para enviar al display
int _yk_gra_o_ant=0;      //valor anterior

int _ek_rpm[MMAX];
int _uk_vel_v[NMAX];   // uk[0] tension de exitacion del motor (OUTPUT CONTROLER)

int _ek_gra[MMAX];
int _uk_pos_v[NMAX];  // uk[0] tension de exitacion del motor (OUTPUT CONTROLER)

int _out_pos_gra;     // salidas actuales 
int _out_vel_rpm;

char _vectString[11];  // Buffer de hasta 10 caracteres + NULL
int _ref;            // Referencia

int _new=0;			// Flags de edicion de los controladores
int _newp=0;
int _newv=0;
int _newListo=0;
   
// Prototipos
void loop_irq_FP(void);
void display_irq_FP(void);		
void ini_cola(int*, int);		
void LecturaPantalla(void);		
int AdquiereValor(char*, char*);
// Prototipos sub-funciones
void lcd_centenaMod(int reg);	
int lcd2(int);					
void cola_FP(int*, int, int); 
int lineal_FP (int, int , int , int, int); 
int ProdEsc_FP(int*, int*, int); 
int GdeZ_FP(struct GdeZ*, int*, int*); 


void main(void)
{
 _ControlType=0;  //Velocidad = 0   /   Posicion = 1 
 
 // Coeficientes para el control de Velocidad
 _GdeZvel.a[0]=(int)FP(0.0,Q)*0;        
 _GdeZvel.a[1]=(int)FP(-1.0,Q);
 _GdeZvel.b[0]=(int)FP(0.6482,Q);
 _GdeZvel.b[1]=(int)FP(-0.5304,Q);

 // Coeficientes para el control de Posicion
 _GdeZpos.a[0]=(int)FP(1.0,Q)*0;        
 _GdeZpos.a[1]=(int)FP(0.0,Q);
 _GdeZpos.b[0]=(int)FP(0.1265,Q);
 _GdeZpos.b[1]=(int)FP(-0.1035,Q);
 
 // Longitudes de los vectores de coeficientes (VEL)
 _GdeZvel.m=M+1;
 _GdeZvel.n=N+1;

 // Longitudes de los vectores de coeficientes (POS)
 _GdeZpos.m=MM+1;
 _GdeZpos.n=NN+1;

 //Inicializamos las colas de Velocidad:
 ini_cola(_ek_rpm,M+1);
 ini_cola(_uk_vel_v,N+1);
    
 //Inicializamos las colas de Posicion:
 ini_cola(_ek_gra,MM+1);
 ini_cola(_uk_pos_v,NN+1);
 
 //Inicializamos el vectString
 _vectString[0]=0;


 CrearSimulador();
 LanzarSimulador();
 Sleep(TS);         // Requerimiento del simulador
  

 //Mensajes:
   printf("Sistema de control de Velocidad/Posicion (Fixed Point) SISO por IRQs\n");
   printf("Modo de uso:\n");
   printf("-tipeando < vel >  = selecciona el control de velocidad\n");
   printf("-tipeando < pos >  = selecciona el control de posicion\n");
   printf("-tipeando < potoff > = ingresar referencia por teclado\n");
   printf("-tipeando < poton > = ingresar referencia con el mouse\n");
   printf("-tipeando < REFn > siendo < n > un valor numerico\n");
   printf("-tipeando < new > = ingresar nuevo controlador");
   printf("\n");

 //Instala las IRQs
 EstablecerISRTeclado(LecturaPantalla);
 EstablecerISRTemporizador(TIMER0,TS,loop_irq_FP);
 EstablecerISRTemporizador(TIMER1,TS/10,display_irq_FP);
   
 Idle();
 
}

// ini_cola: inicializa arrays tipo int
// ============
// Recibe: puntero al vector de Stack, largo del Stack  <Globales> 
// Retorna: nada
void ini_cola(int *tabla, int l)
{
   int i;
   for (i=0;i<l;i++)
   {
      tabla[i]=0;
   }
}

// lcd2: Convierte un numero decimal de 0 a 9 en codigo 7 segmentos 
// =====
// El display es del tipo 7 segmentos
// Posee DP:punto decimal, APAGADO del digito y ERROR
// Recibe: un numero en INT
// Retorna: un cofigo 7 seg en INT
/*-----------------------------------------------------------------
Nota: el apagado del los digitos y el punto decimal no estan implementados !
------------------------------------------------------------------*/
int lcd2(int n)
{
   int code[10];
   *(code+0)=N0;
   *(code+1)=N1;
   *(code+2)=N2;
   *(code+3)=N3;
   *(code+4)=N4;
   *(code+5)=N5;
   *(code+6)=N6;
   *(code+7)=N7;
   *(code+8)=N8;
   *(code+9)=N9;

   return code[n];
}


// display_irq_FP: Escribimos la medicion correspondiente en el display por IRQ
// ===============
// Utiliza funciones previas como "lcd_centenaMod", "lcd" y "lcd2".
// Recibe:  nada
// Retorna: nada
void display_irq_FP(void)
{
   if(_ControlType) //Velocidad = 0   /   Posicion = 1 
      {
       //Escribimos en el display de 7 segmentos
       lcd_centenaMod(_out_pos_gra);
      }
   else
      {
       //Escribimos en el display de 7 segmentos
       lcd_centenaMod(_out_vel_rpm);
      }
}

// lcd_centenaMod:Imprime en el display LCD de 3 digitos (requiere de "lcd2.c")
// ===============
// El display es del tipo 7 segmentos
// Recibe: un INT de un registro
// Retorna: nada
/*-----------------------------------------------------------------
Nota: el apagado del los digitos y el punto decimal no estan implementados !
------------------------------------------------------------------*/
void lcd_centenaMod(int reg)
{
   int c,d,u;

   if(reg>999||reg<(-199))
   {
      printf("\nSupera la cantidad de digitos del display.\n");
      EscribirEnDisplay(CENTENA,ERROR);
      EscribirEnDisplay(DECENA,ERROR);
      EscribirEnDisplay(UNIDAD,ERROR);
   }
   else
   {
      if(reg<0)
      {
         if(reg>=-99)
         {
            EscribirEnDisplay(CENTENA,(ERROR_LCD));   // ERROR_LCD: -
         }
         if(reg<=-100)
         {
            EscribirEnDisplay(CENTENA,(N1|ERROR_LCD));   // -1 (en el 1er digito)
         }
         reg=reg*(-1); // Positivizo para el resto
      }
      else
      {
         c=reg/100;   // Como son INT, pierdo la parte fraccionaria
         c=lcd2(c);
         EscribirEnDisplay(CENTENA,c);
      }

      d=reg/10;
      while(d>9)
      {
         d=d-10;
      }
      d=lcd2(d);
      EscribirEnDisplay(DECENA,d);

      u=reg;
      while(u>99)
      {
         u=u-100;
      }
      while(u>9)
      {
         u=u-10;
      }
      u=lcd2(u);
      EscribirEnDisplay(UNIDAD,u);
      // aqui ya tenemos el digito de las centenas, decena y unidades en el LCD
   }
}

// LecturaPantalla: adquiere caracteres por IRQ y edita las referencias de posicicion
// ================ velocidad, o uso del potenciometro de referencia.
// Recibe: nada
// Retorna: nada
void LecturaPantalla(void)
{  int i=0;
   char *potenciometro; // *controlVel, *controlPos;
   
   if (_RX_SerialComm!=13)   // No se presiono INTRO
   {   printf("Ingresado: %c\n",_RX_SerialComm);    //char
       i=strlen(_vectString);
       _vectString[i]=(_RX_SerialComm);
       _vectString[i+1]=0;
   }
   else                                    // Se presiono INTRO
   {  
      printf("vectString: %s ...\n",_vectString); //string
	  if(_newListo==0)		// Flag para recepción de coeficientes
	  {
		  if(_new==0)		// Flag de edicion del controlador
			{
				if ((strcmp(_vectString,"pos"))==0)      // 0 si son iguales
					{
						_ControlType=1;         //Velocidad = 0   /   Posicion = 1
						printf("Control de Posicion activado!\n");
					}   
				if (strcmp(_vectString,"vel")==0)
					{
						_ControlType=0;         //Velocidad = 0   /   Posicion = 1
						printf("Control de Velocidad activado!\n");
					}
				if (strcmp(_vectString,"poton")==0)      //lectura AQUI
					{
						_Pote_active=1;            // 1 = Pote (mouse)  /  0 = Numero (teclado)
						printf("Usando el potenciometro con el mouse. \n");
					}
				if (strcmp(_vectString,"potoff")==0)      //lectura AQUI
					{
						_Pote_active=0;            // 1 = Pote (mouse)  /  0 = Numero (teclado)
						printf("Ingresar consigna numerica. \n");
					}
				//introduciendo valor de referencia
				_ref=AdquiereValor(_vectString,"REF");
      
				  if (strcmp(_vectString,"new")==0)      //lectura AQUI
					{
						_new=1;	// Flag de edicion del controlador
						printf("\n¿Desea ingresar un nuevo controlador de Posición o de Velocidad? [p] [v]\n");
					}
			}
			else
			{			// editando el controlador
				if ((_newp|_newv)==0)
				{
					if (strcmp(_vectString,"p")==0)      //posicion
						{
							_newp=1; // Flag de edicion del controlador de pos
						}			
					if (strcmp(_vectString,"v")==0)      //velocidad
						{
							_newv=1; // Flag de edicion del controlador de vel
						}
					if ((_newp|_newv)==0)
						{
							printf("\nDebe presionar [p] o [v].\n");
						}
				}
				else
				{
					_newListo=1; // Flag para recepción de coeficientes
					printf("\n La sintaxis de ingreso del controlador es la siguiente:\n");
					printf("Cant_coef_M Cant_coef_N b0 b1 b2 b3 b4 bm a1 a2 a3 a4 a5 an \n");
				}
			}
	  }
	  else
	  {			// estamos listos para actualizar un controlador
		  AdquiereControlador(_vectString);
		  _newListo=0;	// desactivamos la edicion del controlador
		  _newp=0;
		  _newv=0;
		  printf("\nControlador actualizado.\n");
	  }
	  

      ////// limpiando la cadena
      _vectString[0]=0;
   }
}


// AdquiereValor: analiza un string recibido buscando la cadena que viene en el string
// ============== palabra. El contenido del string que le sigue a la palabra, debe ser
//				  un string numerico segun la convencion, el cual convierte en un int
// Recibe: string de datos, y string con la palabra de cabecera
// Retorna: dato numerico (int)
int AdquiereValor(char* cmd, char* palabra)
{
   char *dp;
   int valor=0;
      dp=strstr(cmd,palabra);
      if (dp!=NULL)
      {
            dp=dp+strlen(palabra);
            valor = atoi(dp);
      }
   return valor;
}

// AdquiereControlador: analiza un string de datos separados por espacios y los reacomoda 
// ==================== en variables de una estructura, previo conversión en enteros de
//						coma fija (fixed point).
// Recibe: string de datos segun una convencion predefinida
// Retorna: nada (edita una estructura global)
void AdquiereControlador(_vectString)
{
}

// lineal_FP: Convierte un dato de entrada, en otro de salida a traves de una funcion lineal.
// ==========
// Hace uso de las macros:  #define PROD_FP(a,b,q)  (((a)*(b))>>(q))
//							#define DIV_FP(a,b,q)	(((a)<<(q))/(b))
//
// Recibe: dato de entrada_X , Y_max, Y_min, X_max, X_min  <<int>>
// Retorna: dato convertido_Y (int)
int lineal_FP (int Xin, int Ymax, int Ymin, int Xmax, int Xmin)
{
   int Yout;
 
	// Ecuacion de la recta:
    // Yout=Ymin+(Xin-Xmin)*((Ymax-Ymin)/(Xmax-Xmin));

   Yout=Ymin+(PROD_FP((Xin-Xmin),DIV_FP((Ymax-Ymin),(Xmax-Xmin),Q),Q));

   return Yout;
}

// GdeZ_FP: Función transferencia en el dominio Z  <<< G[z] >>>.
// ========
// Recibe: un vector de coeficientes         b[m]      Long: M+1
//         un vector de coeficientes         a[n]      Long: N+1
//         un vector de entradas temporales  u         
//         un vertor de salidas anteriores   y         
//         grado del polinomio (valor de M)
//         grado del polinomio (valor de N)
// Retorna: salida del sistema
//------------------------------------------------------------------------------------
// En n (tiempo discreto):
//
//  y[k] = b[0]*u[k] + b[1]*u[k-1] + ... + b[m]*u[k-m]-a[1]*y[k-1] - ... - a[n]*y[k-n]
//
//  Siendo: u[m] las entradas temporales
//          y[n] las salidas temporales
//
//
// En Z (plano Z):
//
//          Y[z]    b0 + b1.z^-1 + b2.z^-2 + ... + bm.z^-m
//  G[z] = ----- = ----------------------------------------
//          U[z]    1 + a1.z^-1 + a2.z^-2 + ... + an.z^-n
//
//  Siendo: U[z] las entradas transformadas
//          Y[z] las salidas transformadas
//
int GdeZ_FP(struct GdeZ *estructura, int *u, int *y)
{
   int primero,segundo,out;
   
   primero=ProdEsc_FP(&estructura->b[0],u,(int)estructura->m);
   segundo=ProdEsc_FP(&estructura->a[0],y,(int)estructura->n);  // los coeficientes de segundo, se restan
   out=primero-segundo;
   return out;
}

// ProdEsc_FP: realiza el producto escalar entre dos vectores.
// ===========
// Hace uso de la macro:  #define PROD_FP(a,b,q)  (((a)*(b))>>(q))							
//
// Recibe: dos vectores y la longitud de los mismos
// Retorna: un resultado escalar
int ProdEsc_FP(int *vec1, int *vec2, int l)
{
   int acc=0;
   int acc2;
   int i;
   for(i=0;i<l;i++)
   {
      acc2=PROD_FP(vec1[i],vec2[i],Q);
      acc=acc+acc2;
   }
   return acc;
}
// cola_FP: cola FIFO.
// ========
// Recibe: puntero al vector de Stack previamente definido, largo del Stack, dato a ingresar
// Retorna: nada
void cola_FP(int *tabla, int l, int dataNew)
{
   int i;
   // SHIFT los datos de la cola FIFO (poco eficiente con muchos datos)
   for (i=l-1;i>0;i--)
   {
      tabla[i]=tabla[i-1];
   }
   // Insertamos el dataNew
   tabla[0]=dataNew;
}

// loop_irq_FP: Lazo de control de Velocidad/Posicion (Fixed Point) en una IRQ
// ============
// Requiere de Lineal_FP, GdeZ_FP, ini_cola, cola
// Recibe:  nada
// Retorna: nada
void loop_irq_FP(void)
{
   int valorLeido_c, u_PWM_c, consig_c;
   int signo=1;

   int consigna_rpm_i;            // ref. de velocidad (IN)
   int consigna_gra_i;            // ref. de posicion angular (IN)
   
   int duty_porcentual;	// antes usaba tambien yma_k_v, ym_k_v
   
   int ek_actual_rpm=0;       // Signal Error (INPUT CONTROLER)
   int ek_actual_gra=0;       // Signal Error (INPUT CONTROLER)

      /////// 1 - Leemos valores de Entrada y Consigna
      // Entrada:
      if(_ControlType==0)   //Velocidad = 0   /   Posicion = 1
      {
         valorLeido_c=LeerEntradaAnalogica(0);         // Entrada 0
         valorLeido_c=FP(valorLeido_c,Q);	// Lo pasamos a Fixed Point

         //Consigna de Vel:
         if(_Pote_active)  // 1 = Pote (mouse)  /  0 = Numero (teclado)
         {
            consig_c=(int)LeerEntradaAnalogica(2);            // Entrada 2
			consig_c=FP(consig_c,Q);	// Lo pasamos a Fixed Point
            consigna_rpm_i=lineal_FP((int)consig_c, TACOM_Y_MAX_rpm, TACOM_Y_MIN_rpm, AD_X_MAX_c, AD_X_MIN_c);   // ref.con el pote
         }
         else
         {
            consigna_rpm_i=(int)_ref;   //ingresada por teclado
			consigna_rpm_i=FP(consigna_rpm_i,Q);	// Lo pasamos a Fixed Point
         }

      }
      else
      {
         valorLeido_c=LeerEntradaAnalogica(1);         // Entrada 1
         valorLeido_c=FP(valorLeido_c,Q);	// Lo pasamos a Fixed Point

         //Consigna de Pos:
         if(_Pote_active)  // 1 = Pote (mouse)  /  0 = Numero (teclado)
         {
            consig_c=(int)LeerEntradaAnalogica(2);            // Entrada 2
			consig_c=FP(consig_c,Q);	// Lo pasamos a Fixed Point
            consigna_gra_i=lineal_FP((int)consig_c, TITA_Y_MAX_gra, TITA_Y_MIN_gra, AD_X_MAX_c, AD_X_MIN_c);   // ref.con el pote
         }
         else
         {
            consigna_gra_i=(int)_ref;   //ingresada por teclado
			consigna_gra_i=FP(consigna_gra_i,Q);	// Lo pasamos a Fixed Point
         }
      }


////Trabajamos con un controlador o con el otro dependiendo del tipo de control deseado.

   if(_ControlType==0)   //Velocidad = 0   /   Posicion = 1
   {
 	// Finalmente convertimos de (0_1023)c a (-100_+100)rpm en Fixed Point
		_yk_rpm_o=lineal_FP((int)valorLeido_c, TACOM_Y_MAX_rpm, TACOM_Y_MIN_rpm, AD_X_MAX_c, AD_X_MIN_c);

		//Actualizamos Display
		_out_vel_rpm=FPtoINT(_yk_rpm_o,Q);

      /////// 3 - Sumador --> Signal Error (ek_rpm)
      ek_actual_rpm=consigna_rpm_i-_yk_rpm_o;


      /////// 4 - Desplaza tablas temporales 
      cola_FP(_ek_rpm, _GdeZvel.m,ek_actual_rpm);
      cola_FP(_uk_vel_v, _GdeZvel.N, NO_VALIDO);   // NO_VALIDO será sobreescrito por GdeZ (uk_actual_v)   


      /////// 5 - Regulador (GdeZ_FP)
      // Prototipo: int GdeZ_FP(int *estructura, int *u, int *y)
      //										 ek_rpm    uk_v
      _uk_vel_v[0]=GdeZ_FP(&_GdeZvel, _ek_rpm, _uk_vel_v);
      

      /////// 6 - Convertimos de Vmotor deseada [-10V a +10V] a Duty Cycle[%]
      duty_porcentual=lineal_FP(_uk_vel_v[0], DUTY_MAX, DUTY_MIN, TACOM_X_MAX_v, TACOM_X_MIN_v);
      

      if(duty_porcentual>(QQ*100))      // Acotamos la salida del PWM
      {
         //printf("duty_porcentual = %f\n",duty_porcentual);
         duty_porcentual=100*QQ;
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
    	   // Finalmente convertimos de (0_1023)c a (-100_+100)rpm en Fixed Point
		_yk_gra_o=lineal_FP((int)valorLeido_c, TITA_Y_MAX_gra, TITA_Y_MIN_gra, AD_X_MAX_c, AD_X_MIN_c);


	  	//Actualizamos Display
		_out_pos_gra=FPtoINT(_yk_gra_o,Q);

      /////// 3 - Sumador --> Signal Error (ek_gra)
      
     // Adecuacion por limite en +/-180º
     _yk_gra_medido_o=_yk_gra_o;
     if((_yk_gra_o-_yk_gra_o_ant)<=(-180*QQ))
     {
        _yk_gra_o=_yk_gra_o+(360*QQ);
       
     }
     if((_yk_gra_o-_yk_gra_o_ant)>=(180*QQ))
     {
        _yk_gra_o=_yk_gra_o-(360*QQ);
        
     }
   
     ek_actual_gra=consigna_gra_i-_yk_gra_o;  //Senal de error adecuada
      _yk_gra_o_ant=_yk_gra_o;

      /////// 4 - Desplaza tablas temporales 
      cola_FP(_ek_gra, _GdeZpos.m,ek_actual_gra);
      cola_FP(_uk_pos_v, _GdeZpos.n, NO_VALIDO);   // NO_VALIDO será sobreescrito por GdeZ (uk_actual_v)   


      /////// 5 - Regulador (GdeZ_FP)
      // Prototipo: int GdeZ(int *estructura, int *u, int *y)
      //									   ek_gra    uk_v
      _uk_pos_v[0]=GdeZ_FP(&_GdeZpos, _ek_gra, _uk_pos_v);
      

      /////// 6 - Convertimos de Vmotor deseada [-10V a +10V] a Duty Cycle[%]
      duty_porcentual=lineal_FP(_uk_pos_v[0], DUTY_MAX, DUTY_MIN, TITA_X_MAX_v, TITA_X_MIN_v);
      
      if(duty_porcentual>(QQ*100))      // Acotamos la salida del PWM
      {
         //printf("duty_porcentual = %f\n",duty_porcentual);
         duty_porcentual=100*QQ;
      }
      if(duty_porcentual<0)
      {
         //printf("duty_porcentual = %f\n",duty_porcentual);
         duty_porcentual=0;
      }
   }


      /////// 7 - PWM
      u_PWM_c=(int)lineal_FP(duty_porcentual, PWM_Y_MAX_c, PWM_Y_MIN_c, DUTY_MAX, DUTY_MIN);

	  u_PWM_c= FPtoINT(u_PWM_c,Q); //Pasamos de Fixed Point a decimal (int)

	  EscribirSalidaPwm((int)u_PWM_c, (PWM_Y_MAX_c/QQ));
}

