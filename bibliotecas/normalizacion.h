#ifndef _NORMALIZACION_H
#define _NORMALIZACION_H 1
#endif

#ifndef _LECTURA_RS_H
#include "lecturaRS.h"
#endif


#define MAX_INT 9999999
#define MAX_FLOAT 99999999.0

typedef struct vecLista{
  int item; //id del item/usuario
  int *list;//vector de la lista (items que evalua un usuario/ usuarios que han evaluado un item)
  float *rat; //calificación usuario a item
  int tam; //tamaño de la lista
}VL;

typedef struct itemsCount{
  int x, c; //contador de items/usuarios= x, cont= c
}VIC;

typedef struct listasTrain{
  VL *lUs, *lIt;
  int tUs, tIt;
  Elemento *medUs;
}LISTS;

void aleatoriza(EvalR *vec, int tamVec);

int cmpfuncInt (const void * a, const void * b);
int cmpfuncElem (const void * a, const void * b);
int cmpfuncRatIt (const void * a, const void * b);
int cmpfuncRatUs (const void * a, const void * b);

Elemento *mediaVec(EvalR *datos, int r, int *tam, int tipo);
void normMedia(EvalR *datos, int r, Elemento *us, int tamUs);
float **normB(EvalR *datos, int r, int tamUs, int tamIt, float lamda2, float lamda3);
VL *getLista(EvalR *datos, int r, int tamVec, int tipo);

int bbStr(char **vector, int m, int n, char *val);
int busquedabinaria(void *vector, int m, int n, void *val, int tipo);
int bbElem(Elemento *vector, int m, int n, int val);
int bbEvalUs(EvalR *vector, int m, int n, int val);
int bb3(VIC *vector, int m, int n, int val);
int bb2(int *vector, int m, int n, int val);
int bb1(VL *vector, int m, int n, int val);

void freeVL(VL *vl, int tam);