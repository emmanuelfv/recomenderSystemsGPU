
#ifndef _ITEMKNN_GPU_H
#define _ITEMKNN_GPU_H 1
#endif

#ifndef _LECTURA_RS_H
#include "../bibliotecas/lecturaRS.h"
#endif

#ifndef _GLIBCXX_MATH_H
#include <math.h>
#endif

#ifndef _NORMALIZACION_H
#include "../bibliotecas/normalizacion.h"
#endif

#define BLOCK_SIZE_IK 32


float *generaDatosR(Eval *dat);

float *generaMatrizDatos(EvalR *train, int trainTam, int numUs, int numIt);

void construccionModelo(Matrix R, MatrixElemento knn);

void applyModelAllGPU(Matrix R, MatrixElemento knn,
                      MatrixElemento rec);
