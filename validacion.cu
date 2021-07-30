
//  En este código se contempla la validación crusada como principal
//  característica del código

#include "validacion.h"

//-------------------------------------------------------------
//-------------------------------------------------------------
//main---------------------------------------------------------
int main(int argc, char **argv) {

#ifdef ALLOW_PRINTS
  printf("//////////////////////////////////");
  printf("//////////////////////////////////");
  printf("////////////////////////////////\n");
#endif
  srand(time(NULL));
  
  
  if(argc < 6){
    printf("{metodo} {dataset} {nGrupos} {nVecinos} {nRecomendaciones}\n");
    return 0;
  }
  
  int metodo = atoi(argv[1]);
  int dataset = atoi(argv[2]);
  int kGrupos = atoi(argv[3]);
  int nVecinos = atoi(argv[4]);
  int nRecomendaciones = atoi(argv[5]);
  int similitud = 0;
  if(metodo == ITEMKNN_SP_GPU && argc < 7){
    printf("{metodo} {dataset} {nGrupos} {nVecinos} {nRecomendaciones}\n");
    printf("En caso de tener metodo = 22 agregar {similitud (1,2,3,4)}\n");
    return 0;
  }
  similitud = atoi(argv[6]);
  
  #ifdef ALLOW_PRINTS
  printf("%d datos\n", vec->tREC);
  #endif

  // evaluación de tiempos
  if (metodo == ITEMKNN_ORIG) {
    // evaluacion_tiempos_ikOriginal(vec, nVecinos, nRecomendaciones);
    // freeEval(vec);
    // #ifdef ALLOW_PRINTS
    // printf("Resultados correctos.\n");
    // #endif
  } else if (metodo == ITEMKNN_OPT) {
    // evaluacion_tiempos_ik(vec, nVecinos, nRecomendaciones);
    // freeEval(vec);
    // #ifdef ALLOW_PRINTS 
    // printf("Resultados correctos.\n");
    // #endif
  // } else if (0){ // (metodo == ITEMKNN_GPU){
  //   crossValidationT(metodo, vec, kGrupos, nVecinos, nRecomendaciones);
  //     // crossValidationT1(vec, kGrupos, nVecinos);
  //     freeEval(vec);

  // } else if (0){
  //   crossValidationT(metodo, vec, kGrupos, nVecinos, nRecomendaciones);
  //   // evaluacion_tiempos_ik_sp(vec, kGrupos, nVecinos);
  //   freeEval(vec);

  } else if (metodo == ITEMKNN_GPU || metodo == ITEMKNN_SP_GPU || metodo == POP) {
    evaluacion_tiempos_exactitud(metodo, dataset, kGrupos, nVecinos, nRecomendaciones, similitud);
    
  } else if (metodo == 100){
    // // return 0;
    // findB();
    // freeEval(vec);
//    popUso(dataset, nRecomendaciones)
  }

  // solo métodos ya paralelizados


  return 0;
}
//-------------------------------------------------------------
//-------------------------------------------------------------

double fEvalb(EvalR *vec, int tamVec, double *b, int tamB, 
  double mean, double l1, int nU){
  double res =0;
  for (int i =0 ; i< tamB; i++){
    res += b[i] * b[i];
  }
  res *= l1;


  for (int i =0 ; i< tamVec; i++){
    res += vec[i].rat - mean - b[vec[i].us] - b[vec[i].it + nU];
  }
  return res;
}

void findB(Eval *vec){
  int *ratings = (int*) malloc(sizeof(int) * vec->tREC);
  for(int i = 0; i< vec->tREC; i++)
    ratings[i] = (int) vec->REC[i].rat;
  
  // printf("%d\n", (int) vec->REC[i].rat);

  double mean = gsl_stats_float_mean( (float*) &vec->REC->rat, 4, vec->tREC);
  // printf("mean: %f\n", mean);

  contadores(vec);
  int tamB = vec->numU + vec->numI;
  double *b = (double *) malloc(sizeof(double)*(tamB));
  for(int i = 0; i< tamB; i++)
    b[i] = (double) (rand() % 9) - 4;

  // for(int i = 0; i< 10; i++)
  //   printf("%f\n", b[i]);

  printf("mean: %f\n", fEvalb(vec->REC, vec->tREC, b, tamB, mean, 0.015, vec->numU));

  return; 
}

//tiempos
void evaluacion_tiempos_exactitud(int metodo, int dataset, int kGrupos, int nVecinos, int nRecomendaciones, int similitud){

  Eval *setData;

  int i;
  Elemento *vUs, *vIt, *test;
  int *indTUs, *indTIt, *indTest, tUs, tIt;

  Elemento *Mat;
  int *indSim;

  // B_UI b;

  Elemento *recomendaciones;

  clock_t t_ini, t_fin;
  double secs;

  //se presentan los resultados en <resultados>
  //el orden es: tiempoConstrucción, tiempoExplotación, precisión, recall, NDCG
  float **resultados = allocM_F(kGrupos, 5);

  for(i =0; i<kGrupos; i++){

#ifdef ALLOW_PRINTS
  printf("\nvalidación: %d\n", i);
#endif

    for(int j=0; j<5; j++) resultados[i][j] = 0;

    if (metodo == ITEMKNN_GPU) {
      //lectura de dataset (partes)
      getCrossSets(dataset, kGrupos, i, &setData, &test, &indTest);
      
      #ifdef ALLOW_PRINTS
      printf("Datos: %d\n", setData->tREC);
      #endif
      
      Matrix R;
      MatrixElemento knn;
      MatrixElemento rec;
      
      //matriz de calificaciones
      R.w = tUs = setData->numU;
      R.h = tIt = setData->numI;
      // R.d = (float *) malloc(8);
      R.d = generaMatrizDatos(setData->REC, setData->tREC, tUs, tIt);
      
      //matriz de vecinos
      knn.w = nVecinos;
      knn.h = R.h;
      knn.d = (Elemento *) malloc(knn.w * knn.h * sizeof(Elemento));
      // knn.d = (Elemento *) malloc(sizeof(Elemento));
      
      //matriz de resultados
      rec.w = nRecomendaciones;
      rec.h = tUs;
      rec.d = (Elemento *) malloc(rec.w * rec.h * sizeof(Elemento));
      // rec.d = (Elemento *) malloc(sizeof(Elemento));
      
      //construcción
      t_ini = clock();  
      construccionModelo(R, knn);
      t_fin = clock();
      secs = (double)(t_fin - t_ini) / CLOCKS_PER_SEC;
      #ifdef ALLOW_PRINTS
      printf("%.16g,", secs);
      #endif
      resultados[i][0] += secs;
      
      //explotación
      t_ini = clock();
      applyModelAllGPU(R, knn, rec);
      t_fin = clock();
      secs = (double)(t_fin - t_ini) / CLOCKS_PER_SEC;
      #ifdef ALLOW_PRINTS
      printf("%.16g\n", secs);
      #endif
      resultados[i][1] += secs;
      
      //evaluación  
      evaluacion(rec.d, tUs, nRecomendaciones, test, indTest, &resultados[i][2], &resultados[i][3], &resultados[i][4]);
    
      free(R.d);
      free(knn.d);
      free(rec.d);

      freeEval(setData);
      free(test);
      free(indTest);

    }else if (metodo == ITEMKNN_SP_GPU){
      getCrossSets_SP(dataset, kGrupos, i, &vUs, &indTUs, &tUs, &vIt, &indTIt, &tIt, &test, &indTest);
      
      float *medUs, *medIt;
      getMediaPsql(indTUs, tUs, &medUs, USER);
      getMediaPsql(indTIt, tIt, &medIt, ITEM);
      
//       for(int q=0; q<tIt; q++)
//       printf("%d %f\n", q, medIt[q]);

// return;
      //Construcción del modelo
      t_ini = clock();
      construccion_SP_GPU(vUs, vIt, indTUs, indTIt, tUs, tIt, medUs, medIt, &Mat, &indSim, similitud);
      t_fin = clock();
      secs = (double)(t_fin - t_ini) / CLOCKS_PER_SEC;
      resultados[i][0] = secs;

      // for(int k=0; k<tIt; k++){
      //   printf("%d %d\n", k, indSim[k]);
      // }
      // printf("\n\n");
      // for(int k=0; k<3; k++){
      //   for(int l=indSim[100+k]; l< indSim[100+k+1]; l++){
      //     printf("%d %f\n", Mat[l].ind, Mat[l].val);
      //   }
      // }
      
      //Aplicacion del modelo
      t_ini = clock();
      aplicacion_SP_GPU(Mat, indSim, vUs, indTUs, vIt, indTIt, tUs, tIt, medIt, nVecinos, nRecomendaciones, &recomendaciones);
      t_fin = clock();
      secs = (double)(t_fin - t_ini) / CLOCKS_PER_SEC;
      resultados[i][1] = secs;

      //evaluación
      evaluacion(recomendaciones, tUs, nRecomendaciones, test, indTest, &resultados[i][2], &resultados[i][3], &resultados[i][4]);

      // free(b.bu);
      // free(b.bi);
      
      free(recomendaciones);
      
      free(Mat);
      free(indSim);
      
      free(medUs);
      free(medIt);

      free(test);
      free(indTest);
      free(vIt);
      free(indTIt);
      free(vUs);
      free(indTUs);
      // return;
      // continue;
    }else if(metodo == POP){
      int *pops;
      int nU;

      getCrossSets_POP(dataset, kGrupos, i, nRecomendaciones, &pops, &nU, &test, &indTest);

      simple_pop(pops, nU, nRecomendaciones, &recomendaciones);

      evaluacion(recomendaciones, nU, nRecomendaciones, test, indTest, &resultados[i][2], &resultados[i][3], &resultados[i][4]);
    }
    
#ifdef ALLOW_PRINTS
    printf("resultados%d: ", kGrupos);
    for(int j=0; j<5; j++)
      printf("%f,", resultados[i][j]);
    printf("\n");
#endif

  }

  printf("%s ", getNombreTabla(dataset));

  float *s = (float *) malloc(sizeof(float)*5);
  for(int r=0; r<5; r++){
    s[r] = 0;
    for(i=0; i<kGrupos; i++){
      s[r] += resultados[i][r]/kGrupos;
    }
  }
  printf("%d,%d,%d,", dataset, nVecinos, similitud);
  for(i=0; i<5; i++){
    printf("%f,", s[i]);
  }
  printf("\n");

  free(s);
  freeM_F(resultados, 5);

  return;
}




// tiempos ikOriginal-------------------------------------------
int evaluacion_tiempos_ikOriginal(Eval *vec, int k, int n) {
  float **R, **S, **U;
  int **rec;
  contadores(vec);

  R = generaMatrizR(vec);

  clock_t t_ini, t_fin;
  double secs;

  t_ini = clock();
  S = construirModelo_ikOriginal(R, k, vec->numU, vec->numI);
  t_fin = clock();
  secs = (double)(t_fin - t_ini) / CLOCKS_PER_SEC;
  printf("%.16g,", secs);

  t_ini = clock();
  U = allocM_F(vec->numU, vec->numI);
  rec = (int **)malloc(sizeof(int *) * vec->numU);
  for (int i = 0; i < vec->numU; i++) {
    for (int j = 0; j < vec->numI; j++)
      U[i][j] = R[j][i];

    rec[i] = aplicarModelo_ikOriginal(S, vec->numI, U[i], n);
  }
  t_fin = clock();
  secs = (double)(t_fin - t_ini) / CLOCKS_PER_SEC;
  printf("%.16g\n", secs);

#ifdef ALLOW_PRINTS
  printf("itemKNN_m^2, resultados para usuarios 2, 5, 10\n");
  for (int i = 0; i < n; i++) {
    printf("%d %d %d\n", rec[2][i], rec[5][i], rec[10][i]);
  }
#endif

  freeM_F(R, vec->numI);
  freeM_F(S, vec->numI);
  freeM_F(U, vec->numU);
  freeM_I(rec, vec->numU);

  return 0;
}

// tiempos ik---------------------------------------------------
int evaluacion_tiempos_ik(Eval *vec, int k, int n) {
  clock_t t_ini, t_fin;

  // cuenta numero de ítems y de usuarios
  contadores(vec);
  int nI = vec->numI;
  int nU = vec->numU;

  // genera una matriz que contiene todas las evaluaciones de los usuarios
  float **R = generaMatrizR(vec);
  int **S_ind = allocM_I(nI, k);
  float **S_val = allocM_F(nI, k);

  // Construcción del modelo
  t_ini = clock();
  construirModelo_ik(S_ind, S_val, R, k, nU, nI);
  t_fin = clock();
  double secs = (double)(t_fin - t_ini) / CLOCKS_PER_SEC;
  printf("%.16g,", secs);

  // Explotación del modelo
  t_ini = clock();
  float **U = allocM_F(nU, nI);
  int **rec = (int **)malloc(sizeof(int *) * nU);
  for (int i = 0; i < nU; i++) {
    for (int j = 0; j < nI; j++) {
      U[i][j] = R[j][i];
    }
    rec[i] = aplicarModelo_ik(S_ind, S_val, nI, k, U[i], n);
  }
  t_fin = clock();
  secs = (double)(t_fin - t_ini) / CLOCKS_PER_SEC;
  printf("%.16g,", secs);
// printf("%.16g\n", (double)(t_fin - t_ini) / CLOCKS_PER_SEC);

// verificación de recomendaciones
#ifdef ALLOW_PRINTS
  printf("itemKNN_m*K, resultados para usuarios 2, 5, 10\n");
  for (int i = 0; i < n; i++) {
    printf("%d %d %d\n", rec[2][i], rec[5][i], rec[10][i]);
  }
#endif

  // Limpieza de la memoria
  freeM_F(R, nI);
  freeM_I(S_ind, nI);
  freeM_F(S_val, nI);
  freeM_F(U, nU);
  freeM_I(rec, nU);

  return 0;
}

//tiempos ik_sp
int evaluacion_tiempos_ik_sp(Eval *vec, int kGrupos, int nVecinos){

  void crossValidationT1(Eval *vec, int kGrupos, int nVecinos);

  return 0;
}
