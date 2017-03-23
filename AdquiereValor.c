// AdquiereValor: 
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include "SimulaMotor.h"

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