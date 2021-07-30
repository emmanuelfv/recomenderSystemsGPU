#ifndef _POP_H
#define _POP_H 1
#endif

#ifndef _LECTURA_RS_H
#include "../bibliotecas/lecturaRS.h"
#endif

void getCrossSets_POP(int dataset, int kGrupos, int parte, int nRecomendaciones, int **pops, int *nU, Elemento **test, int **indTest);

void simple_pop(int *pops, int nU, int nRecomendaciones, Elemento **recomendaciones);