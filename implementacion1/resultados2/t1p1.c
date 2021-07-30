/*
Version 3

principales diferencias con respecto a la version 2:

La medida se similitud es la similitud cosenoidal ajustada adjCos

La obtención de los datos se hace mediante consultas a un servidor en sql
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <postgresql/libpq-fe.h>

#include "lecturaRS.h"
#include "normalizacion.h"

typedef struct
{
    int ind;
    float val;
} Kvec;

//funciones para base de datos
void DB_exit(PGconn *conn);
Eval *DB_to_eval(char *tabla, int registros);
Eval *leerDB(int tipo);

//funciones de normalización

//funciones itemKNN
int cmpfun(const void *a, const void *b);
float **generaR(Eval *dat);
float cos_sim(float *A, float *B, int t);
float *mediaEvalR(EvalR *datos, int r, int tam);
float adj_cos(float *A, float *B, int t, float *uMed);
void actualizaKValores(int *vec_ind, float *vec_val, int k, float elem, int ind);
void buildModel(int **S_ind, float **S_val, float **R, int k, int nU, int nI);
int *applyModel(int **S_ind, float **S_val, int nI, int k, float *u, int n);

void main(int argc, char **argv)
{
    //Eval *vec = leerArchivo(atoi(argv[1]));
    Eval *vec = leerDB(atoi(argv[1]));


    int k = 50, n = 5, i, j;
    int **S_ind;
    float **R, **S_val;
    int **rec = NULL;
    contadores(vec);
    int nI = vec->numI;
    int nU = vec->numU;

    R = generaR(vec);

    clock_t t_ini, t_fin;
    double secs;

    t_ini = clock();

    S_ind = (int **)malloc(sizeof(int *) * nI);
    S_val = (float **)malloc(sizeof(float *) * nI);
    for (int i = 0; i < nI; i++)
    {
        S_ind[i] = (int *)malloc(sizeof(int) * k);
        S_val[i] = (float *)malloc(sizeof(float) * k);
    }


    float *uMed = mediaEvalR(vec->REC, vec->tREC, nU);
    buildModel(S_ind, S_val, R, k, vec->numU, vec->numI, uMed);
    free(uMed);

    t_fin = clock();
    secs = (double)(t_fin - t_ini) / CLOCKS_PER_SEC;
    printf("%.16g,", secs);


    t_ini = clock();

    //se aplicó a todos los usuarios pero lo ideal es aplicarlo a una lista de estos.
    float **U = (float **)malloc(sizeof(float *) * nU);
    rec = (int **)malloc(sizeof(int *) * nU);
    
    /*
    for(int i=0; i<k; i++)
    {
        printf("%f\n", S_val[0][i]);
    }*/

    for (int i = 0; i < nU; i++)
    {
        U[i] = (float *)malloc(sizeof(float) * nI);
        for (int j = 0; j < nI; j++)
        {
            U[i][j] = R[j][i];
        }
        
        rec[i] = applyModel(S_ind, S_val, nI, k, U[i], n);
    }

    t_fin = clock();
    secs = (double)(t_fin - t_ini) / CLOCKS_PER_SEC;
    printf("%.16g\n", secs);

    printf("itemKNN_m*K, resultados para usuarios 2, 5, 10\n");
    for (i = 0; i < n; i++)
    {
        printf("%d %d %d\n", rec[2][i], rec[5][i], rec[10][i]);
    }

    freeM((void **)R, nI);
    freeM((void **)S_ind, nI);
    freeM((void **)S_val, nI);
    freeM((void **)U, nU);
    freeM((void **)rec, nU);

    freeEval(vec);
    return;
}







//----------------------------------------------------
//-------------itemKNN--------------------------------
int cmpfun(const void *a, const void *b)
{
    //Esta función compara dos elementos recomendables de acuerdo
    //al posible nivel de interés del usuario
    Kvec *A = (Kvec *)a;
    Kvec *B = (Kvec *)b;

    return (B->val - A->val) * 1000000;
}

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










//-----------------------------------------------------
//--------------psql----------------------------------
void DB_exit(PGconn *conn)
{
    PQfinish(conn);
    exit(1);
}

Eval *DB_to_eval(char *tabla, int registros)
{

    PGconn *conn = PQconnectdb("user=emmanuel dbname=recomendaciones");

    if (PQstatus(conn) == CONNECTION_BAD)
    {
        fprintf(stderr, "Falló coneción a la base de datos: %s\n",
                PQerrorMessage(conn));
        DB_exit(conn);
    }

    char querty[100];
    if (registros == 0)
        sprintf(querty, "SELECT user_id, item_id, rating FROM %s", tabla);
    else
        sprintf(querty, "SELECT user_id, item_id, rating FROM %s LIMIT %d", tabla, registros);
    PGresult *res = PQexec(conn, querty);

    if (PQresultStatus(res) != PGRES_TUPLES_OK)
    {

        printf("No se encontró la tabla. Saliendo.\n");
        PQclear(res);
        DB_exit(conn);
    }

    int i, j, nFields;

//    nFields = PQnfields(res);
    
    Eval *Nod = (Eval *)malloc(sizeof(Eval));
    Nod->REC = (EvalR *)malloc(sizeof(EvalR) * PQntuples(res));
    Nod->LU = NULL;
    Nod->LI = NULL;

//        printf("%d\n",nFields);

    for (i = 0; i < PQntuples(res); i++)
    {
        Nod->REC[i].us = atoi(PQgetvalue(res, i, 0));
        Nod->REC[i].it = atoi(PQgetvalue(res, i, 1));
        Nod->REC[i].rat = atof(PQgetvalue(res, i, 2));
        //Nod->REC[i].time = atoi(PQgetvalue(res, i, 3));
        //Nod->REC = realloc(Nod->REC, (i + 2) * sizeof(EvalR));
    }

    Nod->tREC = PQntuples(res);

    PQclear(res);
    PQfinish(conn);

    return Nod;
}

Eval *leerDB(int tipo)
{
    char table[20];
    if (tipo == 1)
        strcpy(table, "ml100k");
    else if (tipo == 2)
        strcpy(table, "ml1m");
    else if (tipo == 3)
        strcpy(table, "ml10m");
    else if (tipo == 4)
        strcpy(table, "ml25m");
    else if (tipo == 5)
        strcpy(table, "bookcrossing_index");
    else if (tipo == 6)
        strcpy(table, "anime_index");
    else if (tipo == 7)
        strcpy(table, "netflix");
    else if (tipo == 8)
        strcpy(table, "good_netflix_index");
    else if (tipo == 9)
        strcpy(table, "amazon");
    else if (tipo == 10)
        strcpy(table, "good_amazon_index");
    else {
        printf("conjunto invalido\n");
        exit(1);
    }
    DB_to_eval(table, 0);
}
