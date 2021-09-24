#include "itemKNN_SP_GPU.h"
// #include <curand_kernel.h>

#define checkCudaErr(ans) { gpuErrchk((ans), __FILE__, __LINE__); }
void gpuErrchk(cudaError_t err, const char *file, int line){
  if (err != cudaSuccess) {
    fprintf(stderr, "(error code %s %s %d)!\n", cudaGetErrorString(err), file, line);
    exit(EXIT_FAILURE);
  }
  return;
}

__device__ float similitudCCP_gpu(Elemento *vecIt, int *indIt, float *medIt, int itemI, int itemJ, int h){
  // funcion coseno ajustado
  //preserentemente, se debe tener vecIt centrado en la media del usuario
  //es decir: u = [3,3,5,5]; medU = 4 -> u' = [-1,-1,1,1]
  int i=0, j=0, cont=0;
  float sum1, sum2, sum3, ri, rj;
  sum1 = sum2 = sum3 = 0;

  // printf("%d %d\n", indIt[itemI+1]-indIt[itemI], indIt[itemJ+1]-indIt[itemJ]);
  while(i<indIt[itemI+1]-indIt[itemI] && j<indIt[itemJ+1]-indIt[itemJ]){
    // printf("%d %d..\n", vecIt->REC[indIt[itemI]+i].us, vecIt->REC[indIt[itemJ]+j].us);
    if(vecIt[indIt[itemI]+i].ind == vecIt[indIt[itemJ]+j].ind){
      ri = vecIt[indIt[itemI]+i].val - medIt[itemI];
      rj = vecIt[indIt[itemJ]+j].val - medIt[itemJ];
      sum1 += ri * rj;
      sum2 += ri * ri;
      sum3 += rj * rj;  
      cont++;
      i++;
      j++;

    }
    else if(vecIt[indIt[itemI]+i].ind > vecIt[indIt[itemJ]+j].ind) j++;
    else i++;
  }
  if (sum2*sum3 == 0) return 0;

  float res = sum1/sqrt(sum2*sum3);
  if(cont < h) return res*((float) cont/h);
  return res;
}

__device__ float similitudCos_gpu(Elemento *vecIt, int *indIt, int itemI, int itemJ, int h){
  // funcion coseno ajustado
  //preserentemente, se debe tener vecIt centrado en la media del usuario
  //es decir: u = [3,3,5,5]; medU = 4 -> u' = [-1,-1,1,1]
  int i=0, j=0, cont=0;
  float sum1, sum2, sum3, ri, rj;
  sum1 = sum2 = sum3 = 0;

  // printf("%d %d\n", indIt[itemI+1]-indIt[itemI], indIt[itemJ+1]-indIt[itemJ]);
  while(i<indIt[itemI+1]-indIt[itemI] && j<indIt[itemJ+1]-indIt[itemJ]){
    // printf("%d %d..\n", vecIt->REC[indIt[itemI]+i].us, vecIt->REC[indIt[itemJ]+j].us);
    ri = vecIt[indIt[itemI]+i].val;
    rj = vecIt[indIt[itemJ]+j].val;
    if(vecIt[indIt[itemI]+i].ind == vecIt[indIt[itemJ]+j].ind){
      sum1 += ri * rj;
      sum2 += ri * ri;
      sum3 += rj * rj;  
      cont++;
      i++;
      j++;

    }
    else if(vecIt[indIt[itemI]+i].ind > vecIt[indIt[itemJ]+j].ind) {
      sum3 += rj * rj;  
      j++;
    }
    else{
      sum2 += ri * ri;
      i++;
    } 
  }
  if (sum2*sum3 == 0) return 0;

  float res = sum1/sqrt(sum2*sum3);
  if(cont < h) return res*((float) cont/h);
  return res;
}

__device__ float similitudAdCos_gpu(Elemento *vecIt, int *indIt, float *medUs, int itemI, int itemJ, int h){
  // funcion coseno ajustado
  //preserentemente, se debe tener vecIt centrado en la media del usuario
  //es decir: u = [3,3,5,5]; medU = 4 -> u' = [-1,-1,1,1]
  int i=0, j=0, cont=0;
  float sum1, sum2, sum3, ri, rj;
  sum1 = sum2 = sum3 = 0;

  // printf("%d %d\n", indIt[itemI+1]-indIt[itemI], indIt[itemJ+1]-indIt[itemJ]);
  while(i<indIt[itemI+1]-indIt[itemI] && j<indIt[itemJ+1]-indIt[itemJ]){
    // printf("%d %d..\n", vecIt->REC[indIt[itemI]+i].us, vecIt->REC[indIt[itemJ]+j].us);
    if(vecIt[indIt[itemI]+i].ind == vecIt[indIt[itemJ]+j].ind){
      ri = vecIt[indIt[itemI]+i].val - medUs[vecIt[indIt[itemI]+i].ind];
      rj = vecIt[indIt[itemJ]+j].val - medUs[vecIt[indIt[itemJ]+j].ind];
      sum1 += ri * rj;
      sum2 += ri * ri;
      sum3 += rj * rj;  
      cont++;
      i++;
      j++;
    }
    else if(vecIt[indIt[itemI]+i].ind > vecIt[indIt[itemJ]+j].ind) j++;
    else i++;
  }
  if (sum2*sum3 == 0) return 0;

  float res = sum1/sqrt(sum2*sum3);
  if(cont < h) return res*((float) cont/h);
  return res;
}

__device__ float similitudEuc_gpu(Elemento *vecIt, int *indIt, int itemI, int itemJ, int h){
  // __device__ float similitudEuc_gpu(Elemento *vecIt, int *indIt, float *medUs, int itemI, int itemJ, int h){
    // funcion coseno ajustado
  //preserentemente, se debe tener vecIt centrado en la media del usuario
  //es decir: u = [3,3,5,5]; medU = 4 -> u' = [-1,-1,1,1]
  int i=0, j=0, cont=0;
  float sum1, ri, rj;
  sum1 = 0;

  // printf("%d %d\n", indIt[itemI+1]-indIt[itemI], indIt[itemJ+1]-indIt[itemJ]);
  while(i<indIt[itemI+1]-indIt[itemI] && j<indIt[itemJ+1]-indIt[itemJ]){
    // printf("%d %d..\n", vecIt->REC[indIt[itemI]+i].us, vecIt->REC[indIt[itemJ]+j].us);
    ri = vecIt[indIt[itemI]+i].val;
    rj = vecIt[indIt[itemJ]+j].val;
    if(vecIt[indIt[itemI]+i].ind == vecIt[indIt[itemJ]+j].ind){
      
      // ri = vecIt[indIt[itemI]+i].val - medUs[vecIt[indIt[itemI]+i].ind];
      // rj = vecIt[indIt[itemJ]+j].val - medUs[vecIt[indIt[itemJ]+j].ind];
      sum1 += (ri - rj)*(ri - rj);
      cont++;
      i++;
      j++; 
    }
    else if(vecIt[indIt[itemI]+i].ind > vecIt[indIt[itemJ]+j].ind){
      sum1 += rj*rj;
      j++;
    } 
    else{
      sum1 += ri*ri;
      i++;
    } 
  }
  float res = sqrt(sum1);
  if(cont < h) return res*((float) cont/h);
  return res;
}


__global__ void construct_sp_gpu_kernel_ccp(Elemento *d_vecUs, int *d_indUs, Elemento *d_vecIt, int *d_indIt, 
  int nU, int nI, float *med, int *d_itemsConSimilitud, Elemento *d_mat, int *d_indSim){


  int i, u, us, j, it, l=0; 
  
  i = blockIdx.x * blockDim.x + threadIdx.x;
  
  if (i < nI) {

    for(u=0; u<d_indIt[i+1]-d_indIt[i]; u++){
      us = d_vecIt[d_indIt[i]+u].ind;
      for(j=0; j<d_indUs[us+1]-d_indUs[us]; j++){
        it = d_vecUs[d_indUs[us]+j].ind;
        if(i != it){
          if(d_itemsConSimilitud[i*nI + it] == 0){
            d_itemsConSimilitud[i*nI + it] = 1;
          }
        }
      }
    }
    
    for(j=0; j<nI; j++){
      if(d_itemsConSimilitud[i*nI + j]){
        float val = similitudCCP_gpu(d_vecIt, d_indIt, med, i, j, 50);
        if(val > 0.2){
          d_mat[i*nI + l].ind = j;
          d_mat[i*nI + l++].val = val;
        }
      }
    }
    d_indSim[i+1] = l;
  }
}

__global__ void construct_sp_gpu_kernel_cos(Elemento *d_vecUs, int *d_indUs, Elemento *d_vecIt, int *d_indIt, 
  int nU, int nI, int *d_itemsConSimilitud, Elemento *d_mat, int *d_indSim){


  int i, u, us, j, it, l=0; 
  int cont=0;
  
  i = blockIdx.x * blockDim.x + threadIdx.x;
  
  if (i < nI) {

    for(u=0; u<d_indIt[i+1]-d_indIt[i]; u++){
      us = d_vecIt[d_indIt[i]+u].ind;
      for(j=0; j<d_indUs[us+1]-d_indUs[us]; j++){
        it = d_vecUs[d_indUs[us]+j].ind;
        if(i != it){
          if(d_itemsConSimilitud[i*nI + it] == 0){
            d_itemsConSimilitud[i*nI + it] = 1;
            cont++;
          }
        }
      }
    }

    d_indSim[i+1] = cont;
    for(j=0; j<nI; j++){
      if(d_itemsConSimilitud[i*nI + j]){
        float simTemp = similitudCos_gpu(d_vecIt, d_indIt, i, j, 1);
        if(simTemp > 0.0){
          d_mat[i*nI + l].ind = j;
          d_mat[i*nI + l++].val = simTemp;
        }
        else d_indSim[i+1] -= 1;
      }
    }
  }
}

__global__ void construct_sp_gpu_kernel_cos_2(Elemento *d_vecUs, int *d_indUs, Elemento *d_vecIt, int *d_indIt, 
  int nU, int nI, int nVecinos, int *d_itemsConSimilitud, Elemento *d_mat){


  int i, u, us, j, k, it, pmin; 
  int cont=0;
  float min;
  
  i = blockIdx.x * blockDim.x + threadIdx.x;
  
  if (i < nI) {

    for(u=0; u<d_indIt[i+1]-d_indIt[i]; u++){
      us = d_vecIt[d_indIt[i]+u].ind;
      for(j=0; j<d_indUs[us+1]-d_indUs[us]; j++){
        it = d_vecUs[d_indUs[us]+j].ind;
        if(i != it){
          if(d_itemsConSimilitud[i*nI + it] == 0){
            d_itemsConSimilitud[i*nI + it] = 1;
            cont++;
          }
        }
      }
    }

    for(j=0; j<nI; j++){
      if(d_itemsConSimilitud[i*nI + j]){
        float simTemp = similitudCos_gpu(d_vecIt, d_indIt, i, j, 1);
        if(simTemp > 0.0){
          min = 99;
          for(k=0; k<nVecinos; k++)
          {
            if(d_mat[i*nVecinos + k].val<min){
              min = d_mat[i*nVecinos + k].val;
              pmin = k;
            }
          }
          if(simTemp>min){
            d_mat[i*nVecinos + pmin].val = simTemp;
            d_mat[i*nVecinos + pmin].ind = j;
          }
        }
      }
    }
  }
}


__global__ void construct_sp_gpu_kernel_ad_cos(Elemento *d_vecUs, int *d_indUs, Elemento *d_vecIt, int *d_indIt, 
  int nU, int nI, float *med, int *d_itemsConSimilitud, Elemento *d_mat, int *d_indSim){


  int i, u, us, j, it, l=0; 
  int cont=0;
  
  i = blockIdx.x * blockDim.x + threadIdx.x;
  
  if (i < nI) {

    for(u=0; u<d_indIt[i+1]-d_indIt[i]; u++){
      us = d_vecIt[d_indIt[i]+u].ind;
      for(j=0; j<d_indUs[us+1]-d_indUs[us]; j++){
        it = d_vecUs[d_indUs[us]+j].ind;
        if(i != it){
          if(d_itemsConSimilitud[i*nI + it] == 0){
            d_itemsConSimilitud[i*nI + it] = 1;
            cont++;
          }
        }
      }
    }

    for(j=0; j<nI; j++){
      if(d_itemsConSimilitud[i*nI + j]){
        float val = similitudAdCos_gpu(d_vecIt, d_indIt, med, i, j, 50);
        if (val > 0.2){
          d_mat[i*nI + l].ind = j;
          d_mat[i*nI + l++].val = val;
        }
      }
    }
    d_indSim[i+1] = l;
  }
}

__global__ void construct_sp_gpu_kernel_euc(Elemento *d_vecUs, int *d_indUs, Elemento *d_vecIt, int *d_indIt, 
  int nU, int nI, int *d_itemsConSimilitud, Elemento *d_mat, int *d_indSim){


  int i, u, us, j, it, l=0; 
  int cont=0;
  
  i = blockIdx.x * blockDim.x + threadIdx.x;
  
  if (i < nI) {

    for(u=0; u<d_indIt[i+1]-d_indIt[i]; u++){
      us = d_vecIt[d_indIt[i]+u].ind;
      for(j=0; j<d_indUs[us+1]-d_indUs[us]; j++){
        it = d_vecUs[d_indUs[us]+j].ind;
        if(i != it){
          if(d_itemsConSimilitud[i*nI + it] == 0){
            d_itemsConSimilitud[i*nI + it] = 1;
            cont++;
          }
        }
      }
    }

    d_indSim[i+1] = cont;
    for(j=0; j<nI; j++){
      if(d_itemsConSimilitud[i*nI + j]){
        d_mat[i*nI + l].ind = j;
        d_mat[i*nI + l++].val = similitudEuc_gpu(d_vecIt, d_indIt, i, j, 1);
      }
    }
  }
}

//
void construccion_SP_GPU(Elemento *vecUs, Elemento *vecIt, int *indUs, int *indIt, int nU, int nI, 
                float *medUs, float *medIt, Elemento **mat, int **indSim, int similitud){
  //Esta función construye el modelo en forma de una matriz dispersa con formato
  //Compresed Sparse Row CSR ya que se tiene un vector de indices que marcan el inicio
  //de cada renglón para el almacenamiento de los datos.
  
  // clock_t t_1, t_2, t_3, t_4, t_5;
  // double secs;
  
  cudaError_t err = cudaSuccess;
  Elemento *d_vecUs, *d_vecIt, *d_mat;
  int *d_indUs, *d_indIt, *d_indSim, *d_itemsConSimilitud;
  float *d_medUs, *d_medIt;
  size_t size;
  
  // t_1 = clock(); 
  
  //copiado en GPU
  size = sizeof(Elemento)*indUs[nU];
  err = cudaMalloc(&d_vecUs, size);
  checkCudaErr(err);    
  err = cudaMemcpy(d_vecUs, vecUs, size, cudaMemcpyHostToDevice);
  checkCudaErr(err);
  
  size = sizeof(Elemento)*indIt[nI];
  err = cudaMalloc(&d_vecIt, size);
  checkCudaErr(err);    
  err = cudaMemcpy(d_vecIt, vecIt, size, cudaMemcpyHostToDevice);
  checkCudaErr(err);
  
  size = sizeof(int)*(nU+1);
  err = cudaMalloc(&d_indUs, size);
  checkCudaErr(err);    
  err = cudaMemcpy(d_indUs, indUs, size, cudaMemcpyHostToDevice);
  checkCudaErr(err);
  
  size = sizeof(int)*(nI+1);
  err = cudaMalloc(&d_indIt, size);
  checkCudaErr(err);    
  err = cudaMemcpy(d_indIt, indIt, size, cudaMemcpyHostToDevice);
  checkCudaErr(err);
  
  size = sizeof(float)*nU;
  err = cudaMalloc(&d_medUs, size);
  checkCudaErr(err);
  err = cudaMemcpy(d_medUs, medUs, size, cudaMemcpyHostToDevice);
  checkCudaErr(err);
  
  size = sizeof(float)*nI;
  err = cudaMalloc(&d_medIt, size);
  checkCudaErr(err);
  err = cudaMemcpy(d_medIt, medIt, size, cudaMemcpyHostToDevice);
  checkCudaErr(err);
  
  
  size = sizeof(Elemento)*nI*nI;
  err = cudaMalloc(&d_mat, size);
  checkCudaErr(err);
  
  size = sizeof(int)*nI*nI;
  err = cudaMalloc(&d_itemsConSimilitud, size);
  checkCudaErr(err);
  
  size = sizeof(int)*(nI+1);
  err = cudaMalloc(&d_indSim, size);
  checkCudaErr(err);
  
  // t_2 = clock();
  
  // float *med;
  if(similitud == PEARSON){
    construct_sp_gpu_kernel_ccp<<<(nI + BLOCK_SIZE_SP - 1) / BLOCK_SIZE_SP, BLOCK_SIZE_SP>>>(d_vecUs, d_indUs, 
    d_vecIt, d_indIt, nU, nI, d_medIt, d_itemsConSimilitud, d_mat, d_indSim);
  }else if(similitud == COSENO){
    construct_sp_gpu_kernel_cos<<<(nI + BLOCK_SIZE_SP - 1) / BLOCK_SIZE_SP, BLOCK_SIZE_SP>>>(d_vecUs, d_indUs, 
    d_vecIt, d_indIt, nU, nI, d_itemsConSimilitud, d_mat, d_indSim);
  }else if(similitud == COSENO_AJUSTADO){
    construct_sp_gpu_kernel_ad_cos<<<(nI + BLOCK_SIZE_SP - 1) / BLOCK_SIZE_SP, BLOCK_SIZE_SP>>>(d_vecUs, d_indUs, 
    d_vecIt, d_indIt, nU, nI, d_medUs, d_itemsConSimilitud, d_mat, d_indSim);
  }else if(similitud == EUCLIDEANO){
    construct_sp_gpu_kernel_euc<<<(nI + BLOCK_SIZE_SP - 1) / BLOCK_SIZE_SP, BLOCK_SIZE_SP>>>(d_vecUs, d_indUs, 
    d_vecIt, d_indIt, nU, nI, d_itemsConSimilitud, d_mat, d_indSim);
  }else{
    printf("Error en la selección de función objetivo.\n");
    exit(0);
  }
          
  // t_3 = clock();
  
  //  printf("%d...%d\n", nU, nI);
  mat[0] = (Elemento *) malloc(sizeof(Elemento)*nI*nI);
  indSim[0] = (int *) malloc(sizeof(int)*(1+nI));
  
  
  err = cudaMemcpy(mat[0], d_mat, sizeof(Elemento)*nI*nI, cudaMemcpyDeviceToHost);
  checkCudaErr(err);
  
  err = cudaMemcpy(indSim[0], d_indSim, sizeof(int)*(1+nI), cudaMemcpyDeviceToHost);
  checkCudaErr(err);
  // t_4 = clock();
  
  
  indSim[0][0] = 0;
  for(int i=0; i<nI; i++){
    // printf("%d_%d\n", i, indSim[0][i+1]);
    for(int j=0; j<indSim[0][i+1]; j++){
      // printf("%d_%d_%d ", indSim[0][i] + j, i*nI + j, i);
      // if(j%20==0) printf("\n");
      mat[0][indSim[0][i] + j] = mat[0][i*nI + j];
    }
    // // printf("\n");
    indSim[0][i+1] += indSim[0][i];
  }
  
  
  // // exit(1);
  // mat[0] = (Elemento *) realloc(mat[0], sizeof(Elemento)*indSim[0][nI]);
  
  cudaFree(d_vecUs);
  cudaFree(d_vecIt);
  cudaFree(d_indUs);
  cudaFree(d_indIt);
  cudaFree(d_medUs);
  cudaFree(d_medIt);
  cudaFree(d_itemsConSimilitud);
  cudaFree(d_mat);
  cudaFree(d_indSim);
  // t_5 = clock();
  
  // secs = (double)(t_2 - t_1) / CLOCKS_PER_SEC;
  // printf("%.8f,", secs);
  // secs = (double)(t_3 - t_2) / CLOCKS_PER_SEC;
  // printf("%.8f,", secs);
  // secs = (double)(t_4 - t_3) / CLOCKS_PER_SEC;
  // printf("%8f,", secs);
  // secs = (double)(t_5 - t_4) / CLOCKS_PER_SEC;
  // printf("%8f,", secs);

  return;
}





__device__ int bbElemGPU(Elemento *vector, int m, int n, int val){
  //de un vector Elemento, retorna el indice de un valor espesifico
  if(n==m) return -1;
  if(n-m == 1){
    if(vector[m].ind==val) return m;
    else return -1; //no es un usuario valido
  }

  int mitad = (m + n) / 2;
  if(val<vector[mitad].ind){ 
    return bbElemGPU(vector, m, mitad, val);
  }
  else{
    return bbElemGPU(vector, mitad, n, val);
  }
}

__device__ void bbElemGPU2(Elemento *vector, int m, int n, int val, int *res){
  //de un vector Elemento, retorna el indice de un valor espesifico

  int mitad;
  while(1){
    if(n==m){
      *res = -1;
      return;
    }
    if(n-m == 1){
      *res = (vector[m].ind==val)?m:-1;
      return;
    }
    mitad = (m + n) / 2;
    if(val<vector[mitad].ind){ 
      n = mitad;
    }
    else{
      m = mitad;
    }
    
  }

}


__global__ void apply_knn_sp_gpu_kernel(Elemento *d_mat, int *d_indSim, Elemento *d_vecUs, int *d_indUs, Elemento *d_vecIt,
  int *d_indIt, int nU, int nI, float media, float *bu, float *bi, int nVecinos, Elemento *d_topVecinos, float *d_allRec, int u){

  int i, it, iit, j, k, l, pmin; 
  float min, x1;

  // curandState local_state;
  
  i = blockIdx.x * blockDim.x + threadIdx.x;
  
  if (i < nI) {
    it = bbElemGPU(d_vecUs, d_indUs[u], d_indUs[u+1], i);
    if(it == -1){
      //i es recomendable, se calculará rui
      
      for(j=0; j<nVecinos; j++){
        d_topVecinos[i * nVecinos + j].val = 0;
      }
      
      // Busqueda de buenos vecinos
      k=0;
      for(j=0; j<d_indUs[u+1]-d_indUs[u]; j++){
        it = d_vecUs[d_indUs[u]+j].ind;
        iit = bbElemGPU(d_mat, d_indSim[i], d_indSim[i+1], it);
        if(iit != -1){
          if(d_mat[iit].val > 0){
            // vecinos[k++] = d_mat[iit];
            k++;
            if(k>nVecinos){
              min = MAX_FLOAT;
              for(l=0; l<nVecinos; l++){
                if(d_topVecinos[i * nVecinos + l].val < min){
                  min = d_topVecinos[i * nVecinos + l].val;
                  pmin = l;
                }
              }
              if (d_mat[iit].val > min){
                d_topVecinos[i * nVecinos + pmin] = d_mat[iit];
              }
            }
            else{
              d_topVecinos[i * nVecinos + k-1] = d_mat[iit];
            }   
          }
        }
      }
      
      // delimitación de vecinos
      if(k==0) return;
      if(k>nVecinos) k = nVecinos;
      
      // calculo r_ui
      x1 = 0;
      // int x2=0;
      for(j=0; j<k; j++){
        it = bbElemGPU(d_vecUs, d_indUs[u], d_indUs[u+1], d_topVecinos[i * nVecinos + j].ind);
        
        // x1 += d_topVecinos[i * nVecinos + j].val * (d_vecUs[it].val);
        x1 += d_topVecinos[i * nVecinos + j].val * (d_vecUs[it].val - media - bu[u] - bi[d_topVecinos[i * nVecinos + j].ind]);
        // x2 += d_topVecinos[i * nVecinos + j].val;
      }
      
      //d_topRec[i].ind = i;
      //d_topRec[i].val = x1 + media + bu[u] + bi[i];
      d_allRec[u * nI + i] = x1 + media + bu[u] + bi[i];
      return;
      
    }
  }
}


__device__ void actualizaKValoresA(Elemento *vec, int k, float elem, int ind) {
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

__device__ void swapA(Elemento *xp, Elemento *yp) {
  //intercambio de elementos
  Elemento temp = *xp;
  *xp = *yp;
  *yp = temp;
}

__device__ void bubbleSortA(Elemento *vec, int row, int colTam) {
  //ordenamiento de elementos basico (solo para pocos elementos)
  int i, j;
  for (i = 0; i < colTam - 1; i++)
    for (j = 0; j < colTam - i - 1; j++)
      if (vec[row * colTam + j].val < vec[row * colTam + j + 1].val)
        swapA(&vec[row * colTam + j], &vec[row * colTam + j + 1]);
}


__global__ void apply_top_k_sp_gpu_kernel(int nU, int nI, int nRecomenciones, 
  float *d_topRec, Elemento *d_recomendaciones){
  
  int u = blockIdx.x * blockDim.x + threadIdx.x;
  
  if (u < nU) {
    for (int i = 0; i < nRecomenciones; i++) {
      d_recomendaciones[u * nRecomenciones + i].val = 0;
    }

    for (int i = 0; i < nI; i++)
      actualizaKValoresA(&d_recomendaciones[u * nRecomenciones], nRecomenciones, d_topRec[u * nI + i], i);

    bubbleSortA(d_recomendaciones, u, nRecomenciones);
  }
}



void aplicacion_SP_GPU(Elemento *mat, int *indSim, Elemento *vecUs, int *indUs, Elemento *vecIt,
  int *indIt, int nU, int nI, B_UI b, int nVecinos, int nRecomendaciones, Elemento **recomendaciones){
  //Esta función aplica el método esparcido mediante GPU.

  // clock_t t_1, t_2, t_3, t_4, t_5;
  // double secs;  

  cudaError_t err = cudaSuccess;
  Elemento *d_mat, *d_vecUs, *d_vecIt, *d_recomendaciones;
  int *d_indSim, *d_indUs, *d_indIt;
  float *d_bi, *d_bu;
  size_t size;

  // t_1 = clock();

  //copiado en GPU
  size = sizeof(Elemento)*indSim[nI];
  err = cudaMalloc(&d_mat, size);
  checkCudaErr(err);
  err = cudaMemcpy(d_mat, mat, size, cudaMemcpyHostToDevice);
  checkCudaErr(err);
  
  size = sizeof(Elemento)*indUs[nU];
  err = cudaMalloc(&d_vecUs, size);
  checkCudaErr(err);    
  err = cudaMemcpy(d_vecUs, vecUs, size, cudaMemcpyHostToDevice);
  checkCudaErr(err);
  
  size = sizeof(Elemento)*indIt[nI];
  err = cudaMalloc(&d_vecIt, size);
  checkCudaErr(err);    
  err = cudaMemcpy(d_vecIt, vecIt, size, cudaMemcpyHostToDevice);
  checkCudaErr(err);
  
  size = sizeof(int)*(nI+1);
  err = cudaMalloc(&d_indSim, size);
  checkCudaErr(err);    
  err = cudaMemcpy(d_indSim, indSim, size, cudaMemcpyHostToDevice);
  checkCudaErr(err);
  
  size = sizeof(int)*(nU+1);
  err = cudaMalloc(&d_indUs, size);
  checkCudaErr(err);    
  err = cudaMemcpy(d_indUs, indUs, size, cudaMemcpyHostToDevice);
  checkCudaErr(err);
  
  size = sizeof(int)*(nI+1);
  err = cudaMalloc(&d_indIt, size);
  checkCudaErr(err);    
  err = cudaMemcpy(d_indIt, indIt, size, cudaMemcpyHostToDevice);
  checkCudaErr(err);
  
  size = sizeof(float)*nI;
  err = cudaMalloc(&d_bi, size);
  checkCudaErr(err);
  err = cudaMemcpy(d_bi, b.bi, size, cudaMemcpyHostToDevice);
  checkCudaErr(err);

  size = sizeof(float)*nU;
  err = cudaMalloc(&d_bu, size);
  checkCudaErr(err);
  err = cudaMemcpy(d_bu, b.bu, size, cudaMemcpyHostToDevice);
  checkCudaErr(err);
  
  Elemento *d_topVecinos;//, *d_topRec;
  float *d_allRec;

  size = sizeof(Elemento)*nI*nVecinos;
  err = cudaMalloc(&d_topVecinos, size);
  checkCudaErr(err);

  // size = sizeof(Elemento)*nI;
  // err = cudaMalloc(&d_topRec, size);
  // checkCudaErr(err);
  
  size = sizeof(float)*nI*nU;
  err = cudaMalloc(&d_allRec, size);
  checkCudaErr(err);
  
  size = sizeof(Elemento)*nRecomendaciones*nU;
  err = cudaMalloc(&d_recomendaciones, size);
  checkCudaErr(err);
  
  
  recomendaciones[0] = (Elemento *) malloc(sizeof(Elemento) * nU * nRecomendaciones);
  //Elemento *rec = (Elemento *) malloc(sizeof(Elemento) * nI);
  
  int u;

  // t_2 = clock();
  // double s1=0, s2=0;

  for(u=0; u<nU; u++){
    // t_3 = clock();
    cudaMemset(d_topVecinos, 0, nI*nVecinos*sizeof(Elemento));
    //cudaMemset(d_topRec, 0, nRecomendaciones*sizeof(Elemento));
    
    apply_knn_sp_gpu_kernel<<<(nI + BLOCK_SIZE_SP - 1) / BLOCK_SIZE_SP, BLOCK_SIZE_SP>>>(d_mat, d_indSim, 
      d_vecUs, d_indUs, d_vecIt, d_indIt, nU, nI, b.media, d_bu, d_bi, nVecinos, d_topVecinos, d_allRec, u);
      
      
    // t_4 = clock();
    
    // size = sizeof(Elemento)*nI;
    // err = cudaMemcpy(rec, d_topRec, size, cudaMemcpyDeviceToHost);
    // // err = cudaMemcpy(&recomendaciones[0][u*nRecomendaciones], d_recomendaciones, size, cudaMemcpyDeviceToHost);
    // checkCudaErr(err);
    
    // qsort(rec, nI, sizeof(Elemento), cmpfuncElem);
    // //copiado de recomendaciones en "recomendaciones"
    // for(i=0; i<nRecomendaciones; i++){
    //   recomendaciones[0][u*nRecomendaciones + i] = rec[i];
    // }
    
    // t_5 = clock();
    
    // s1 += (double)(t_4 - t_3) / CLOCKS_PER_SEC;
    
  }
      
  
  apply_top_k_sp_gpu_kernel<<<(nU + BLOCK_SIZE_SP - 1) / BLOCK_SIZE_SP, BLOCK_SIZE_SP>>>(nU, 
    nI, nRecomendaciones, d_allRec, d_recomendaciones);
    
  size = sizeof(Elemento)*nRecomendaciones*nU;
  err = cudaMemcpy(recomendaciones[0], d_recomendaciones, size, cudaMemcpyDeviceToHost);
  // err = cudaMemcpy(&recomendaciones[0][u*nRecomendaciones], d_recomendaciones, size, cudaMemcpyDeviceToHost);
  checkCudaErr(err);
  
  // t_5 = clock();
  
  // s2 = (double)(t_5 - t_4) / CLOCKS_PER_SEC;
  // free(rec);

  cudaFree(d_mat);
  cudaFree(d_vecUs);
  cudaFree(d_vecIt);
  cudaFree(d_indSim);
  cudaFree(d_indUs);
  cudaFree(d_indIt);
  cudaFree(d_bi);
  cudaFree(d_bu);
  cudaFree(d_topVecinos);
  cudaFree(d_allRec);
  cudaFree(d_recomendaciones);


  
  // secs = (double)(t_2 - t_1) / CLOCKS_PER_SEC;
  // printf("%8f,", secs);
  // printf("%8f,", s1);
  // printf("%8f\n", s2);

  return;
}

/*
ml100k
1693428 datos de 2825761 posibles
1715542 datos de 2829124 posibles
1712816 datos de 2829124 posibles
1726722 datos de 2829124 posibles
1710028 datos de 2829124 posibles

ml1m
10612368 datos de 13734436 posibles
10603862 datos de 13734436 posibles
10625368 datos de 13734436 posibles
10650178 datos de 13734436 posibles
10611680 datos de 13734436 posibles

amazon_0.1% 233 -> 0.25 
399,354 datos de 1,000,000 posibles
396832 datos de 1000000 posibles
397738 datos de 1000000 posibles
396401 datos de 1000000 posibles
398916 datos de 1000000 posibles

netflix_1%
997002 datos de 1000000 posibles
997002 datos de 1000000 posibles
997002 datos de 1000000 posibles
997002 datos de 1000000 posibles
997002 datos de 1000000 posibles
*/

//versión 2, más mejor
//construcción: retorna matriz elemento[nu * nVecinos];

__global__ void construct_sp_vecinos_gpu_kernel(int nI, int nVecinos, Elemento *d_mat, int *d_indSim, Elemento *d_matVecinos){

  int i, min, pk;
  i = blockIdx.x * blockDim.x + threadIdx.x;
  
  if (i < nI) {
    for (int j = 0; j < nVecinos; j++) {
      d_matVecinos[i * nVecinos + j].val = 0;
    }

    for (int j = 0; j < d_indSim[i+1] - d_indSim[i]; j++) {
      min = MAX_FLOAT; // se define como infinito
      for (int k = 0; k < nVecinos; k++) {
        if (d_matVecinos[i * nVecinos + k].val < min) {
          min = d_matVecinos[i * nVecinos + k].val;
          pk = k;
        }
      }

      if (d_mat[d_indSim[i] + j].val > min) {
        d_matVecinos[i * nVecinos + pk] = d_mat[d_indSim[i] + j];
      }
    }
  }
}


void construccion_SP_GPU2(Elemento *vecUs, Elemento *vecIt, int *indUs, int *indIt, int nU, int nI, 
  float *medUs, float *medIt, int nVecinos, Elemento **matVecinos, int similitud){
  //Esta función construye el modelo en forma de una matriz dispersa con formato
  //Compresed Sparse Row CSR ya que se tiene un vector de indices que marcan el inicio
  //de cada renglón para el almacenamiento de los datos.

  // clock_t t_1, t_2, t_3, t_4, t_5;
  // double secs;

  cudaError_t err = cudaSuccess;
  Elemento *d_vecUs, *d_vecIt, *d_matVecinos;
  int *d_indUs, *d_indIt, *d_itemsConSimilitud;
  float *d_medUs, *d_medIt;
  size_t size;

  // t_1 = clock(); 

  //copiado en GPU
  // printf("%d", indUs[nU]);
  size = sizeof(Elemento)*indUs[nU];
  err = cudaMalloc(&d_vecUs, size);
  checkCudaErr(err);    
  err = cudaMemcpy(d_vecUs, vecUs, size, cudaMemcpyHostToDevice);
  checkCudaErr(err);

  size = sizeof(Elemento)*indIt[nI];
  err = cudaMalloc(&d_vecIt, size);
  checkCudaErr(err);    
  err = cudaMemcpy(d_vecIt, vecIt, size, cudaMemcpyHostToDevice);
  checkCudaErr(err);

  size = sizeof(int)*(nU+1);
  err = cudaMalloc(&d_indUs, size);
  checkCudaErr(err);    
  err = cudaMemcpy(d_indUs, indUs, size, cudaMemcpyHostToDevice);
  checkCudaErr(err);

  size = sizeof(int)*(nI+1);
  err = cudaMalloc(&d_indIt, size);
  checkCudaErr(err);    
  err = cudaMemcpy(d_indIt, indIt, size, cudaMemcpyHostToDevice);
  checkCudaErr(err);

  size = sizeof(float)*nU;
  err = cudaMalloc(&d_medUs, size);
  checkCudaErr(err);
  err = cudaMemcpy(d_medUs, medUs, size, cudaMemcpyHostToDevice);
  checkCudaErr(err);

  size = sizeof(float)*nI;
  err = cudaMalloc(&d_medIt, size);
  checkCudaErr(err);
  err = cudaMemcpy(d_medIt, medIt, size, cudaMemcpyHostToDevice);
  checkCudaErr(err);
  
  size = sizeof(int)*nI*nI;
  err = cudaMalloc(&d_itemsConSimilitud, size);
  checkCudaErr(err);

  size = sizeof(Elemento)*nI*nVecinos;
  err = cudaMalloc(&d_matVecinos, size);
  checkCudaErr(err);


  // float *med;
  if(similitud == COSENO){
  // if(similitud == PEARSON){
  // construct_sp_gpu_kernel_ccp<<<(nI + BLOCK_SIZE_SP - 1) / BLOCK_SIZE_SP, BLOCK_SIZE_SP>>>(d_vecUs, d_indUs, 
  // d_vecIt, d_indIt, nU, nI, d_medIt, d_itemsConSimilitud, d_mat, d_indSim);
  // }else if(similitud == COSENO){
  construct_sp_gpu_kernel_cos_2<<<(nI + BLOCK_SIZE_SP - 1) / BLOCK_SIZE_SP, BLOCK_SIZE_SP>>>(d_vecUs, d_indUs, 
  d_vecIt, d_indIt, nU, nI, nVecinos, d_itemsConSimilitud, d_matVecinos);
  // }else if(similitud == COSENO_AJUSTADO){
  // construct_sp_gpu_kernel_ad_cos<<<(nI + BLOCK_SIZE_SP - 1) / BLOCK_SIZE_SP, BLOCK_SIZE_SP>>>(d_vecUs, d_indUs, 
  // d_vecIt, d_indIt, nU, nI, d_medUs, d_itemsConSimilitud, d_mat, d_indSim);
  // }else if(similitud == EUCLIDEANO){
  // construct_sp_gpu_kernel_euc<<<(nI + BLOCK_SIZE_SP - 1) / BLOCK_SIZE_SP, BLOCK_SIZE_SP>>>(d_vecUs, d_indUs, 
  // d_vecIt, d_indIt, nU, nI, d_itemsConSimilitud, d_mat, d_indSim);
  }else{
  printf("Error en la selección de función objetivo.\n");
  exit(0);
  }

  matVecinos[0] = (Elemento *) malloc(size);
  err = cudaMemcpy(matVecinos[0], d_matVecinos, size, cudaMemcpyDeviceToHost);
  checkCudaErr(err);

  cudaFree(d_vecUs);
  cudaFree(d_vecIt);
  cudaFree(d_indUs);
  cudaFree(d_indIt);
  cudaFree(d_medUs);
  cudaFree(d_medIt);
  cudaFree(d_itemsConSimilitud);
  cudaFree(d_matVecinos);

  return;
}

// __global__ getMatrixR(Elemento *vecUs, int *indUs, int nU, int nI, float *d_matR){
//   int u = blockIdx.x * blockDim.x + threadIdx.x;
  
//   if(u < nU){
//     for(int i=0; i<nI; i++) d_matR[u*nI+i] = 0;
//     for(int i=0; i<indUs[u+1]-indUs[u]; i++) d_matR[u*nI + vecUs[u+i].ind] = vecUs[u+i].val;
//   }
// }


__global__ void apply_sp_2_Kernel(Elemento *vecUs, int *indUs, int nU, int nI, int nVecinos, int nRecomendaciones, 
  float media, float *bi, float *bu, Elemento *d_matVecinos, Elemento *d_matRecomenciones){
  
  int pmin, i, j, it;
  float min, r_ui;
  int u = blockIdx.x * blockDim.x + threadIdx.x;
  if (u < nU) {
    // float min;
    for (i = 0; i < nI; i++) {
      r_ui = media + bu[u] + bi[i];
      // r_ui = 0;
      for (j = 0; j < nVecinos; j++) {
        it = bbElemGPU(vecUs, indUs[u], indUs[u+1], d_matVecinos[i * nVecinos + j].ind);
        if(it != -1){
          r_ui += d_matVecinos[i * nVecinos + j].val * (vecUs[it].val - media - bu[u] - bi[d_matVecinos[i * nVecinos + j].ind]);
          // r_ui += d_matVecinos[i * nVecinos + j].val * (vecUs[it].val);
        }
      }

      if (bbElemGPU(vecUs, indUs[u], indUs[u+1], i) != -1)
        r_ui = 0;
      
      min = 99999.9;
      for(j=0; j<nRecomendaciones; j++){
        if(d_matRecomenciones[u*nRecomendaciones + j].val < min){
          min = d_matRecomenciones[u*nRecomendaciones + j].val;
          pmin = j;
        }
      }
      if(r_ui > min){
        d_matRecomenciones[u*nRecomendaciones + pmin].val = r_ui;
        d_matRecomenciones[u*nRecomendaciones + pmin].ind = i;
      }
    }

    bubbleSortA(d_matRecomenciones, u, nRecomendaciones);
  }
}


void aplicacion_SP_GPU2(Elemento *matVecinos, Elemento *vecUs, int *indUs, int nU, int nI, 
  B_UI b, int nVecinos, int nRecomendaciones, Elemento **recomendaciones){
    
  cudaError_t err = cudaSuccess;
  Elemento *d_matVecinos, *d_vecUs, *d_recomendaciones;
  int *d_indUs;
  float *d_bi, *d_bu, *d_matR;
  size_t size;

  //copiado en GPU
  size = sizeof(Elemento)*nI*nVecinos;
  err = cudaMalloc(&d_matVecinos, size);
  checkCudaErr(err);
  err = cudaMemcpy(d_matVecinos, matVecinos, size, cudaMemcpyHostToDevice);
  checkCudaErr(err);
  
  size = sizeof(Elemento)*indUs[nU];
  err = cudaMalloc(&d_vecUs, size);
  checkCudaErr(err);    
  err = cudaMemcpy(d_vecUs, vecUs, size, cudaMemcpyHostToDevice);
  checkCudaErr(err);
  
  size = sizeof(int)*(nU+1);
  err = cudaMalloc(&d_indUs, size);
  checkCudaErr(err);    
  err = cudaMemcpy(d_indUs, indUs, size, cudaMemcpyHostToDevice);
  checkCudaErr(err);
  
  size = sizeof(float)*nI;
  err = cudaMalloc(&d_bi, size);
  checkCudaErr(err);
  err = cudaMemcpy(d_bi, b.bi, size, cudaMemcpyHostToDevice);
  checkCudaErr(err);

  size = sizeof(float)*nU;
  err = cudaMalloc(&d_bu, size);
  checkCudaErr(err);
  err = cudaMemcpy(d_bu, b.bu, size, cudaMemcpyHostToDevice);
  checkCudaErr(err);

  size = sizeof(Elemento)*nRecomendaciones*nU;
  err = cudaMalloc(&d_recomendaciones, size);
  checkCudaErr(err);
  err = cudaMemset(d_recomendaciones, 0, size);
  checkCudaErr(err);
  
  apply_sp_2_Kernel<<<(nU + BLOCK_SIZE_SP - 1) / BLOCK_SIZE_SP, BLOCK_SIZE_SP>>>(d_vecUs, d_indUs, nU, nI, 
    nVecinos, nRecomendaciones, b.media, d_bi, d_bu, d_matVecinos, d_recomendaciones);
  
  recomendaciones[0] = (Elemento *) malloc(size);
  err = cudaMemcpy(recomendaciones[0], d_recomendaciones, size, cudaMemcpyDeviceToHost);
  checkCudaErr(err);
  

  cudaFree(d_matVecinos);
  cudaFree(d_vecUs);
  cudaFree(d_indUs);
  cudaFree(d_bi);
  cudaFree(d_bu);
  cudaFree(d_recomendaciones);

  return;
}
