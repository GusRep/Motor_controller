// LecturaPantalla: adquiere caracteres por IRQ y edita las referencias de posicicion
// velocidad, o uso del potenciometro de referencia.
// Recibe: nada
// Retorna: nada

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include "SimulaMotor.h"

// Variables globales
//extern char _RX_SerialComm;
extern char _vectString[10];
extern int _Pote_active;      // 1 = Pote (mouse)  /  0 = Numero (teclado)
extern float _ref;            // Referencia
extern int _ControlType;


void LecturaPantalla(void)
{  int i=0;
   char *potenciometro, *controlVel, *controlPos;
   
   if (_RX_SerialComm!=13)   // No se presiono INTRO
   {   printf("Ingresado: %c\n",_RX_SerialComm);    //char
       i=strlen(_vectString);
       _vectString[i]=(_RX_SerialComm);
       _vectString[i+1]=0;
   }
   else                                    // Se presiono INTRO
   {  
      printf("vectString: %s ...\n",_vectString); //string

      if ((strcmp(_vectString,"pos"))==0)      // 0 si son iguales
      {
            _ControlType=1;         //Velocidad = 0   /   Posicion = 1
      }   
      if (strcmp(_vectString,"vel")==0)
      {
            _ControlType=0;         //Velocidad = 0   /   Posicion = 1
      }
      if (strcmp(_vectString,"pot")==0)      //lectura AQUI
      {
            _Pote_active=1;            // 1 = Pote (mouse)  /  0 = Numero (teclado)
            printf("Usando el potenciometro. \n");
      }
      
      //introduciendo valor de referencia
      _ref=AdquiereValor(_vectString,"REF");
      potenciometro=strstr(_vectString,"REF");
      if (potenciometro!=NULL)
     {
        _Pote_active=0;      // 1 = Pote (mouse)  /  0 = Numero (teclado)
     }

      
      ////// limpiando la cadena
      _vectString[0]=0;
   }
}
