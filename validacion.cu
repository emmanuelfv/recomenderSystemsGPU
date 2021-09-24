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
  
  if(argc < 7){
    printf("{prueba} {aloritmo} {dataset} {nGrupos} {nVecinos} {nRecomendaciones}\n");
    return 0;
  }
  
  int prueba = atoi(argv[1]);
  int metodo = atoi(argv[2]);
  int dataset = atoi(argv[3]);
  int kGrupos = atoi(argv[4]);
  int nVecinos = atoi(argv[5]);
  int nRecomendaciones = atoi(argv[6]);
  int parte=0;
  int similitud=0;

  if(prueba == 3 && argc < 8){
    printf("{prueba} {algoritmo} {dataset} {nGrupos} {nVecinos} {nRecomendaciones} {parte}\n");
    printf("En caso de tener metodo = 22 o 23 agregar {similitud (1,2,3,4)}\n");
    return 1;
  }
  if((metodo == ITEMKNN_SP_GPU || metodo == ITEMKNN_SP_GPU2) && argc < 9){
    printf("En caso de tener metodo = 22 o 23 agregar {similitud (1,2,3,4)}\n");
    return 1;
  }
  parte = atoi(argv[7]);
  similitud = atoi(argv[8]);
  
  #ifdef ALLOW_PRINTS
  printf("%d datos\n", vec->tREC);
  #endif

  // evaluación de tiempos
  if(prueba == 1){
    if (metodo == ITEMKNN_ORIG || metodo == ITEMKNN_OPT) {
      Eval *vec = leerDB(dataset);
      tiempos_iknn(metodo, vec, nVecinos, nRecomendaciones);
      #ifdef ALLOW_PRINTS 
      printf("Proceso completado correctamente.\n");
      #endif
      freeEval(vec);
    }
    else{
      printf("Solo se permiten los algoritmos %d %d.\n", ITEMKNN_ORIG, ITEMKNN_OPT);
    } 
      
  } 
  else if (prueba == 2){
    if (parte > kGrupos) {
      printf("La parte seleccionada de la validación cruzada es mayor al número de partes disponibles.\n");
      return 1;
    }
    if ((metodo == ITEMKNN_SP_GPU || metodo == ITEMKNN_SP_GPU2) && (similitud > 4 || similitud < 1)) {
      printf("Similitud no valida.\n");
      return 1;
    }
    if (metodo == ITEMKNN_GPU || metodo == ITEMKNN_SP || metodo == ITEMKNN_SP_GPU || metodo == ITEMKNN_SP_GPU2 || metodo == POP) {
      tiempos_exactitud(metodo, dataset, kGrupos, nVecinos, nRecomendaciones, similitud);
    }
    else {
      printf("Solo se permiten los algoritmos %d %d %d %d %d.\n", ITEMKNN_GPU, ITEMKNN_SP, ITEMKNN_SP_GPU, ITEMKNN_SP_GPU2, POP);
    }
  } 
  else if(prueba == 3){
    if (parte > kGrupos) {
      printf("La parte seleccionada de la validación cruzada es mayor al número de partes disponibles.\n");
      return 1;
    }
    if ((metodo == ITEMKNN_SP_GPU || metodo == ITEMKNN_SP_GPU2) && (similitud > 4 || similitud < 1)) {
      printf("Similitud no valida.\n");
      return 1;
    }

    if (metodo == ITEMKNN_GPU || metodo == ITEMKNN_SP || metodo == ITEMKNN_SP_GPU || metodo == ITEMKNN_SP_GPU2 || metodo == POP) {
      tiempos_exactitud_parcial(metodo, dataset, kGrupos, nVecinos, nRecomendaciones, parte, similitud);
      return 0;
    }
    else {
      printf("Solo se permiten los algoritmos %d %d %d %d %d.\n", ITEMKNN_GPU, ITEMKNN_SP, ITEMKNN_SP_GPU, ITEMKNN_SP_GPU2, POP);
      return 1;
    }
  }
  else printf("prueba no valida {1, 2, 3}");
  return 1;
}


// tiempos ikOriginal-------------------------------------------
int tiempos_iknn(int metodo, Eval *vec, int k, int n) {
  if(metodo == ITEMKNN_ORIG){
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
  else if(metodo == ITEMKNN_OPT){

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
  else printf("solo se admiten metodos item KNN.");
  return 1;
}


void tiempos_exactitud(int metodo, int dataset, int kGrupos, int nVecinos, int nRecomendaciones, int similitud){

  Eval *setData;

  int i;
  Elemento *vUs, *vIt, *test;
  int *indTUs, *indTIt, *indTest, tUs, tIt;

  Elemento *Mat, *matVecinos;
  int *indSim;


  B_UI b;

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

      get_b(vUs, vIt, indTUs, indTIt, tUs, tIt, &b);

      //Construcción del modelo
      t_ini = clock();
      construccion_SP_GPU(vUs, vIt, indTUs, indTIt, tUs, tIt, medUs, medIt, &Mat, &indSim, similitud);
      t_fin = clock();
      secs = (double)(t_fin - t_ini) / CLOCKS_PER_SEC;
      resultados[i][0] = secs;
      
      
      //Aplicacion del modelo
      t_ini = clock();
      aplicacion_SP_GPU(Mat, indSim, vUs, indTUs, vIt, indTIt, tUs, tIt, b, nVecinos, nRecomendaciones, &recomendaciones);
      t_fin = clock();
      secs = (double)(t_fin - t_ini) / CLOCKS_PER_SEC;
      resultados[i][1] = secs;

      // printf("Elementos en M: %d de %d posibles\n", indSim[tIt], tIt*tIt);
      
      //evaluación
      evaluacion(recomendaciones, tUs, nRecomendaciones, test, indTest, &resultados[i][2], &resultados[i][3], &resultados[i][4]);
      
      //liveración de memoria
      free(b.bu);
      free(b.bi);
      
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
    }else if (metodo == ITEMKNN_SP_GPU2){
      getCrossSets_SP(dataset, kGrupos, i, &vUs, &indTUs, &tUs, &vIt, &indTIt, &tIt, &test, &indTest);
      
      float *medUs, *medIt;
      getMediaPsql(indTUs, tUs, &medUs, USER);
      getMediaPsql(indTIt, tIt, &medIt, ITEM);

      get_b(vUs, vIt, indTUs, indTIt, tUs, tIt, &b);


      // printf(" %d %d %f\n", tUs, tIt, b.media);
      //Construcción del modelo
      t_ini = clock();
      construccion_SP_GPU2(vUs, vIt, indTUs, indTIt, tUs, tIt, medUs, medIt, nVecinos, &matVecinos, similitud);
      t_fin = clock();
      secs = (double)(t_fin - t_ini) / CLOCKS_PER_SEC;
      resultados[i][0] = secs;
      
      //Aplicacion del modelo
      t_ini = clock();

      aplicacion_SP_GPU2(matVecinos, vUs, indTUs, tUs, tIt, b, nVecinos, nRecomendaciones, &recomendaciones);
      
      t_fin = clock();
      secs = (double)(t_fin - t_ini) / CLOCKS_PER_SEC;
      resultados[i][1] = secs;
      
      // evaluación
      evaluacion(recomendaciones, tUs, nRecomendaciones, test, indTest, &resultados[i][2], &resultados[i][3], &resultados[i][4]);
      
      //liveración de memoria
      free(b.bu);
      free(b.bi);
      
      free(recomendaciones);

      free(matVecinos);
      
      free(medUs);
      free(medIt);

      free(test);
      free(indTest);
      free(vIt);
      free(indTIt);
      free(vUs);
      free(indTUs);

    }else if (metodo == ITEMKNN_SP){
      getCrossSets_SP(dataset, kGrupos, i, &vUs, &indTUs, &tUs, &vIt, &indTIt, &tIt, &test, &indTest);
      
      float *medUs, *medIt;
      getMediaPsql(indTUs, tUs, &medUs, USER);
      getMediaPsql(indTIt, tIt, &medIt, ITEM);
      
      get_b(vUs, vIt, indTUs, indTIt, tUs, tIt, &b);
      
      
      //Construcción del modelo
      t_ini = clock();
      construccion_SP(vUs, vIt, indTUs, indTIt, tUs, tIt, medUs, medIt, &Mat, &indSim, similitud);
      t_fin = clock();
      secs = (double)(t_fin - t_ini) / CLOCKS_PER_SEC;
      resultados[i][0] = secs;
      
      //Aplicacion del modelo
      t_ini = clock();
      aplicacion_SP(Mat, indSim, vUs, indTUs, vIt, indTIt, tUs, tIt, b, nVecinos, nRecomendaciones, &recomendaciones);
      t_fin = clock();
      double secs2 = (double)(t_fin - t_ini) / CLOCKS_PER_SEC;
      resultados[i][1] = secs2;
      printf("%f, %d\n", secs + secs2, i);
      
      //evaluación
      evaluacion(recomendaciones, tUs, nRecomendaciones, test, indTest, &resultados[i][2], &resultados[i][3], &resultados[i][4]);
      
      //liveración de memoria
      free(b.bu);
      free(b.bi);
      
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

  // printf("%s ", getNombreTabla(dataset));

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




void tiempos_exactitud_parcial(int metodo, int dataset, int kGrupos, int nVecinos, int nRecomendaciones, int parte, int similitud){

  Eval *setData;

  int i;
  Elemento *vUs, *vIt, *test;
  int *indTUs, *indTIt, *indTest, tUs, tIt;

  Elemento *Mat, *matVecinos;
  int *indSim;

  B_UI b;

  Elemento *recomendaciones;

  clock_t t_ini, t_fin, tt_ini, tt_fin;

  //se presentan los resultados en <resultados>
  //el orden es: tiempoConstrucción, tiempoExplotación, precisión, recall, NDCG

  tt_ini = clock();
  float *resultados = (float *) malloc(sizeof(float)*6);

  for(int j=0; j<5; j++) resultados[j] = 0;

  if (metodo == ITEMKNN_GPU) {
    //lectura de dataset (partes)
    getCrossSets(dataset, kGrupos, parte, &setData, &test, &indTest);
    
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
    resultados[0] += (float)(t_fin - t_ini) / CLOCKS_PER_SEC;
    
    //explotación
    t_ini = clock();
    applyModelAllGPU(R, knn, rec);
    t_fin = clock();
    resultados[1] += (double)(t_fin - t_ini) / CLOCKS_PER_SEC;
    
    //evaluación  
    evaluacion(rec.d, tUs, nRecomendaciones, test, indTest, &resultados[2], &resultados[3], &resultados[4]);
  
    free(R.d);
    free(knn.d);
    free(rec.d);

    freeEval(setData);
    free(test);
    free(indTest);

  }else if (metodo == ITEMKNN_SP_GPU){
    getCrossSets_SP(dataset, kGrupos, parte, &vUs, &indTUs, &tUs, &vIt, &indTIt, &tIt, &test, &indTest);
    
    float *medUs, *medIt;
    getMediaPsql(indTUs, tUs, &medUs, USER);
    getMediaPsql(indTIt, tIt, &medIt, ITEM);

    get_b(vUs, vIt, indTUs, indTIt, tUs, tIt, &b);

    //Construcción del modelo
    t_ini = clock();
    construccion_SP_GPU(vUs, vIt, indTUs, indTIt, tUs, tIt, medUs, medIt, &Mat, &indSim, similitud);
    t_fin = clock();
    resultados[0] += (float)(t_fin - t_ini) / CLOCKS_PER_SEC;
  
    
    //Aplicacion del modelo
    t_ini = clock();
    aplicacion_SP_GPU(Mat, indSim, vUs, indTUs, vIt, indTIt, tUs, tIt, b, nVecinos, nRecomendaciones, &recomendaciones);
    t_fin = clock();
    resultados[1] += (double)(t_fin - t_ini) / CLOCKS_PER_SEC;
    
    // printf("Elementos en M: %d de %d posibles\n", indSim[tIt], tIt*tIt);
    
    //evaluación
    evaluacion(recomendaciones, tUs, nRecomendaciones, test, indTest, &resultados[2], &resultados[3], &resultados[4]);
    
    //liveración de memoria
    free(b.bu);
    free(b.bi);
    
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
  }else if (metodo == ITEMKNN_SP_GPU2){
    getCrossSets_SP(dataset, kGrupos, parte, &vUs, &indTUs, &tUs, &vIt, &indTIt, &tIt, &test, &indTest);
    
    float *medUs, *medIt;
    getMediaPsql(indTUs, tUs, &medUs, USER);
    getMediaPsql(indTIt, tIt, &medIt, ITEM);

    get_b(vUs, vIt, indTUs, indTIt, tUs, tIt, &b);


    // printf(" %d %d %f\n", tUs, tIt, b.media);
    //Construcción del modelo
    t_ini = clock();
    construccion_SP_GPU2(vUs, vIt, indTUs, indTIt, tUs, tIt, medUs, medIt, nVecinos, &matVecinos, similitud);
    t_fin = clock();
    resultados[0] += (float)(t_fin - t_ini) / CLOCKS_PER_SEC;

    //Aplicacion del modelo
    t_ini = clock();
    aplicacion_SP_GPU2(matVecinos, vUs, indTUs, tUs, tIt, b, nVecinos, nRecomendaciones, &recomendaciones);
    t_fin = clock();
    resultados[1] += (double)(t_fin - t_ini) / CLOCKS_PER_SEC;
    
    // evaluación
    evaluacion(recomendaciones, tUs, nRecomendaciones, test, indTest, &resultados[2], &resultados[3], &resultados[4]);
    
    //liveración de memoria
    free(b.bu);
    free(b.bi);
    
    free(recomendaciones);

    free(matVecinos);
    
    free(medUs);
    free(medIt);

    free(test);
    free(indTest);
    free(vIt);
    free(indTIt);
    free(vUs);
    free(indTUs);

  }else if (metodo == ITEMKNN_SP){
    getCrossSets_SP(dataset, kGrupos, parte, &vUs, &indTUs, &tUs, &vIt, &indTIt, &tIt, &test, &indTest);
    
    float *medUs, *medIt;
    getMediaPsql(indTUs, tUs, &medUs, USER);
    getMediaPsql(indTIt, tIt, &medIt, ITEM);
    
    get_b(vUs, vIt, indTUs, indTIt, tUs, tIt, &b);
    
    
    //Construcción del modelo
    t_ini = clock();
    construccion_SP(vUs, vIt, indTUs, indTIt, tUs, tIt, medUs, medIt, &Mat, &indSim, similitud);
    t_fin = clock();
    resultados[0] = (float)(t_fin - t_ini) / CLOCKS_PER_SEC;;
    
    //Aplicacion del modelo
    t_ini = clock();
    aplicacion_SP(Mat, indSim, vUs, indTUs, vIt, indTIt, tUs, tIt, b, nVecinos, nRecomendaciones, &recomendaciones);
    t_fin = clock();
    resultados[1] = (float) (t_fin - t_ini) / CLOCKS_PER_SEC;
    
    //evaluación
    evaluacion(recomendaciones, tUs, nRecomendaciones, test, indTest, &resultados[2], &resultados[3], &resultados[4]);
    
    //liveración de memoria
    free(b.bu);
    free(b.bi);
    
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



  }else if(metodo == POP){
    int *pops;
    int nU;

    getCrossSets_POP(dataset, kGrupos, parte, nRecomendaciones, &pops, &nU, &test, &indTest);

    simple_pop(pops, nU, nRecomendaciones, &recomendaciones);

    evaluacion(recomendaciones, nU, nRecomendaciones, test, indTest, &resultados[2], &resultados[3], &resultados[4]);
  }

  tt_fin = clock();
  resultados[5] =  (float)(tt_fin - tt_ini) / CLOCKS_PER_SEC;
    
  printf("%d,%d,%d,%d,", dataset, nVecinos, similitud, parte);
  printf("%f,", resultados[5]);
  for(i=0; i<5; i++){
    printf("%f,", resultados[i]);
  }
  printf("\n");

  free(resultados);

  return;
}
