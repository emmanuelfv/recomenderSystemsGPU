//Es necesario importar tambien lecturaRS.h

#ifndef _ITEMKNN_H
#define _ITEMKNN_H 1
#endif

#ifndef _LECTURA_RS_H
#include "../bibliotecas/lecturaRS.h"
#endif

#ifndef _NORMALIZACION_H
#include "../bibliotecas/normalizacion.h"
#endif

float **generaMatrizR(Eval *dat);

//Complejidad de explotación: O(nItems x nItems)
float **construirModelo_ikOriginal(float **R, int k, int nU, int nI);
int *aplicarModelo_ikOriginal(float **S, int nI, float *u, int n);


//Complejidad de explotación: O(nItems x nVecinos)
void construirModelo_ik(int **S_ind, float **S_val, float **R, int k, int nU, int nI);
int *aplicarModelo_ik(int **S_ind, float **S_val, int nI, int k, float *u, int n);
