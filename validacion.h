#ifndef _VALIDACION_H
#define _VALIDACION_H 1
#endif

//#define ALLOW_PRINTS //#-------------importante
// #define DEBUG

#include <cstdlib>
#include <stdlib.h>
#include <string.h>



// #ifndef _LECTURA_RS_H
// #include "bibliotecas/lecturaRS.h"
// #endif

// #ifndef _NORMALIZACION_H
// #include "bibliotecas/normalizacion.h"
// #endif

#ifndef _ITEMKNN_H
#include "implementacion1/itemKNN.h"
#endif

#ifndef _ITEMKNN_GPU_H
#include "implementacion1/itemKNN_GPU.h"
#endif

#ifndef _ITEMKNN_SP_H
#include "implementacion2/itemKNN_SP.h"
#endif

#ifndef _ITEMKNN_SP_GPU_H
#include "implementacion2/itemKNN_SP_GPU.h"
#endif

#ifndef _POP_H
#include "pop/POP.h"
#endif

#ifndef _TIME_H
#include <time.h>
#endif

#include <gsl/gsl_statistics.h>



//variales para identificar el método
#define ITEMKNN_ORIG    11
#define ITEMKNN_OPT     12
#define ITEMKNN_GPU     13

#define ITEMKNN_SP      21
#define ITEMKNN_SP_GPU  22

#define DYNAMIC_INDEX   3

#define POP             4

void limpiezaTrain(EvalR *train, int tamTrain, int **listaUsuarios,
                   int **listaItems, int *tUs, int *tIt);

float *generaMatrizDatos(EvalR *train, int trainTam, int numUs, int numIt);

void evaluacionTopN(MatrixINT restaultados, EvalR *test, int testTam,
                    int *listaUsuarios, int *listaItems, int max,
                    float *PRECISION, float *RECALL, float *NDCG);

void crossValidationT(int metodo, Eval *vecLectura, int kGrupos, int nVecinos, 
                      int nRecomendaciones);

int evaluacion_tiempos_ikOriginal(Eval *vec, int k, int n);

int evaluacion_tiempos_ik(Eval *vec, int k, int n);

int evaluacion_tiempos_ik_sp(Eval *vec, int k, int n);

void findB(Eval *vec);

void evaluacion_tiempos_exactitud(int metodo, int dataset, int kGrupos, int nVecinos, int nRecomendaciones, int similitud);