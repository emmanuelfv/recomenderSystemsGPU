
/*
Verción derivada de la version 6
La intención es reducir el tamaño del código escrito en esta etapa,
actualemente existen 908 lineas de código, el uso de las librerías pertinentes 
deberían ser capaces de reducir el tamaño en esta implementación.
*/

#include <cstdlib>
#include <cuda_runtime.h>

#include "itemKNN_GPU.h"


//#define ALLOW_PRINTS


void printM(const Matrix M) {
  // muestra la matriz en forma cuadrada
  for (int i = 0; i < M.h; i++) {
    for (int j = 0; j < M.w; j++) {
      printf("%5.0f ", M.d[i * M.w + j]);
    }
    printf("\n");
  }
  printf("\n");
  return;
}

float *generaMatrizDatos(EvalR *train, int trainTam, int numUs, int numIt) {
  //genera el vector de la matriz R
  float *R;
  R = (float *) malloc(numUs * numIt * sizeof(float));
  if(R== NULL){
    printf("Error NULL en R\n");
    exit(1);
  }
  for (int i = 0; i < numUs * numIt; i++) {
    R[i] = 0;
  }
  
  for (int i = 0; i < trainTam; i++) {
    if ((train[i].it) * numUs + (train[i].us) >= numUs * numIt){
      printf("Error, almacenando %d %d %d %d.", i, train[i].us, train[i].it, (train[i].it) * numUs + (train[i].us));
      exit(1);
    }
    R[(train[i].it) * numUs + (train[i].us)] = train[i].rat;
  }

  return R;
}


float *generaDatosR(Eval *dat) {
  // genera el vector de la matriz (misma función que "generaMatrizDatos()")
  float *R;
  R = (float *)malloc(sizeof(float) * dat->numU * dat->numI);
  for (int i = 0; i < dat->numI * dat->numU; i++) {
    R[i] = 0;
  }
  
  for (int i = 0; i < dat->tREC; i++) {
    R[(dat->REC[i].it) * dat->numU + (dat->REC[i].us)] = dat->REC[i].rat;
  }
  
  return R;
}


__global__ void MatSimilitudKernel(Matrix R, Matrix S) {
  //Genera el computo de la similitud entre elementos (ítems)
  float a = 0, b = 0, c = 0;
  int row = blockIdx.y * blockDim.y + threadIdx.y;
  int col = blockIdx.x * blockDim.x + threadIdx.x;
  if (row < S.h && col < S.w) {
    for (int i = 0; i < R.w; i++) {
      a += R.d[row * R.w + i] * R.d[col * R.w + i];
      b += R.d[row * R.w + i] * R.d[row * R.w + i];
      c += R.d[col * R.w + i] * R.d[col * R.w + i];
    }
    float div = sqrt(b * c);
    div==0? S.d[row * S.w + col] = 0 : S.d[row * S.w + col] = a / div;
    if (col == row) 
      S.d[row * S.w + col] = 0;
  }
}

__global__ void MatVecinosKernel(Matrix S, MatrixElemento knn) {
  //Genera a partir de una matriz de similitudes una de vecinos
  int row, pj;
  row = blockIdx.x * blockDim.x + threadIdx.x;
  float min;
  
  if (row < S.h) {
    for (int i = 0; i < knn.w; i++) {
      knn.d[row * knn.w + i].val = 0;
    }
    for (int i = 0; i < S.w; i++) {
      min = MAX_FLOAT; // se define como infinito
      for (int j = 0; j < knn.w; j++) {
        if (knn.d[row * knn.w + j].val < min) {
          min = knn.d[row * knn.w + j].val;
          pj = j;
        }
      }
      if (S.d[row * S.w + i] > min) {
        knn.d[row * knn.w + pj].val = S.d[row * S.w + i];
        knn.d[row * knn.w + pj].ind = i;
      }
    }
  }
}

void construccionModelo(Matrix R, MatrixElemento knn) {
  //obtiene una matriz de vecinos cercanos para cada ítem
  Matrix d_R;
  d_R.w = R.w; //nU
  d_R.h = R.h; //nI
  size_t size = R.w * R.h * sizeof(float);
  cudaMalloc(&d_R.d, size);
  cudaMemcpy(d_R.d, R.d, size, cudaMemcpyHostToDevice);
  
  Matrix d_S;
  d_S.w = R.h; //nI
  d_S.h = R.h; //nI
  size = d_S.w * d_S.h * sizeof(float);
  cudaMalloc(&d_S.d, size);
  
  dim3 dimBlock(BLOCK_SIZE_IK, BLOCK_SIZE_IK);
  dim3 dimGrid((d_S.w + dimBlock.x - 1) / dimBlock.x, 
  (d_S.h + dimBlock.y - 1) / dimBlock.y);
  MatSimilitudKernel<<<dimGrid, dimBlock>>>(d_R, d_S);

  MatrixElemento d_knn;
  d_knn.w = knn.w; //nV
  d_knn.h = knn.h; //nI
  size = knn.w * knn.h * sizeof(Elemento);
  
  cudaMalloc(&d_knn.d, size);

  MatVecinosKernel<<<(d_knn.h + BLOCK_SIZE_IK - 1) / BLOCK_SIZE_IK, BLOCK_SIZE_IK>>>(d_S, d_knn);
    
  cudaMemcpy(knn.d, d_knn.d, size, cudaMemcpyDeviceToHost);
  
  cudaFree(d_R.d);
  cudaFree(d_S.d);
  cudaFree(d_knn.d);
  
  return;
}
    
__device__ void actualizaKValores(Elemento *vec, int k, float elem, int ind) {
  //Esta función cambia en cada llamada el peor elemento en VEC por ELEM e IND 
  //si es mejor ELEM que el peor.
      int i, pi;
  float min = MAX_FLOAT;
  
  for (i = 0; i < k; i++) {
    if (vec[i].val < min) {
      min = vec[i].val;
      pi = i;
    }
  }
  if (elem > min) {
    vec[pi].ind = ind;
    vec[pi].val = elem;
  }
  
  return;
}

__device__ void swap(Elemento *xp, Elemento *yp) {
  //intercambio de elementos
  Elemento temp = *xp;
  *xp = *yp;
  *yp = temp;
}

__device__ void bubbleSort(Elemento *vec, int row, int colTam) {
  //ordenamiento de elementos basico (solo para pocos elementos)
  int i, j;
  for (i = 0; i < colTam - 1; i++)
    for (j = 0; j < colTam - i - 1; j++)
      if (vec[row * colTam + j].val < vec[row * colTam + j + 1].val)
        swap(&vec[row * colTam + j], &vec[row * colTam + j + 1]);
}


__global__ void applyModelKernel(Matrix R, MatrixElemento knn,
  MatrixElemento rec, float *x) {
    
  int u = blockIdx.x * blockDim.x + threadIdx.x;
  if (u < rec.h) {
    for (int i = 0; i < knn.h; i++) {
      x[u * knn.h + i] = 0;
      for (int j = 0; j < knn.w; j++) {
        x[u * knn.h + i] +=
          knn.d[i * knn.w + j].val * R.d[u + knn.d[i * knn.w + j].ind * R.w]; 
      }

      if (R.d[u + i * R.w] != 0)
        x[u * knn.h + i] = 0;
    }

    for (int i = 0; i < rec.w; i++) {
      rec.d[u * rec.w + i].val = 0;
    }

    for (int i = 0; i < knn.h; i++)
      actualizaKValores(&rec.d[u * rec.w], rec.w, x[u * knn.h + i], i);

    bubbleSort(rec.d, u, rec.w);
  }
}


void applyModelAllGPU(Matrix R, MatrixElemento knn,
  MatrixElemento rec) {
  cudaError_t err = cudaSuccess;

  Matrix d_R;
  d_R.w = R.w; // n = |U|
  d_R.h = R.h; // m = |I|
  size_t size = R.w * R.h * sizeof(float);
  err = cudaMalloc(&d_R.d, size);
  err = cudaMemcpy(d_R.d, R.d, size, cudaMemcpyHostToDevice);

  MatrixElemento d_knn;
  d_knn.w = knn.w;
  d_knn.h = knn.h;
  size = knn.w * knn.h * sizeof(Elemento);
  err = cudaMalloc(&d_knn.d, size);
  err = cudaMemcpy(d_knn.d, knn.d, size, cudaMemcpyHostToDevice);

  MatrixElemento d_rec;
  d_rec.w = rec.w; //nRecomenciones
  d_rec.h = rec.h; //n
  size = rec.w * rec.h * sizeof(Elemento);
  err = cudaMalloc(&d_rec.d, size);

  float *d_x;
  cudaMalloc(&d_x, sizeof(float) * R.h * R.w);

  applyModelKernel<<<(rec.h + BLOCK_SIZE_IK - 1) / BLOCK_SIZE_IK, BLOCK_SIZE_IK>>>(d_R, d_knn, d_rec, d_x);

  err = cudaMemcpy(rec.d, d_rec.d, size, cudaMemcpyDeviceToHost);

  if (err != cudaSuccess) {
    fprintf(stderr, "falló el copiado de rec (error code %s)!\n",
            cudaGetErrorString(err));
    exit(EXIT_FAILURE);
  }

  cudaFree(d_x);
  cudaFree(d_rec.d);
  cudaFree(d_R.d);
  cudaFree(d_knn.d);

  return;
}


