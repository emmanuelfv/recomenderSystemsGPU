#ifndef _VALIDACION_H
#define _VALIDACION_H 1
#endif

//#define ALLOW_PRINTS //#-------------importante
// #define DEBUG

#include <cstdlib>
#include <stdlib.h>
#include <string.h>



#ifndef _LECTURA_RS_H
#include "bibliotecas/lecturaRS.h"
#endif

#ifndef _NORMALIZACION_H
#include "bibliotecas/normalizacion.h"
#endif

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

//variales para identificar a cada algoritmo
#define ITEMKNN_ORIG    11
#define ITEMKNN_OPT     12
#define ITEMKNN_GPU     13

#define ITEMKNN_SP      21
#define ITEMKNN_SP_GPU  22
#define ITEMKNN_SP_GPU2 23

#define DYNAMIC_INDEX   3

#define POP             4

int tiempos_iknn(int metodo, Eval *vec, int k, int n);

void tiempos_exactitud(int metodo, int dataset, int kGrupos, int nVecinos, int nRecomendaciones, int similitud);

void tiempos_exactitud_parcial(int metodo, int dataset, int kGrupos, int nVecinos, int nRecomendaciones, int parte, int similitud);