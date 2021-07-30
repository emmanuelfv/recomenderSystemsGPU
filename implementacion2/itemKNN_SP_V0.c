


#ifndef _ITEMKNN_SP_H
#define _ITEMKNN_SP_H
#endif


// #ifndef _STDIO_H
// #include <stdio.h>
// #endif

// #ifndef _STDLIB_H
// #include <stdlib.h>
// #endif

// #ifndef _STRING_H
// #include <string.h>
// #endif

// #include <math.h>

// #ifndef _LECTURA_RS_H
// #include "../bibliotecas/lecturaRS.h"
// #endif

// #ifndef _NORMALIZACION_H
// #include "../bibliotecas/normalizacion.h"
// #endif

// #ifndef LIBPQ_FE_H
// #include <postgresql/libpq-fe.h>
// #endif

// #ifndef _LECTURA_PSQL_H
// #include "../bibliotecas/lecturaPSQL.h"
// #endif

// #ifndef _TIME_H
// #include <time.h>
// #endif

#include <math.h>
#include <postgresql/libpq-fe.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "../bibliotecas/lecturaRS.h"
#include "../bibliotecas/normalizacion.h"


#include "itemKNN_SP.h"



//void aleatoriza(EvalR *vec, int tam);
// void crossValidationT(Eval *vec, int k, int tam);

// float **entrenamiento(EvalR *train, int trainTam, LISTS *trainL);
// EvalR *limpiezaTest(EvalR *test, int *testTam, LISTS *trainL);
// float *evaluacion(EvalR *test, int testTam, float **mat, LISTS *trainL);

// int bb2(int *vector, int m, int n, int val);

//-------------------------------------------------------------------------------
// void main(int argc, char **argv){
//   srand(time(NULL));

//   Eval *vec = leerDB(atoi(argv[1]));

//   crossValidationT(vec, 5, vec->tREC);

//   //Eval *vecLectura, int k, int tam;

//   freeEval(vec);

// }



//----------------------------------------------------------------------------
//funciones de similitud
//similitudes p1, p2, p3 y euc aplican solo para filtros de usuarios,
//para filtros de items se aplicará la de cosAdj (coseno ajutado)

//funciona sobre los indices de los usuarios y encuentra la similitud de estos;
double similitud_p1(VL *listaU, int tamUs, int u1, int u2){ 
  int i, j; //, k, tam;
  float sum1, sum2, sum3;
  if(u1 == u2) return 1;

  //unsigned int *sim = (udouble similitud_p1(VL *listaU, int tamUs, int u1, int u2){ 
  i=0;
  j=0;
  sum1 = sum2 = sum3 = 0;
  while(i<listaU[u1].t && j<listaU[u2].t){  
    if(listaU[u1].l[i] == listaU[u2].l[j]){
      //sim = realloc(sim, ++tam*sizeof(unsigned int));///---------------
      //sim[tam-1] = listaU[u1].l[i]; //---------------
      sum1 += listaU[u1].rat[i]*listaU[u2].rat[j];
      sum2 += listaU[u1].rat[i]*listaU[u1].rat[i];
      sum3 += listaU[u2].rat[j]*listaU[u2].rat[j];  
      i++;
      j++;
    }
    else if(listaU[u1].l[i] > listaU[u2].l[j]) j++;
    else i++;
  }

  //for(i=0; i<tam; i++) printf("%d\n", sim[i]);

  //printf("%f %f %f---\n", sum1, sum2, sum3);
  if (sum1 == 0) return -1;
  return sum1/(sqrt(sum2)*sqrt(sum3));
}

double similitud_p2(VL *listaU, int tamUs, int u1, int u2, int humbral){ 
  int i, j, cont; //k, tam;
  float sum1, sum2, sum3, penalizacion;
  if(u1 == u2) return 1;
  //encontrando conjunto items(u1) disyuncion items(u2):
  //unsigned int *sim = (unsigned int *) malloc(sizeof(unsigned int));//------------
  //tam = 0;//-------------------------------------
  i= j = cont = 0;
  sum1 = sum2 = sum3 = 0;
  while(i<listaU[u1].t && j<listaU[u2].t){
    
    if(listaU[u1].l[i] == listaU[u2].l[j]){
      //sim = realloc(sim, ++tam*sizeof(unsigned int));///---------------
      //sim[tam-1] = listaU[u1].l[i]; //---------------
      sum1 += listaU[u1].rat[i]*listaU[u2].rat[j];
      sum2 += listaU[u1].rat[i]*listaU[u1].rat[i];
      sum3 += listaU[u2].rat[j]*listaU[u2].rat[j];  
      i++;
      j++;
      cont++;
    }
    else if(listaU[u1].l[i] > listaU[u2].l[j]) j++;
    else i++;
  }
  if(cont < humbral) penalizacion = (float) cont/humbral;
  else penalizacion = 1;
  //for(i=0; i<tam; i++) printf("%d\n", sim[i]);

  //printf("%f %f %f---\n", sum1, sum2, sum3);
  if (sum1 == 0) return -1;
  return penalizacion * sum1/(sqrt(sum2)*sqrt(sum3));
}


double similitud_p3(VL *listaU, int tamUs, int u1, int u2, int humbral, VIC *listaI, int tamIt){ 
  int i, j, cont; //, k, tam
  float sum1, sum2, sum3;//, penalizacion;
  int m;
  float w;
  if(u1 == u2) return 1;
  //encontrando conjunto items(u1) disyuncion items(u2):
  //unsigned int *sim = (unsigned int *) malloc(sizeof(unsigned int));//------------
  //tam = 0;//-------------------------------------
  i= j = cont = 0;
  sum1 = sum2 = sum3 = 0;
  while(i<listaU[u1].t && j<listaU[u2].t){
    if(listaU[u1].l[i] == listaU[u2].l[j]){
      m = listaI[bb3(listaI, 0, tamIt, listaU[u1].l[i])].c;
      w = log2((float) tamUs/m);

      
      //sim = realloc(sim, ++tam*sizeof(unsigned int));///---------------
      //sim[tam-1] = listaU[u1].l[i]; //---------------
      sum1 += listaU[u1].rat[i]*listaU[u2].rat[j]*w;
      sum2 += listaU[u1].rat[i]*listaU[u1].rat[i]*w;
      sum3 += listaU[u2].rat[j]*listaU[u2].rat[j]*w;
      i++;
      j++;
      cont++;
    }
    else if(listaU[u1].l[i] > listaU[u2].l[j]) j++;
    else i++;
  }

  if (sum2*sum3 == 0) return -1;
  if(cont < humbral) return (sum1/sqrt(sum2*sum3))*((float) cont/humbral);
  else return sum1/sqrt(sum2*sum3);
  
}


double similitud_euc(VL *listaU, int tamUs, int u1, int u2){ 
  int i, j, cont;
  float sum1;
  if(u1 == u2) return 1;
  //encontrando conjunto items(u1) disyuncion items(u2):
  //unsigned int *sim = (unsigned int *) malloc(sizeof(unsigned int));//------------
  //tam = 0;//-------------------------------------
  i= j = cont = 0;
  sum1 = 0;
  while(i<listaU[u1].t && j<listaU[u2].t){
    if(listaU[u1].l[i] == listaU[u2].l[j]){
      //sim = realloc(sim, ++tam*sizeof(unsigned int));///---------------
      //sim[tam-1] = listaU[u1].l[i]; //---------------
      sum1 += ((listaU[u1].rat[i]-listaU[u2].rat[j])/9)*((listaU[u1].rat[i]-listaU[u2].rat[j])/9);
      i++;
      j++;
      cont++;
    }
    else if(listaU[u1].l[i] > listaU[u2].l[j]) j++;
    else i++;
  }
  if (cont == 0) return -1;
  return 1 - sum1/cont;
}

double similitud_adjCos(VL *listaI, int tamIt, int i1, int i2, int h){
  int i, j, cont;
  float sum1, sum2, sum3;
  if(i1 == i2) return 1;

  //unsigned int *sim = (udouble similitud[_p1(VL *listaU, int tamUs, int u1, int u2){ 
  i=0;
  j=0;
  cont=0;
  sum1 = sum2 = sum3 = cont = 0;
  // funcion coseno ajustado
  while(i<listaI[i1].t && j<listaI[i2].t){
    if(listaI[i1].l[i] == listaI[i2].l[j]){
      sum1 += listaI[i1].rat[i]*listaI[i2].rat[j];
      sum2 += listaI[i1].rat[i]*listaI[i1].rat[i];
      sum3 += listaI[i2].rat[j]*listaI[i2].rat[j];  
      cont++;
      i++;
      j++;
    }
    else if(listaI[i1].l[i] > listaI[i2].l[j]) j++;
    else i++;
  }
  if (sum2*sum3 == 0) return -1;

  float res1 = sum1/sqrt(sum2*sum3);
  if(cont < h) return  res1*((float) cont/h);
  else return res1;
}

//---------------------------------------------------------------------------
//recomendación y evaluación del sistema
float recomendacion(float **M, VL *listaU, VM* medUs, int tamUs, int U, int I, int noUs){
  return 0.1;
  //obtencion del indice de usuarios similares
  int i, inU, inI, cont;
  float sum;

  inU = bb1(listaU, 0, tamUs, U);
  if(inU == -1) return -1;
  VM *vec = (VM*) malloc(sizeof(VM)*tamUs);
  for(i=0; i<tamUs; i++){
    vec[i].med = M[inU][i];
    vec[i].x = i;
  }
  
  qsort(vec, tamUs, sizeof(VM), cmpfuncVM);

  cont=0;
  sum = 0;
  inI = 0;

  for(i=0; i<tamUs && cont< noUs; i++){
    inI = (int) bb2((int *)listaU[i].l, 0, listaU[i].t, I);
    if(inI < 0) continue;
    sum += vec[i].med * listaU[i].rat[inI];
    cont++;
    //printf("sum %f %f %f %d\n", sum, vec[i].med, listaU[i].rat[inI2], inI2);
  }
  //printf("total: %f %d\n", sum, cont);
  if (cont == 0) return -1;
  free(vec);
  return  medUs[inU].med + sum/cont;
}

//-----------------------------------------------------------------------------------
float evaluacionSistema(EvalR *Test, int n, float **M, VL *listaU, VM *medUs, int tamUs, int noUs){
  int i, cont;
  float sum;
  float *vec = (float *) malloc(sizeof(float)*n);

  sum = 0;
  cont = 0;
  for(i=0; i<n; i++){
    //if(i%500==0) printf("%d", i); 
    float a;
    int b=0;

    a = recomendacion(M, listaU, medUs, tamUs, Test[i].us, Test[i].it, noUs);
    if(a != -1) vec[cont] = a;
    else b++;
    
    sum += (Test[i].rat - vec[cont])*(Test[i].rat - vec[cont]);
    cont++;
    //if(i % 10 == 0) printf("%4d: %5d %5d %2.6f %2.6f %2.6f\n", i, Test[i].x, Test[i].it, Test[i].rat, vec[i], sum);
  }

  free(vec);
  if(cont==0) return 0;
  else return sqrt(sum/cont++);
}


//---------------------------------------------------------------------------------
float recomendacionItem(float **M, VL *listaI, VM* medUs, int tamIt, int U, int I, int noIt, int tamUs){
  //obtencion del indice de usuarios similares
  int i, inU, inI, cont, inI2;
  float sum;

  inI = bb1(listaI, 0, tamIt, I);
  inU = bbVM(medUs, 0, tamUs, U);

  if(inI == -1) return 2000;
  VM *vec = (VM*) malloc(sizeof(VM)*tamIt);
  for(i=0; i<tamIt; i++){
    vec[i].med = M[inI][i];
    vec[i].x = i;
  }
  
  qsort(vec, tamIt, sizeof(VM), cmpfuncVM);

  //for(i=0; i<1000; i++) printf("%d %f\n", vec[i].x, vec[i].med);

  cont=0;
  sum = 0;
  inI2 = 0;

  for(i=0; i<tamIt && vec[i].med>0 && cont < noIt; i++){
    inI2 = (int) bb2((int *)listaI[i].l, 0, listaI[i].t, U);
    if(inI2 < 0) continue;
    sum += vec[i].med * listaI[i].rat[inI2];
    cont++;
    //printf("sum %f %f %f %d\n", sum, vec[i].med, listaU[i].rat[inI2], inI2);
  }
  //printf("total: %f %d\n", sum, cont);
  if (cont == 0) return medUs[inU].med;
  if ((medUs[inU].med + sum/cont) > 10 ) return 10;
  free(vec);
  return  medUs[inU].med + sum/cont;
}


//-----------------------------------------------------------------------------------
float evaluacionSistemaItem(EvalR *Test, int n, float **M, VL *listaI, VM *medUs, int tamIt, int noIt, int tamUs){
  int i, cont;
  float sum;
  float *vec = (float *) malloc(sizeof(float)*n);

  sum = 0;
  cont = 0;
  for(i=0; i<n; i++){
    float a;
    int b=0;
    a = recomendacionItem(M, listaI, medUs, tamIt, Test[i].us, Test[i].it, noIt, tamUs);
    if(a != -1){
      vec[cont] = a;
      sum += (Test[i].rat - vec[cont])*(Test[i].rat - vec[cont]);
      cont++;
    } 
    else b++;
    
    //if(i % 10 == 0) printf("%4d: %5d %5d %2.6f %2.6f %2.6f\n", i, Test[i].us, Test[i].it, Test[i].rat, vec[i], sum);
  }
  free(vec);
  if(cont==0) return 0;
  else return sqrt(sum/cont);
}


//----------------------------------------------------------------------------
void crossValidationT1(Eval *vecLectura, int k, int tam){
  /*
    Esta función valida el método a utilizar.
      Los que hace es que encontrar la presición del método, a partir de 
        un conjunto de datos,
        k como el numero de diviciones,
        una metrica de entrenamiento y
        una función como métrica de evaluación,
      
  */

  //separación de grupos
  int i, j, l, a, b;
  EvalR *vec= (EvalR *) malloc(sizeof(EvalR)*tam); 
  EvalR **grupos = (EvalR **) malloc(sizeof(EvalR*)*k);
  int indices[k], tams[k];

  //genera vector auxiliar para desordenarlo
  for(i=0; i<tam; i++){
    vec[i] = vecLectura->REC[i];
    //printf("%d ", vec[i].us);
  } 
  
  //printf("\n");
  aleatoriza(vec, tam);
  //for(i=0; i<tam; i++) printf("%d ", vec[i].us);
  //printf("\n");

  //define los tamaños de los vectores, 
  //ejemplo: tam=13, k=5 ... tams= {3,3,3,2,2}, sum(tams[i])=13
  a = tam%k;
  b = tam/k;
  for(i=0; i<k; i++){
    if(a == 0) tams[i] = b;
    else{
      tams[i] = b+1;
      a--;
    }
    grupos[i] = (EvalR *) malloc(sizeof(EvalR)*(tams[i]));
    indices[i] = 0;
  }
  //for(i=0; i<k; i++) printf("%d\n", tams[i]);
  //printf("\n");

  //se llena cada grupo con valores equilibrados
  //ejemplo vec={1,2,3,4,5,6,7,8,9}, k = 4 ... g={ {1,5,9}, {2,6}, {3,7}, {4,8} }
  for(i=0; i<tam; i++){
    a = indices[i%k]++;
    grupos[i%k][a]= vec[i];
  }

  /*
  for(i=0; i<k; i++){
    for(j=0; j<tams[i]; j++){
      printf("%d ", grupos[i][j].us);
    }
    printf("\n");
  } */

  //inicio de las pruebas
  printf("inicio de las pruebas\n");
  float **RMSE = (float**) malloc(sizeof(float*)*k);
  int trainTam, testTam;
  EvalR *train, *test;
  for(i=0; i<k; i++){
    printf("\n\nPrueba %d:\n\n", i+1);
    //identificar conjunto test
    testTam = tams[i];
    test = (EvalR *) malloc(sizeof(EvalR)*(tams[i]));
    for(j=0; j<tams[i]; j++){
      test[j] = grupos[i][j];
    }

    //identificar conjunto train
    trainTam = 0;
    for(j=0; j<k; j++){
      if(i==j) continue;
      trainTam += tams[j];
    }
    train = (EvalR *) malloc(sizeof(EvalR)*trainTam);
    a=0;
    for(j=0; j<k; j++){
      if(i==j) continue;
      for(l=0; l<tams[j]; l++){
        train[a+l] = grupos[j][l];
      }
      a += tams[j];
    }

    /*
    //impresiones de conjuntos‘similitud_adjCos’:
    printf("test:\n");
    for(j=0; j<testTam; j++){
      printf("%d ", test[j].us);
    }
    printf("\n");
    printf("train:\n");
    for(j=0; j<trainTam; j++){
      printf("%d ", train[j].us);
    }
    printf("\n");*/

    float **mat;
    LISTS trainL;
    EvalR *testLimpio; //-------------------------

    //float *xx = (float *) malloc(sizeof(float)*6);
    mat = entrenamiento(train, trainTam, &trainL);
    testLimpio = limpiezaTest(test, &testTam, &trainL); //--------------------
    RMSE[i] = evaluacion(testLimpio, testTam, mat, &trainL);


    int vecinos[] = {10, 20, 30, 50, 100, 300, 1000};
    for(j=0; j<7; j++)
      printf("el resultado RMSE conjunto %d con %d vecinos es: %f\n", i+1, vecinos[j], RMSE[i][j]);

    for(j=0;j<trainL.tIt; j++){
      free(mat[j]);
    }
    free(mat);
    free(trainL.lIt);
    free(trainL.lUs);
    free(trainL.medUs);
    free(testLimpio);
    free(train);
    free(test);

    break;
    
  }

  printf("validación crusada para %d conjuntos\n", k);
  for(i=0;i<7;i++){
    int vecinos[] = {10, 20, 30, 50, 100, 300, 1000};
    float sum=0;
    for(j=0;j<k;j++){
      sum += RMSE[j][i]; //cada renglon contiene los 9 ejemplos de vecindarios
    }
    printf("para %d vecinos, RMSE= %f\n", vecinos[i], sum/k);
  }

  for(i=0;i<k; i++){
    free(grupos[i]);
    free(RMSE[i]);
  }
  free(RMSE);
  free(grupos);
  free(vec);
}

float **entrenamiento(EvalR *train, int trainTam, LISTS *trainL){
  int i, j;
  int tamUs, tamIt;
  float **mat;
  //cambio de tipo de dato (sin tiempo)
  EvalR *train2 = (EvalR *) malloc(sizeof(EvalR)*trainTam);
  for(i=0; i<trainTam; i++){
    train2[i].us = train[i].us;
    train2[i].it = train[i].it;
    train2[i].rat = train[i].rat;
    //printf("%d %d %f\n", train2[i].us, train2[i].it, train2[i].rat);
  }
  printf("cambiado el tipo de dato.\n");
  
  qsort(train2, trainTam, sizeof(EvalR), cmpfuncRatUs);
  //for(i=0; i<trainTam; i++) printf("%d %d %f\n", train2[i].us, train2[i].it, train2[i].rat);
  
  
  VM *Us;
  Us = mediaVec(train2, trainTam, &tamUs, USER);
  //for(i=0; i<tamUs; i++) printf("us: %d %f\n", Us[i].x, Us[i].med);
  //printf("%d\n", tamUs);
  
  //normalizar valores
  normalizacion(train2, trainTam, Us, tamUs);
  //for(i=0; i<50; i++) printf("%d %d %f\n", train2[i].us, train2[i].it, train2[i].rat);
  
  VL *listaUs = getLista(train2, trainTam, tamUs, USER);
  qsort(train2, trainTam, sizeof(EvalR), cmpfuncRatIt);
  VM *It = mediaVec(train2, trainTam, &tamIt, ITEM);
  VL *listaIt = getLista(train2, trainTam, tamIt, ITEM);
  
  //for(i=0;i<tamIt; i++) printf("%d\n", listaIt[i].x);
  trainL->lUs= listaUs;
  trainL->lIt= listaIt;
  trainL->tUs= tamUs;
  trainL->tIt= tamIt;
  trainL->medUs = Us;
  
  
  mat = (float**) malloc(sizeof(float*)*tamUs);
  for(i=0; i<tamUs; i++){
    mat[i] = (float *) malloc(sizeof(float)*tamUs);
    if(i % 100 == 0) printf("%d\n", i);
    for(j=0; j<tamUs; j++){
      mat[i][j] = similitud_p2(listaUs, tamUs, i, j, 50);
      //if(i==10) printf("%f\n",mat_ii[i][j]);
    }
  }
  free(train2);
  return mat;
}

EvalR *limpiezaTest(EvalR *test, int *testTam, LISTS *trainL){
  int i, j, inI, inU;

  //eliminación de datos del test que no se encuentran en el conjunto de train
  EvalR *test2 = (EvalR*) malloc(sizeof(EvalR)*(*testTam));
  j=0;
  for(i=0; i<(*testTam); i++){
    inU = bb1(trainL->lUs, 0, trainL->tUs, test[i].us);
    inI = bb1(trainL->lIt, 0, trainL->tIt, test[i].it);
    if(inI!=-1 && inU!=-1){
      test2[j] = test[i];
      j++;
    }
    //printf("%d", test2[j].us);
  }
  
  test2 = (EvalR *) realloc(test2, sizeof(Eval)*j);
  //for(i=0;i<j;i++) printf("%d %d\n", test2[i].us, test2[i].it);
  printf("Había %d elementos, quedan %d\n", *testTam, j);
  *testTam = j;
  return test2;
}

float *evaluacion(EvalR *test, int testTam, float **mat, LISTS *trainL){
  int i;
  float *xx = (float*) malloc(sizeof(float)*9);

  int k[] = {10, 20, 30, 50, 100, 300, 1000};
  for(i=0; i<7; i++){
    //xx[i] = evaluacionSistemaItem(test, testTam, mat, trainL->lIt, trainL->medUs, trainL->tIt, k[i], trainL->tUs);
    xx[i] = evaluacionSistema(test, testTam, mat, trainL->lUs, trainL->medUs, trainL->tUs, k[i]);
    //xx[i] = 0;
  }
  //for(i=0; i<6; i++)
  //  printf("El valor RMSE de %d es: %f\n", k[i], xx[i]);

  return xx;
}
