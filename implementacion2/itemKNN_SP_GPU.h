#ifndef _ITEMKNN_SP_GPU_H
#define _ITEMKNN_SP_GPU_H 1
#endif

#ifndef _LECTURA_RS_H
#include "../bibliotecas/lecturaRS.h"
#endif


#ifndef _NORMALIZACION_H
#include "../bibliotecas/normalizacion.h"
#endif

#define BLOCK_SIZE_SP 32

void construccion_SP_GPU(Elemento *vecUs, Elemento *vecIt, int *indUs, int *indIt, int nU, int nI, 
                float *medUs, float *medIt, Elemento **mat, int **indSim, int similitud);

void aplicacion_SP_GPU(Elemento *mat, int *indSim, Elemento *vecUs, int *indUs, Elemento *vecIt,
  int *indIt, int nU, int nI, float *medIt, int nVecinos, int nRecomendaciones, Elemento **recomendaciones);

