


#ifndef _ITEMKNN_SP_H
#define _ITEMKNN_SP_H 1
#endif

#ifndef _LECTURA_RS_H
#include "../bibliotecas/lecturaRS.h"
#endif

#ifndef _NORMALIZACION_H
#include "../bibliotecas/normalizacion.h"
#endif

typedef struct {
    int i, j;
    float sim;
}MatrizDispersa;

typedef struct {
    float media, *bu, *bi;
}B_UI;

void construccion_SP(Elemento *vecUs, Elemento *vecIt, int *indUs, int *indIt, int nU, int nI, 
                float *medUs, float *medIt, Elemento **mat, int **indSim, int similitud);
                
void aplicacion_SP(Elemento *mat, int *indSim, Elemento *vecUs, int *indUs, Elemento *vecIt,
  int *indIt, int nU, int nI, float *medIt, int nVecinos, int nRecomendaciones, Elemento **recomendaciones);

//void get_b(Elemento *vecUs, Elemento *vecIt, int *indUs, int *indIt, int nU, int nI, B_UI *b);