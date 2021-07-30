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

void actualizaKValores(float *vec, int k, float elem)
{
    int i, pi;
    float min = MAX_FLOAT;

    for (i = 0; i < k; i++)
    {
        if (vec[i] < min)
        {
            min = vec[i];
            pi = i;
        }
    }
    if (elem > min)
        vec[pi] = elem;
    return;
}

int cuentaMinimos(const float *vec, int n)
{
    float min = MAX_FLOAT;
    int n_min = 0;
    for (int i = 0; i < n; i++)
    {
        if (vec[i] < min)
            min = vec[i];
    }
    for (int i = 0; i < n; i++)
    {
        if (vec[i] == min)
            n_min++;
    }
    return n_min;
}

int estaEnKValores(const float *vec, int k, float elem, int *n_min)
{
    float min = MAX_FLOAT;
    int i;

    for (i = 0; i < k; i++)
    {
        if (vec[i] < min)
        {
            min = vec[i];
        }
    }
    if (elem >= min){
        return 1;
    }
    else if(elem == min && *n_min > 0)
    {
        *n_min = *n_min - 1;
        return 1;
    }
    else return 0;
}

int cmpfun(const void *a, const void *b)
{
    Kvec *A = (Kvec *) a;
    Kvec *B = (Kvec *) b;

    //return ((B->val - A->val)*1000000000 + (B->ind - A->ind));
    return (B->val - A->val)*1000000;
}

float **buildModel(float **R, int k, int nU, int nI)
{
    /*Utiliza como entrada 
    una matriz de recomendaciones de items por usuarios R,
    un número de vecinos k,
    el número de usuarios nU y el número de ítems nI.

    Genera una matriz S de las similitudes entre ítems, limitado a k
    ítems similares a cada ítem. */

    float **S = malloc(sizeof(float *) * nI);
    float vec_k[k];
    for (int i = 0; i < nI; i++)
    {
        S[i] = malloc(sizeof(float) * nI);
        //Este ciclo define la complejidad de la construcción del modelo como
        //O(|I|^2 * |U|)
        for (int j = 0; j < nI; j++)
        {
            if (i != j)
                S[i][j] = cos_sim(R[i], R[j], nU);
            else
                S[i][j] = 0;
        }

        for (int j = 0; j < k; j++)
        {
            vec_k[j] = 0;
        }

        for (int j = 0; j < nI; j++)
            actualizaKValores(vec_k, k, S[i][j]);

        int n_min;
        n_min = cuentaMinimos(vec_k, k);

        for (int j = 0; j < nI; j++)
        {
            if (!estaEnKValores(vec_k, k, S[i][j], &n_min))
                S[i][j] = 0;
        }
    }
    return S;
}

int *applyModel(float **S, int nI, float *u, int n)
{
    /*Hace uso de
    la matriz S generada en buildModel(),
    el número de ítems nI,
    un usuario u definido como las evaluaciones que ha hecho sobre diversos ítems en un vector de nI posiciones,
    el número de recomendaciones a realizar n.

    El resultado se expresa como una impresión de los índices de los ítems recomendados.*/

    float *x = (float *)malloc(sizeof(float) * nI);
    float *vec_k = (float *)malloc(sizeof(float) * n);
    int *rec = (int *)malloc(sizeof(int) * n);
    int cont = 0;
    //Este ciclo define la complejidad de la aplicación del modelo como
    //O(|I|^2)
    for (int i = 0; i < nI; i++)
    {
        x[i] = 0;
        for (int j = 0; j < nI; j++)
        {
            x[i] += S[i][j] * u[j];
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
        actualizaKValores(vec_k, n, x[i]);

    int n_min = 2;
    n_min = cuentaMinimos(vec_k, n);

    Kvec *temp = (Kvec*) malloc(sizeof(Kvec)*n);


    for (int i = 0; i < nI; i++)
    {
        if (estaEnKValores(vec_k, n, x[i], &n_min))
        {
            temp[cont].ind = i;
            temp[cont++].val = x[i];

    
        }
    }
    
    qsort(temp, n, sizeof(Kvec), cmpfun);

    for(int i = 0; i< n; i++)
    {
        rec[i] = temp[i].ind;
    }

    
    free(x);
    free(vec_k);
    return rec;
}

void main(int argc, char **argv)
{
    //Eval *vec = leerArchivo(atoi(argv[1]));
    Eval *vec = leerDB(atoi(argv[1]));


    int k = 50, n = 5, i, j;
    float **R, **S, **U;
    int **rec;
    contadores(vec);

    R = generaR(vec);
//    printf("%d datos\n", vec->tREC);
 
    clock_t t_ini, t_fin;
    double secs;

    t_ini = clock();

    S = buildModel(R, k, vec->numU, vec->numI);

    t_fin = clock();

    secs = (double)(t_fin - t_ini) / CLOCKS_PER_SEC;
    printf("%.16g,", secs);


    t_ini = clock();

    //se aplicó a todos los usuarios pero lo ideal es aplicarlo a una lista de estos.
    U = (float **)malloc(sizeof(float *) * vec->numU);
    rec = (int **)malloc(sizeof(int *) * vec->numU);


    for (int i = 0; i < vec->numU; i++)
    {
        U[i] = (float *)malloc(sizeof(float) * vec->numI);
        for (int j = 0; j < vec->numI; j++)
        {
            U[i][j] = R[j][i];
        }
        rec[i] = applyModel(S, vec->numI, U[i], n);

    }

    t_fin = clock();
    secs = (double)(t_fin - t_ini) / CLOCKS_PER_SEC;
    printf("%.16g\n", secs);

    printf("itemKNN_m^2, resultados para usuarios 2, 5, 10\n");
    for(i=0; i< n; i++)
    {
        printf("%d %d %d\n", rec[2][i], rec[5][i], rec[10][i]);
    }

    freeM((void **)R, vec->numI);
    freeM((void **)S, vec->numI);
    freeM((void **)U, vec->numU);
    freeM((void **)rec, vec->numU);
    
    freeEval(vec);
    return;
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
