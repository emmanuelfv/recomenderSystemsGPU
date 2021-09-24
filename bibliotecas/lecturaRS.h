#ifndef _LECTURA_RS_H
#define _LECTURA_RS_H 1
#endif

// #define ALLOW_PRINTS

#ifndef _STDIO_H
#include <stdio.h>
#endif

#ifndef _STDLIB_H
#include <stdlib.h>
#endif

#ifndef _STRING_H
#include <string.h>
#endif

#ifndef _TIME_H
#include <time.h>
#endif

#ifndef _GLIBCXX_MATH_H
#include <math.h>
#endif

#ifndef LIBPQ_FE_H
#include <postgresql/libpq-fe.h>
#endif

#define DATOS_CONECCION "user=emmanuel dbname=recomendaciones"

#define USER 0
#define ITEM 1
#define INT__ 10
#define CHAR__ 11
#define STRING__ 12
#define FLOAT__ 13
#define DOUBLE__ 14

//medidas de similitud utilizadas en itemknn_sp
#define PEARSON             1
#define COSENO              2
#define COSENO_AJUSTADO     3
#define EUCLIDEANO          4

typedef struct
{
  int us, it, time;
  float rat;
} EvalR;

typedef struct
{
  char **LU, **LI;
  EvalR *REC;
  int tREC, tLU, tLI, tCadLU, tCadLI, numU, numI;
} Eval;

typedef struct {
  int ind;
  float val;
} Elemento;

//Estructuras Matriz
typedef struct {
  int w;
  int h;
  float *d;
} Matrix;

typedef struct {
  int w;
  int h;
  int *d;
} MatrixINT;

typedef struct {
  int w;
  int h;
  Elemento *d;
} MatrixElemento;

typedef struct {
    float media, *bu, *bi;
}B_UI;


//Constructores Matriz
// Matrix *constructorMatrix_F(int a, int b, int alloc);

//Alocación/Liberación de memoria
int **allocM_I(int a, int b);
float **allocM_F(int a, int b);

void freeM_I(int **M, int t);
void freeM_F(float **M, int t);
void freeM_Str(char **M, int t);
void freeM_EvalR(EvalR **M, int t);
void freeM_Elem(Elemento **M, int t);
void freeEval(Eval *vec);


EvalR *copyEvalR(EvalR *V, int t);

//Organización de información
void printEvalR(Eval *vec, int tamDatos);

void contadores(Eval *datos);

int cmpfunElem(const void *a, const void *b);

//PSQL
#define ALL_PARTS 1
#define PARTIAL 2 

void DB_exit(PGconn *conn);
void checkPGconn(PGconn *conn);
void checkPGresult(PGconn *conn, PGresult *res);


char *getNombreTabla(int tipo);

Eval *leerDB(int tipo);

Eval *leerCrossSet(int tipo, int k, int parte);

void getCrossSets(int dataset, int k, int parte, Eval **setData,  
  Elemento **test, int **indTest);

void getCrossSets_SP(int dataset, int k, int parte, Elemento **vUs, int **indUs, int *tUs, 
  Elemento **vIt, int **indIt, int *tIt, Elemento **test, int **indTest);

void getMediaPsql(int *ind, int t, float **med, int tipo);

//DEBUG
#ifdef DEBUG
void validaMemoria(void *x, char *s);
#endif

//Evaluación
void evaluacion(Elemento *recomendaciones, int nU, int nRecomendaciones, Elemento *test, 
            int* indTest, float *PRECISION, float *RECALL, float *NDCG);

