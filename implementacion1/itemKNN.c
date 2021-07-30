/*
Version 3

principales diferencias con respecto a la version 2:

La medida se similitud es la similitud cosenoidal ajustada adjCos

La obtención de los datos se hace mediante consultas a un servidor en sql
*/

#include "itemKNN.h"

//----------------------------------------------------
//-------------itemKNN--------------------------------

float **generaMatrizR(Eval *dat) {
  /*utiliza una lista de recomendaciones dentro de la estructura Eval
  para generar una matriz de recomendaciones R de dimenciones |I| * |U| */

  float **R = allocM_F(dat->numI, dat->numU);
  for (int i = 0; i < dat->numI; i++) {
    for (int j = 0; j < dat->numU; j++) {
      R[i][j] = 0;
    }
  }

  for (int i = 0; i < dat->tREC; i++) {
    R[dat->REC[i].it - 1][dat->REC[i].us - 1] = dat->REC[i].rat;
  }

  return R;
}

float cos_sim(float *A, float *B, int t) {
  //función de similitud cosenoidal entre vectores
  int i;
  float a = 0, b = 0, c = 0;
  for (i = 0; i < t; i++) {
    a += A[i] * B[i];
    b += A[i] * A[i];
    c += B[i] * B[i];
  }
  return a / (sqrt(b) * sqrt(c));
}

//----------------------------------------------------
//Metodo original-------------------------------------
void actualizaKValores_ikOriginal(float *vec, int k, float elem)
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

int cuentaMinimos_ikOriginal(const float *vec, int n)
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

int estaEnKValores_ikOriginal(const float *vec, int k, float elem, int *n_min)
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

float **construirModelo_ikOriginal(float **R, int k, int nU, int nI)
{
    // Utiliza como entrada una matriz de recomendaciones de items por usuarios R,
    // un número de vecinos k, el número de usuarios nU y el número de ítems nI.

    // Genera una matriz S de las similitudes entre ítems, limitado a k
    // ítems similares a cada ítem. 
    float **S =(float **) malloc(sizeof(float *) * nI);
    float vec_k[k];
    for (int i = 0; i < nI; i++)
    {
        S[i] = (float *) malloc(sizeof(float) * nI);
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
            vec_k[j] = 0;

        for (int j = 0; j < nI; j++)
            actualizaKValores_ikOriginal(vec_k, k, S[i][j]);

        int n_min;
        n_min = cuentaMinimos_ikOriginal(vec_k, k);

        for (int j = 0; j < nI; j++)
        {
            if (!estaEnKValores_ikOriginal(vec_k, k, S[i][j], &n_min))
                S[i][j] = 0;
        }
    }

    return S;
}

int *aplicarModelo_ikOriginal(float **S, int nI, float *u, int n)
{
    // Hace uso de la matriz S generada en buildModel(),
    // el número de ítems nI,
    // un usuario u definido como las evaluaciones que ha hecho sobre diversos ítems en un vector de nI posiciones,
    // el número de recomendaciones a realizar n.

    // El resultado se expresa como una impresión de los índices de los ítems recomendados.
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
            x[i] += S[i][j] * u[j];
    }
  
    for (int i = 0; i < nI; i++){
        if (u[i] != 0)
            x[i] = 0;
    }

    for (int i = 0; i < n; i++)
        vec_k[i] = 0;

    for (int i = 0; i < nI; i++)
        actualizaKValores_ikOriginal(vec_k, n, x[i]);

    int n_min = 2;
    n_min = cuentaMinimos_ikOriginal(vec_k, n);

    //ordenamiento
    Elemento *temp = (Elemento*) malloc(sizeof(Elemento)*n);
    for (int i = 0; i < nI; i++){
        if (estaEnKValores_ikOriginal(vec_k, n, x[i], &n_min))
        {
            temp[cont].ind = i;
            temp[cont++].val = x[i];
        }
    }
    qsort(temp, n, sizeof(Elemento), cmpfunElem);
    for(int i = 0; i< n; i++)
        rec[i] = temp[i].ind;

    free(temp);
    free(x);
    free(vec_k);
    return rec;
}


//-----------------------------------------------------
//Método optimizado
void actualizaKValores(int *vec_ind, float *vec_val, int k, float elem,
                       int ind) {
  int i, pi;
  float min = MAX_FLOAT;

  for (i = 0; i < k; i++) {
    if (vec_val[i] < min) {
      min = vec_val[i];
      pi = i;
    }
  }
  if (elem > min) {
    vec_ind[pi] = ind;
    vec_val[pi] = elem;
  }

  return;
}

void construirModelo_ik(int **S_ind, float **S_val, float **R, int k, int nU, int nI) {
  /*Utiliza como entrada
  una matriz de recomendaciones de items por usuarios R,
  un número de vecinos k,
  el número de usuarios nU y el número de ítems nI.

  Genera una matriz S de las similitudes entre ítems, limitado a k
  ítems similares a cada ítem. */
  for (int i = 0; i < nI; i++) {
    float S_temp[nI];

    // Este ciclo define la complejidad de la construcción del modelo como
    // O(|I|^2 * |U|)
    for (int j = 0; j < nI; j++) {
      if (i != j)
        S_temp[j] = cos_sim(R[i], R[j], nU);
      else
        S_temp[j] = 0;
    }

    for (int j = 0; j < k; j++) {
      S_val[i][j] = 0;
    }

    for (int j = 0; j < nI; j++)
      actualizaKValores(S_ind[i], S_val[i], k, S_temp[j], j);
  }
  return;
}

int *aplicarModelo_ik(int **S_ind, float **S_val, int nI, int k, float *u, int n) {
  // Hace uso de la matriz S generada en buildModel(),
  // el número de ítems nI,
  // un usuario u definido como las evaluaciones que ha hecho sobre diversos ítems
  // en un vector de nI posiciones, el número de recomendaciones a realizar n.

  // El resultado se expresa como un vector de los índices de los ítems
  // recomendados.
  float *x = (float *)malloc(sizeof(float) * nI);
  int *rec = (int *)malloc(sizeof(int) * n);
  float vec_k[n];

  // Este ciclo define la complejidad de la aplicación del modelo como
  // O(|I|*K)
  for (int i = 0; i < nI; i++) {
    x[i] = 0;
    for (int j = 0; j < k; j++) {
      x[i] += S_val[i][j] * u[(int)S_ind[i][j]];
    }
  }

  for (int i = 0; i < nI; i++) {
    if (u[i] != 0)
      x[i] = 0;
  }

  for (int i = 0; i < n; i++) 
    vec_k[i] = 0;

  for (int i = 0; i < nI; i++)
    actualizaKValores(rec, vec_k, n, x[i], i);

  Elemento *temp = (Elemento *)malloc(sizeof(Elemento) * n);

  for (int i = 0; i < n; i++) {
    temp[i].ind = rec[i];
    temp[i].val = vec_k[i];
  }

  qsort(temp, n, sizeof(Elemento), cmpfunElem);

  for (int i = 0; i < n; i++)
    rec[i] = temp[i].ind;

  return rec;
}
