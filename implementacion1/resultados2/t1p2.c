/*
Version 2

Las diferencias con la versión 1 se encuentran en la aplicacíon del modelo, se reduce la complejidad
de O(M * M) por O(M*K).
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <postgresql/libpq-fe.h>
#include "lecturaRS.h"
#include "normalizacion.h"

typedef struct {
    int ind;
    float val;
}Kvec;

float **generaR(Eval *dat)
{
    /*utiliza una lista de recomendaciones dentro de la estructura Eval
    para generar una matriz de recomendaciones R de dimenciones |I| * |U| */

    float **R = (float **)malloc(sizeof(float *) * dat->numI);
    for (int i = 0; i < dat->numI; i++)
    {
        R[i] = malloc(sizeof(float) * dat->numU);
        for (int j = 0; j < dat->numU; j++)
        {
            R[i][j] = 0;
        }
    }

    for (int i = 0; i < dat->tREC; i++)
    {
        R[dat->REC[i].it - 1][dat->REC[i].us - 1] = dat->REC[i].rat;
    }

    return R;
}

float cos_sim(float *A, float *B, int t)
{
    int i, a = 0, b = 0, c = 0;
    for (i = 0; i < t; i++)
    {
        a += A[i] * B[i];
        b += A[i] * A[i];
        c += B[i] * B[i];
    }
    return a / (sqrt(b) * sqrt(c));
}

void actualizaKValores(int *vec_ind, float *vec_val, int k, float elem, int ind)
{
    int i, pi;
    float min = MAX_FLOAT;

    for (i = 0; i < k; i++)
    {
        if (vec_val[i] < min)
        {
            min = vec_val[i];
            pi = i;
        }
    }
    if (elem > min)
    {
        vec_ind[pi] = ind;
        vec_val[pi] = elem;
    }
        
    return;
}

int cmpfun(const void *a, const void *b)
{
    Kvec *A = (Kvec *) a;
    Kvec *B = (Kvec *) b;

    return (B->val - A->val)*1000000;
}

void buildModel(int **S_ind, float **S_val, float **R, int k, int nU, int nI)
{
    /*Utiliza como entrada 
    una matriz de recomendaciones de items por usuarios R,
    un número de vecinos k,
    el número de usuarios nU y el número de ítems nI.

    Genera una matriz S de las similitudes entre ítems, limitado a k
    ítems similares a cada ítem. */

    float vec_k[k];
    for (int i = 0; i < nI; i++)
    {
        float S_temp[nI];
        //Este ciclo define la complejidad de la construcción del modelo como
        //O(|I|^2 * |U|)

        for (int j = 0; j < nI; j++)
        {
            if (i != j)
                S_temp[j] = cos_sim(R[i], R[j], nU);
            else
                S_temp[j] = 0;
        }


        for (int j = 0; j < k; j++)
        {
            S_val[i][j] = 0;
        }

        for (int j = 0; j < nI; j++)
            actualizaKValores(S_ind[i], S_val[i], k, S_temp[j], j);

    }
    return;
}

int *applyModel(int **S_ind, float **S_val, int nI, int k, float *u, int n)
{
    /*Hace uso de
    la matriz S generada en buildModel(),
    el número de ítems nI,
    un usuario u definido como las evaluaciones que ha hecho sobre diversos ítems en un vector de nI posiciones,
    el número de recomendaciones a realizar n.

    El resultado se expresa como un vector de los índices de los ítems recomendados.*/

    float *x = (float *)malloc(sizeof(float) * nI);
    float vec_k[n];
    int *rec = (int *) malloc(sizeof(int)*n);
    int cont=0;
    //Este ciclo define la complejidad de la aplicación del modelo como
    //O(|I|*K)
    for (int i = 0; i < nI; i++)
    {
        x[i] = 0;
        for (int j = 0; j < k; j++)
        {
            x[i] += S_val[i][j] * u[(int) S_ind[i][j]];
        }
    }

    for (int i = 0; i < nI; i++)
    {
        if (u[i] != 0)
            x[i] = 0;
    }

    for (int i = 0; i < n; i++)
    {
        vec_k[i] = 0;
    }

    for (int i = 0; i < nI; i++)
        actualizaKValores(rec, vec_k, n, x[i], i);

    Kvec *temp = (Kvec*) malloc(sizeof(Kvec)*n);

    for(int i = 0; i< n; i++)
    {
        temp[i].ind = rec[i];
        temp[i].val = vec_k[i];
    }

    qsort(temp, n, sizeof(Kvec), cmpfun);
    for(int i = 0; i< n; i++)
    {
        rec[i] = temp[i].ind;
    }

    return rec;
}


void main(int argc, char **argv)
{
    clock_t t_ini, t_fin;
    double secs;

    t_ini = clock();
    Eval *vec = leerArchivo(atoi(argv[1]));
    //Eval *vec = leerDB(atoi(argv[1]));

    t_fin = clock();
    secs = (double)(t_fin - t_ini) / CLOCKS_PER_SEC;
    printf("%.16g\n", secs);

    freeEval(vec);
    return;

    int k = 50, n = 5, i, j;
    int **S_ind;
    float **R, **S_val;
    int **rec = NULL;
    contadores(vec);
    int nI = vec->numI;
    int nU = vec->numU;

    R = generaR(vec);

    S_ind = (int**) malloc(sizeof(int*)*nI);
    S_val = (float**) malloc(sizeof(float*)*nI);
    for(int i=0; i<nI; i++){
        S_ind[i] = (int*) malloc(sizeof(int)*k);
        S_val[i] = (float*) malloc(sizeof(float)*k);
    }

    t_ini = clock();


    buildModel(S_ind, S_val, R, k, vec->numU, vec->numI);

    t_fin = clock();
    secs = (double)(t_fin - t_ini) / CLOCKS_PER_SEC;
    printf("%.16g seg en construccion del modelo\n", secs);


    t_ini = clock();

    //se aplicó a todos los usuarios pero lo ideal es aplicarlo a una lista de estos.
    float **U= (float **) malloc(sizeof(float*)*nU); 
    rec = (int**) malloc(sizeof(int*)*nU);

    for(int i=0; i<nU; i++)
    {
        U[i] = (float *) malloc(sizeof(float)*nI);
        for(int j=0; j<nI; j++){
            U[i][j] = R[j][i];
        }
        rec[i] = applyModel(S_ind, S_val, nI, k, U[i], n);
    }

    t_fin = clock();
    secs = (double)(t_fin - t_ini) / CLOCKS_PER_SEC;
    printf("%.16g seg en la explotación para |U| usuarios\n", secs);


    printf("itemKNN_m*K, resultados para usuarios 2, 5, 10\n");   
    for(i=0; i< n; i++)
    {
        printf("%d %d %d\n", rec[2][i], rec[5][i], rec[10][i]);
    }

    freeM((void **)R, nI);
    freeM((void **)S_ind, nI);
    freeM((void **)S_val, nI);
    freeM((void **)U, nU);
    freeM((void **) rec, nU);
}
