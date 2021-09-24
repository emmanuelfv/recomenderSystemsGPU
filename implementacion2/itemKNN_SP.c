
#include "itemKNN_SP.h"

// void get_indices(Eval *vec, int **indi, int *tam, int tipo){
//   //Esta función se encarga de retornar una lista con los indices 
//   //en los cuales los ítems/usuarios tengan su posición principal
//   //se necesitan los datos ordenados
//   //Ejemplo: v:[(1, 2), (1,3), (2,1), (2,3)], ind:[0,2]

//   //printf("%d, %d, %f\n", vec->REC[i].us, vec->REC[i].it, vec->REC[i].rat);
//   indi[0] = (int*) malloc(sizeof(int));

//   indi[0][0] = 0;
//   int actual, j=1; 

//   if(tipo == 0){
//     actual = vec->REC[0].us;
//     for(int i =1; i<vec->tREC; i++){
//       if(actual != vec->REC[i].us){
//         actual = vec->REC[i].us;
//         indi[0] = (int *) realloc(indi[0], sizeof(int)*(j+1));
//         indi[0][j++] = i;
//       }
//     }
//   }else{
//     actual = vec->REC[0].it;
//     for(int i = 1; i<vec->tREC; i++){
//       if(actual != vec->REC[i].it){
//         actual = vec->REC[i].it;
//         indi[0] = (int *) realloc(indi[0], sizeof(int)*(j+1));
//         indi[0][j++] = i;
//       }
//     }
//   }
//   indi[0] = (int *) realloc(indi[0], sizeof(int)*(j+1));
//   indi[0][j++] = vec->tREC;
 
//   *tam = j-1;
//   return;
// }

// void getMediaU(Eval *vec, int *indUs, float **med){
//   float sum;
//   med[0]= (float*) malloc(sizeof(float)*vec->numU);
//   for(int i=0; i< vec->numU; i++){
//     sum = 0;
//     for(int j=0 ; j<indUs[i+1]-indUs[i]; j++){
//       sum += vec->REC[indUs[i]+j].rat;
//     }
//     med[0][i] = sum/(indUs[i+1]-indUs[i]);
//   }
//   return;
// }

// void getMediaI(Eval *vec, int *ind, float **med){
//   float sum;
//   med[0]= (float*) malloc(sizeof(float)*vec->numI);
//   for(int i=0; i< vec->numI; i++){
//     sum = 0;
//     for(int j=0 ; j<ind[i+1]-ind[i]; j++){
//       sum += vec->REC[ind[i]+j].rat;
//     }
//     med[0][i] = sum/(ind[i+1]-ind[i]);
//   }
//   return;
// }

// float similitudAdCos(Eval *vecIt, int *indIt, float *medUs, int nI, int itemI, int itemJ, int h){
//   // funcion coseno ajustado
//   //preserentemente, se debe tener vecIt centrado en la media del usuario
//   //es decir: u = [3,3,5,5]; medU = 4 -> u' = [-1,-1,1,1]
//   int i=0, j=0, cont=0;
//   float sum1, sum2, sum3, ri, rj;
//   sum1 = sum2 = sum3 = 0;

//   // printf("%d %d\n", indIt[itemI+1]-indIt[itemI], indIt[itemJ+1]-indIt[itemJ]);
//   while(i<indIt[itemI+1]-indIt[itemI] && j<indIt[itemJ+1]-indIt[itemJ]){
//     // printf("%d %d..\n", vecIt->REC[indIt[itemI]+i].us, vecIt->REC[indIt[itemJ]+j].us);
//     if(vecIt->REC[indIt[itemI]+i].us == vecIt->REC[indIt[itemJ]+j].us){
//       ri = vecIt->REC[indIt[itemI]+i].rat - medUs[vecIt->REC[indIt[itemI]+i].us];
//       rj = vecIt->REC[indIt[itemJ]+j].rat - medUs[vecIt->REC[indIt[itemJ]+j].us];
//       sum1 += ri * rj;
//       sum2 += ri * ri;
//       sum3 += rj * rj;  
//       cont++;
//       i++;
//       j++;

//     }
//     else if(vecIt->REC[indIt[itemI]+i].us > vecIt->REC[indIt[itemJ]+j].us) j++;
//     else i++;
//   }
//   if (sum2*sum3 == 0) return 0;

//   float res = sum1/sqrt(sum2*sum3);
//   if(cont < h) return res*((float) cont/h);
//   // else return res;
//   // printf("%f\n", res);
//   return res;

// }

// void construccion_SP_GPU(Eval *vecUs, Eval *vecIt, int *indUs, int *indIt, float *medUs, 
//                 Elemento **mat, int **indSim, int *matTam, int similitud){
//   //Esta función construye el modelo en forma de una matriz dispersa con formato
//   //Compresed Sparse Row CSR ya que se tiene un vector de indices que marcan el inicio
//   //de cada renglón para el almacenamiento de los datos.

//   int i, u, us, j, it, nI = vecUs->numI, l=0; 
//   int cont=0;
//   int *itemsConSimilitud = (int *) calloc(nI, sizeof(int));
  
//   indSim[0] = (int*) malloc((1+nI)*sizeof(int));

//   mat[0] = (Elemento *) malloc(sizeof(Elemento));

//   indSim[0][0] = 0;

//   // int max=0, pmax;

//   for(i=0; i<nI; i++){
//     for(u=0; u<indIt[i+1]-indIt[i]; u++){
//       us = vecIt->REC[indIt[i]+u].us;
//       for(j=0; j<indUs[us+1]-indUs[us]; j++){
//         it = vecUs->REC[indUs[us]+j].it;
//         if(i != it){
//           itemsConSimilitud[it] = 1;
//         }
//       }
//     }

//     for(j=0; j<nI; j++){
//       if(itemsConSimilitud[j]){
//         cont++;
//       }
//     }

//     mat[0] = (Elemento *) realloc(mat[0], (1+cont)*sizeof(Elemento));

//     indSim[0][i+1] = cont;
//     for(j=0; j<nI; j++){
//       if(itemsConSimilitud[j]){
//         itemsConSimilitud[j]--;
//         mat[0][l].ind = j;
//         mat[0][l++].val = similitudAdCos(vecIt, indIt, medUs, nI, i, j, 5);
        
//         // if(i<j)
//         //   mat[0][l++].val = similitudAdCos(vecIt, indIt, medUs, nI, i, j);
//         // else{
//         //   mat[0][l++].val = mat[0][indSim[0][j]+i].val;
//         // }
//         // if(cont1 == 3) return res;
//       }
//     }
//   }
//   *matTam = l;
//   // for(int i=0; i<nI; i++){
//   //   printf("%d\n", indSim[0][i+1]-indSim[0][i]);
//   //   for(int j=0; j<indSim[0][i+1]-indSim[0][i]; j++){
//   //     printf("%d %d %f\n", i, mat[0][indSim[0][i] + j].ind, mat[0][indSim[0][i] + j].val);
//   //   }
//   // }

//   printf("%d datos de %.0Lf posibles\n", *matTam, (long double)nI * nI);
//   printf("ok\n");

//   return;
// }

// void aplicacion_SP_GPU(Elemento *mat, int *indSim, Eval *vecUs, int *indUs, Eval *vecIt,
//   int *indIt, float *medIt, int nVecinos, int nRecomendaciones, Elemento **recomendaciones){
//   //En la aplicación del modelo se tiene el resultado para cada usuario de las
  
//   int u, i, r, j, it, iit, k, pmin, l;
//   float min;
//   int nI = vecUs->numI;
//   int nU = vecUs->numU;
//   recomendaciones[0] = (Elemento *) malloc(sizeof(Elemento) * nU * nRecomendaciones);
//   // x recomendaciones. 
//   Elemento *vecinos = (Elemento*) calloc(nI, sizeof(Elemento)); 
//   Elemento *topVecinos = (Elemento*) calloc(nVecinos, sizeof(Elemento));
//   Elemento *topRec = (Elemento *) calloc(nRecomendaciones, sizeof(Elemento));
//   float x1, x2, rui;
//   int indV;


//   for(u=0; u<nU; u++){
//     r=0;
//     for(i=0; i<nI; i++){
//       if(i == vecUs->REC[indUs[u]+r].it)
//         r++;
//       else{ 
//         //i es recomendable, se calculará rui

//         //Busqueda de buenos vecinos
//         k=0;
//         for(j=0; j<indUs[u+1]-indUs[u]; j++){
//           it = vecUs->REC[indUs[u]+j].it;
//           iit = bbElem(mat, indSim[i], indSim[i+1], it);
//           // if(u==2 && i==1121) printf("%d %d %d %d\n", u, i, it, iit);
//           if(iit != -1){
//             // printf("%d %d %d %d\n", indSim[i] + iit, i,indSim[i], iit );
//             if(mat[iit].val > 0.2){
//               vecinos[k++] = mat[iit];
//             }
//           }
//         }

//         //delimitación de vecinos
//         if(k==0){
//           continue;
//         }
//         if(k>nVecinos){
//           for(j=0; j<k; j++){
//             min = MAX_FLOAT;
//             for(l=0; l<nVecinos; l++){
//               if(topVecinos[l].val < min){
//                 min = topVecinos[l].val;
//                 pmin = l;
//               }
//             }
//             if (vecinos[j].val > min){
//               topVecinos[pmin] = vecinos[j];
//             }
//           }
//           k = nVecinos;
//         }
//         else{
//           for(j=0; j<k; j++){
//             topVecinos[j] = vecinos[j];
//           }
//         }
//         // else {
//         //   printf("falla %d %d %d\n", u, i, k);
//         //   // for(j=0; j<vecinos)
//         //   return;
//         // }
//         // continue;

//         //calculo r_ui
//         x1 = 0;
//         x2 = 0;
//         for(j=0; j<k; j++){
//           indV = bbEvalUs(vecUs->REC, indUs[u], indUs[u+1], topVecinos[j].ind);
//           // if(u>900 && i==1595) printf("%d %d %f %f %f\n", j, topVecinos[j].ind, medIt[j], vecUs->REC[indV].rat, topVecinos[j].val);
//           // if(u>900 && i==1595) printf("%f %f %f\n", x1, x2, vecUs->REC[indV].rat - medIt[j]);
          
//           x1 += topVecinos[j].val * (vecUs->REC[indV].rat - medIt[j]);
//           x2 += topVecinos[j].val;
//         }
//         rui = medIt[i] + x1/x2;

//         // if(u==3) printf("%d %f\n", i, rui);

//         //colocación de mejores items para u
//         min = MAX_FLOAT;
//         for(j=0; j<nRecomendaciones; j++){
//           if(topRec[j].val < min){
//             min = topRec[j].val;
//             pmin = j;
//           }
//         }
//         if(rui > min){
//           topRec[pmin].val = rui;
//           topRec[pmin].ind = i;
//         }

//       }
//     }
    
//     //copiado de recomendaciones en "recomendaciones"
//     qsort(topRec, nRecomendaciones, sizeof(Elemento), cmpfuncElem);
//     rui = 0;
//     for(i=0; i<nRecomendaciones; i++){
//       recomendaciones[0][u*nRecomendaciones + i] = topRec[i];
//       rui += recomendaciones[0][u*nRecomendaciones + i].val;
//       // printf("%d %f\n", topRec[i].ind, topRec[i].val);
//     }
//     printf("%d %f\n", u, rui);
//     // memset(topRec, 0, nRecomendaciones*sizeof(Elemento));
//     memset(topVecinos, 0, nVecinos*sizeof(Elemento));
//     memset(topRec, 0, nRecomendaciones*sizeof(Elemento));
  
//   }

//   free(vecinos);
//   free(topVecinos);
//   free(topRec);
//   printf("ok2\n");
//   return;
// }


/*
1
195846 datos de 2829124 posibles
2
356066 datos de 13734436 posibles
3
2723222 datos de 113998329 posibles
4
5760113 datos de -808419087 posibles

6
524819 datos de 125440000 posibles
8
497497 datos de 1000000 posibles
10
9414 datos de 1000000 posibles
5
0 datos de 14953257 posibles*/

/*
195846 datos de 2829124 posibles
real	0m0.715s
356066 datos de 13734436 posibles
real	0m4.756s
2723222 datos de 113998329 posibles
real	3m38.696s
5760113 datos de 3486548209 posibles
real	12m47.579s
9414 datos de 1000000 posibles
real	0m0.586s
497497 datos de 1000000 posibles
real	0m11.980s
524819 datos de 125440000 posibles
real	0m29.970s*/

float similitudCCP(Elemento *vecIt, int *indIt, float *medIt, int nI, int itemI, int itemJ, int h){
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

float similitudCos(Elemento *vecIt, int *indIt, float *null1, int nI, int itemI, int itemJ, int h){
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

float similitudAdCos(Elemento *vecIt, int *indIt, float *medUs, int nI, int itemI, int itemJ, int h){
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

float similitudEuc(Elemento *vecIt, int *indIt, float *medUs, int nI, int itemI, int itemJ, int h){
  // funcion coseno ajustado
  //preserentemente, se debe tener vecIt centrado en la media del usuario
  //es decir: u = [3,3,5,5]; medU = 4 -> u' = [-1,-1,1,1]
  int i=0, j=0, cont=0;
  float sum1, ri, rj;
  sum1 = 0;

  // printf("%d %d\n", indIt[itemI+1]-indIt[itemI], indIt[itemJ+1]-indIt[itemJ]);
  while(i<indIt[itemI+1]-indIt[itemI] && j<indIt[itemJ+1]-indIt[itemJ]){
    // printf("%d %d..\n", vecIt->REC[indIt[itemI]+i].us, vecIt->REC[indIt[itemJ]+j].us);
    if(vecIt[indIt[itemI]+i].ind == vecIt[indIt[itemJ]+j].ind){
      
      // ri = vecIt[indIt[itemI]+i].val - medUs[vecIt[indIt[itemI]+i].ind];
      // rj = vecIt[indIt[itemJ]+j].val - medUs[vecIt[indIt[itemJ]+j].ind];
      ri = vecIt[indIt[itemI]+i].val;
      rj = vecIt[indIt[itemJ]+j].val;
      sum1 += (ri - rj)*(ri - rj);
      cont++;
      i++;
      j++;

    }
    else if(vecIt[indIt[itemI]+i].ind > vecIt[indIt[itemJ]+j].ind) j++;
    else i++;
  }
  float res = 1/(1 - sqrt(sum1));
  if(cont < h) return res*((float) cont/h);
  return res;
}

void construccion_SP(Elemento *vecUs, Elemento *vecIt, int *indUs, int *indIt, int nU, int nI, 
                float *medUs, float *medIt, Elemento **mat, int **indSim, int similitud){
  //Esta función construye el modelo en forma de una matriz dispersa con formato
  //Compresed Sparse Row CSR ya que se tiene un vector de indices que marcan el inicio
  //de cada renglón para el almacenamiento de los datos.

  int i, u, us, j, it, l=0; 
  int cont=0;
  int *itemsConSimilitud = (int *) calloc(nI, sizeof(int));
  
  indSim[0] = (int*) malloc((1+nI)*sizeof(int));

  mat[0] = (Elemento *) malloc(sizeof(Elemento));

  indSim[0][0] = 0;

  // printf("ok\n");
  //     printf("%d\n", i);
  //   continue;
  // int max=0, pmax;
  float (*fun)(Elemento *, int *, float *, int , int , int , int);
  float *med;
  if(similitud == PEARSON){
    fun = similitudCCP;
    med = medIt; 
  }else if(similitud == COSENO){
    fun = similitudCos;
  }else if(similitud == COSENO_AJUSTADO){
    fun = similitudAdCos;
    med = medUs; 
  }else if(similitud == EUCLIDEANO){
    fun = similitudEuc;
  }else{
    printf("Error en la selección de función objetivo.\n");
    exit(0);
  }

  for(i=0; i<nI; i++){
    for(u=0; u<indIt[i+1]-indIt[i]; u++){
      us = vecIt[indIt[i]+u].ind;
      for(j=0; j<indUs[us+1]-indUs[us]; j++){
        it = vecUs[indUs[us]+j].ind;
        if(i != it){
          itemsConSimilitud[it] = 1;
        }
      }
    }

    for(j=0; j<nI; j++){
      if(itemsConSimilitud[j]){
        cont++;
      }
    }

    mat[0] = (Elemento *) realloc(mat[0], (1+cont)*sizeof(Elemento));

    indSim[0][i+1] = cont;
    for(j=0; j<nI; j++){
      if(itemsConSimilitud[j]){
        itemsConSimilitud[j]--;
        mat[0][l].ind = j;
        mat[0][l++].val = fun(vecIt, indIt, med, nI, i, j, 1);
        
        // if(i<j)
        //   mat[0][l++].val = similitudAdCos(vecIt, indIt, medUs, nI, i, j);
        // else{
        //   mat[0][l++].val = mat[0][indSim[0][j]+i].val;
        // }
        // if(cont1 == 3) return res;
      }
    }
  }
  // for(int i=0; i<nI; i++){
  //   printf("%d\n", indSim[0][i+1]-indSim[0][i]);
  //   for(int j=0; j<indSim[0][i+1]-indSim[0][i]; j++){
  //     printf("%d %d %f\n", i, mat[0][indSim[0][i] + j].ind, mat[0][indSim[0][i] + j].val);
  //   }
  // }


  // printf("%d datos de %.0Lf posibles\n", *matTam, (long double)nI * nI);
  // printf("ok\n");

  free(itemsConSimilitud);
  return;
}


void aplicacion_SP(Elemento *mat, int *indSim, Elemento *vecUs, int *indUs, Elemento *vecIt,
  int *indIt, int nU, int nI, B_UI b, int nVecinos, int nRecomendaciones, Elemento **recomendaciones){
  //En la aplicación del modelo se tiene el resultado para cada usuario de las
  
  int u, i, r, j, it, iit, k, pmin, l, flag;
  float min;
  recomendaciones[0] = (Elemento *) malloc(sizeof(Elemento) * nU * nRecomendaciones);
  // x recomendaciones. 
  // Elemento *vecinos = (Elemento*) calloc(nI, sizeof(Elemento)); 
  Elemento *topVecinos = (Elemento*) calloc(nVecinos, sizeof(Elemento));
  Elemento *topRec = (Elemento *) calloc(nRecomendaciones, sizeof(Elemento));
  float x1, rui;//, x2;
  int indV;


  for(u=0; u<nU; u++){
    r=0;
    flag=0;
    // if(u==100) for(j=0; j<indUs[u+1]-indUs[u]; j++) printf("--r%d, i%d r%f\n", u, vecUs[indUs[u]+j].ind, vecUs[indUs[u]+j].val);
    // if(u==100) printf("%d\n", indUs[u+1] - indUs[u]);

    for(i=0; i<nI; i++){
      // if(u==100 && i < 20) printf("aaa  %d %d %d\n", i, vecUs[indUs[u]+r].ind, flag); 
      if(i == vecUs[indUs[u]+r].ind && flag==0){
        r++;
        if(r == indUs[u+1] - indUs[u]) flag++;
        // if(u==100) printf("%d\n", i);
      }
      
      else{ 
        //i es recomendable, se calculará rui

        // //busqueda2
        // k = indSim[i+1] - indSim[i];
        // if(k > nVecinos){
        //   for(j=0; j<k; j++){
        //     min = MAX_FLOAT;
        //     for(l=0; l<nVecinos; l++){
        //       if(topVecinos[l].val < min){
        //         min = topVecinos[l].val;
        //         pmin = l;
        //       }
        //     }
        //     if (mat[indSim[i]+j].val > min){
        //       topVecinos[pmin] = mat[indSim[i]+j];
        //     }
        //   }
        //   k = nVecinos;
        // }
        // else{
        //   for(j=0; j<k; j++){
        //     topVecinos[j] = mat[indSim[i]+j];
        //   }
        // }

        // // for(j=0; j<k; j++){
        // //   if(u==100 && i < 20) printf("bbb %d %d %f\n", i, topVecinos[j].ind, topVecinos[j].val);
        // // }

        // x1 = 0;
        // for(j=0; j<k; j++){
        //   indV = bbElem(vecUs, indUs[u], indUs[u+1], topVecinos[j].ind);
        //   // if(u==100 && i < 20) printf("   %d\n", indV);
        //   if(indV != -1){
        //     x1 += topVecinos[j].val * vecUs[indV].val;
        //     // if(u==100 && i < 20) printf("      %f %f %f\n", x1, topVecinos[j].val, vecUs[indV].val);
        //   }
        // }
        // rui = x1;

        // // if(u==100 && i < 20) printf("%d %f %d\n", i, rui, k);

        //colocación de mejores items para u
        // min = MAX_FLOAT;
        // for(j=0; j<nRecomendaciones; j++){
        //   if(topRec[j].val < min){
        //     min = topRec[j].val;
        //     pmin = j;
        //   }
        // }
        // if(rui > min){
        //   topRec[pmin].val = rui;
        //   topRec[pmin].ind = i;
        // }


        // if(u==300) printf("%d %f\n", i, rui);


        // Busqueda de buenos vecinos
        k=0;
        for(j=0; j<indUs[u+1]-indUs[u]; j++){
          it = vecUs[indUs[u]+j].ind;
          iit = bbElem(mat, indSim[i], indSim[i+1], it);
          // if(u==2 && i==1121) printf("%d %d %d %d\n", u, i, it, iit);
          if(iit != -1){
            // printf("%d %d %d %d\n", indSim[i] + iit, i,indSim[i], iit );
            if(mat[iit].val > 0){
              //se inserta (si es) dentro de los mejores
              //vecinos[k++] = mat[iit];
              k++;
              if(k<=nVecinos){
                topVecinos[k-1] = mat[iit];
              } else {
                min = MAX_FLOAT;
                for(l=0; l<nVecinos; l++){
                  if(topVecinos[l].val < min){
                    min = topVecinos[l].val;
                    pmin = l;
                  }
                }
                if (mat[iit].val > min){
                  topVecinos[pmin] = mat[iit];
                }
              }
            }
          }
        }

        if(k>nVecinos) k = nVecinos;
        // // delimitación de vecinos
        // if(k==0){
        //   continue;
        // }
        // if(k>nVecinos){
        //   for(j=0; j<k; j++){
        //     min = MAX_FLOAT;
        //     for(l=0; l<nVecinos; l++){
        //       if(topVecinos[l].val < min){
        //         min = topVecinos[l].val;
        //         pmin = l;
        //       }
        //     }
        //     if (vecinos[j].val > min){
        //       topVecinos[pmin] = vecinos[j];
        //     }
        //   }
        //   k = nVecinos;
        // }
        // else{
        //   for(j=0; j<k; j++){
        //     topVecinos[j] = vecinos[j];
        //   }
        // }   

        // if(u == 300 && i % 500 == 0){
        //   for(j=0; j<k; j++){
        //     printf("%d %d %d %f\n", u, j, topVecinos[j].ind, topVecinos[j].val);
        //   }
        //   printf("\n");
        // }
        
        // if(k > nVecinos) printf("falla: %d %d %d\n", u, i, k);
        
        // if(u==100 && i < 20){
        //   for(j=0; j<k; j++){
        //     printf("bbb %d %d %f\n", i, topVecinos[j].ind, topVecinos[j].val);
        //   }
        //   printf("\n");
        // }

        // calculo r_ui
        x1 = 0;
        // x2 = 0;
        for(j=0; j<k; j++){
          indV = bbElem(vecUs, indUs[u], indUs[u+1], topVecinos[j].ind);
          // if(u>900 && i==1595) printf("%d %d %f %f %f\n", j, topVecinos[j].ind, medIt[j], vecUs->REC[indV].rat, topVecinos[j].val);
          // if(u>900 && i==1595) printf("%f %f %f\n", x1, x2, vecUs->REC[indV].rat - medIt[j]);
          
          x1 += topVecinos[j].val * (vecUs[indV].val - b.media - b.bu[u] - b.bi[topVecinos[j].ind]);
          // x2 += topVecinos[j].val;
          //  x1 += topVecinos[j].val * vecUs[indV].val;
        }
        // rui = medIt[i] + x1/x2;
        rui = x1 + b.media + b.bu[u] + b.bi[i];


        // if(u==3) printf("%d %f\n", i, rui);

        //colocación de mejores items para u
        min = MAX_FLOAT;
        for(j=0; j<nRecomendaciones; j++){
          if(topRec[j].val < min){
            min = topRec[j].val;
            pmin = j;
          }
        }
        if(rui > min){
          topRec[pmin].val = rui;
          topRec[pmin].ind = i;
        }
      }
      memset(topVecinos, 0, nVecinos*sizeof(Elemento));
    }
    // for(i=0; i<nRecomendaciones; i++){
    //     if(u % 200 == 0) printf("%d %f %f\n", u, rui, min);
    // }    
    //copiado de recomendaciones en "recomendaciones"
    qsort(topRec, nRecomendaciones, sizeof(Elemento), cmpfuncElem);
    rui = 0;
    for(i=0; i<nRecomendaciones; i++){
      recomendaciones[0][u*nRecomendaciones + i] = topRec[i];
      rui += recomendaciones[0][u*nRecomendaciones + i].val;
      // printf("%d %f\n", topRec[i].ind, topRec[i].val);
    }
    // printf("%d %f\n", u, rui);
    memset(topRec, 0, nRecomendaciones*sizeof(Elemento));
  
  }

  // free(vecinos);
  free(topVecinos);
  free(topRec);
  // printf("ok2\n");
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

amazon_0.1%
399354 datos de 1000000 posibles
396832 datos de 1000000 posibles
397738 datos de 1000000 posibles
396401 datos de 1000000 posibles
398916 datos de 1000000 posibles

*/


void get_b(Elemento *vecUs, Elemento *vecIt, int *indUs, int *indIt, int nU, int nI, B_UI *b){
  
  b->bu = (float *) malloc(sizeof(float)*nU);
  b->bi = (float *) malloc(sizeof(float)*nI);

  int lamda2 = 10;
  int lamda3 = 5;
  

  b->media = 0;
  for(int i=0; i<indUs[nU]; i++) b->media += vecUs[i].val/indUs[nU];

  for(int i=0; i<nI; i++)
  {
    b->bi[i] = 0;
    for(int j=0; j<indIt[i+1] - indIt[i]; j++)
    {
      b->bi[i] += vecIt[indIt[i] + j].val - b->media;
    }
    b->bi[i] /= lamda2 + (indIt[i+1] - indIt[i]);
  }

  for(int i=0; i<nU; i++)
  {
    b->bu[i] = 0;
    for(int j=0; j<indUs[i+1] - indUs[i]; j++)
    {
      b->bu[i] += vecUs[indUs[i] + j].val - b->media - b->bi[vecUs[indUs[i] + j].ind];
    }
    b->bu[i] /= lamda3 + (indUs[i+1] - indUs[i]);    
  }

  return;
}