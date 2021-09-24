

//#include <cstdlib>
#include "lecturaRS.h"

// #define DEBUG

#define UIR 3
#define UIRT 4
#define UIT 5

typedef struct uir {
  int us, it;
  char rat;
} Uir;

typedef struct uirt {
  int us, it, time;
  char rat;
} Uirt;

typedef struct uit {
  int us, it, time;
} Uit;

int cmpfunElem(const void *a, const void *b) {
  // Esta función compara dos elementos recomendables de acuerdo
  // al posible nivel de interés del usuario
  Elemento *A = (Elemento *)a;
  Elemento *B = (Elemento *)b;
  return (B->val - A->val) * 1000000;
}


int cmpfuncStr(const void *a, const void *b) {
  // para cadenas
  char **a1 = (char **)a;
  char **a2 = (char **)b;
  return strcmp(*a1, *a2);
}


void printEvalR(Eval *vec0, int tamDatos) {
  EvalR *vec = vec0->REC; 
  for (int i = 0; i < tamDatos; i++) {
    printf("%6d %6d,%6d,%2.0f\n", i + 1, vec[i].us, vec[i].it, vec[i].rat);
  }
  printf("%d datos\n", tamDatos);
}

void freeStr(char **vec, int tam) {
  for (int i = 0; i < tam; i++) {
    free(vec[i]);
  }
  free(vec);
  return;
}

int **allocM_I(int a, int b){
  int **res = (int **)malloc(sizeof(int *) * a);

#ifdef DEBUG
  char s[] = "allocM_I_A";
  validaMemoria(res, s);
#endif

  for (int i = 0; i < a; i++) {
    res[i] = (int *)malloc(sizeof(int) * b);

#ifdef DEBUG
  validaMemoria(res[i], s);
#endif

  }
  return res;
}

float **allocM_F(int a, int b){
  float **res = (float **)malloc(sizeof(float *) * a);

#ifdef DEBUG
  char s[] = "allocM_F_A";
  validaMemoria(res, s);
#endif

  for (int i = 0; i < a; i++) {
    res[i] = (float *)malloc(sizeof(float) * b);

#ifdef DEBUG
  validaMemoria(res[i], s);
#endif

  }
  return res;
}

void freeM_I(int **M, int t) {
  for (int i = 0; i < t; i++) {
    free(M[i]);
  }
  free(M);
}

void freeM_F(float **M, int t) {
  for (int i = 0; i < t; i++) {
    free(M[i]);
  }
  free(M);
}

void freeM_Str(char **M, int t) {
  for (int i = 0; i < t; i++) {
    free(M[i]);
  }
  free(M);
}

void freeM_EvalR(EvalR **M, int t) {
  for (int i = 0; i < t; i++) {
    free(M[i]);
  }
  free(M);
}


void freeM_Elem(Elemento **M, int t){
  for (int i = 0; i < t; i++) {
    free(M[i]);
  }
  free(M);
}

void freeEval(Eval *vec) {
  if (vec->LI != NULL)
    freeStr(vec->LI, vec->tLI);
  if (vec->LU != NULL)
    freeStr(vec->LU, vec->tLU);
  free(vec->REC);
  free(vec);
}

EvalR *copyEvalR(EvalR *V, int t){
  EvalR *res = (EvalR *) malloc(sizeof(EvalR)*t);
  
#ifdef DEBUG
  char s[] = "copyEvalR";
  validaMemoria(res, s);
#endif

  for(int i=0; i<t; i++){
    res[i].us = V[i].us;
    res[i].it = V[i].it;
    res[i].rat = V[i].rat;
  }
  return res;
}


void contadores(Eval *datos) {
  /*Esta función se encarga de contar los items, no necesita ningún ordenamiento
  ya que los datos ya estan dispuestos de forma que ui = [1:n], i_j = [1:m] */
  int maxU = 0, maxI = 0;
  for (int i = 0; i < datos->tREC; i++) {
    if (datos->REC[i].us > maxU)
      maxU = datos->REC[i].us;
    if (datos->REC[i].it > maxI)
      maxI = datos->REC[i].it;
  }
  datos->numU = maxU;
  datos->numI = maxI;
}


void validaMemoria(void *x, char *s){
  // printf("p: %d\n", x);
  if(x == NULL){
    printf("Error puntero nulo en %s\n", s);
    exit(1);
  }
}

// Matrix *constructorMatrix_F(int a, int b, int alloc){
//   Matrix *res = (Matrix*)malloc(sizeof(Matrix));
//   res->w = a;
//   res->h = b;
//   if(alloc == 1)
//     res->d = (float *) malloc(a * b * sizeof(float));
//   else
//     res->d = NULL;
//   return res;
// }

/////PSQL
//-----------------------------------------------------
//--------------psql----------------------------------
Eval *DB_to_eval(char *tabla, int registros, int atributos, int k, int parte);

//generación del titulo de la tabla (ya contenida en PSQL)
char *getNombreTabla(int tipo){
  char *table = (char*) malloc(sizeof(char)*30);
  if (tipo == 1)
    strcpy(table, "ml100k");
  else if (tipo == 2)
    strcpy(table, "ml1m_index");
  else if (tipo == 3)
    strcpy(table, "ml10m_index");
  else if (tipo == 4)
    strcpy(table, "ml25m_index");
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
  else if (tipo == 11)
    strcpy(table, "ml100k_inv");
  else if (tipo == 12)
    strcpy(table, "ml1m_inv");
  else if (tipo == 13)
    strcpy(table, "ml10m_inv");
  else if (tipo == 14)
    strcpy(table, "ml25m_inv");
  else if (tipo == 15)
    strcpy(table, "bookcrossing_inv");
  else if (tipo == 16)
    strcpy(table, "anime_inv");
  else if (tipo == 17)
    strcpy(table, "netflix_inv");
  else if (tipo == 18)
    strcpy(table, "good_netflix_inv");
  else if (tipo == 19)
    strcpy(table, "amazon_inv");
  else if (tipo == 20)
    strcpy(table, "good_amazon_inv");
  else {
    printf("conjunto invalido\n");
    exit(1);
  }
  return table;
}

//Funciones de control de exepciones
void DB_exit(PGconn *conn) {
  PQfinish(conn);
  exit(EXIT_FAILURE);
}

void checkPGconn(PGconn *conn){
  if (PQstatus(conn) == CONNECTION_BAD) {
    fprintf(stderr, "Falló coneción a la base de datos: %s\n", PQerrorMessage(conn));
    DB_exit(conn);
  }
}

void checkPGresult(PGconn *conn, PGresult *res){
  if (PQresultStatus(res) != PGRES_TUPLES_OK) {
    printf("No se encontró la tabla. Saliendo.\n");
    PQclear(res);
    DB_exit(conn);
  }
}

//generción de matriz de flotantes en formato SCR
void generarSCR(PGconn *conn, PGresult *querty, int tipo, int m1, Elemento **vec, int **ind, int *tam){

  //se valida consulta
  checkPGresult(conn, querty);
  if(tipo != USER && tipo != ITEM){
    printf("Error en generarSCR\n");
    exit(EXIT_FAILURE);
  }

  int i, j, u;

  //memoria para vec
  vec[0] = (Elemento *) malloc(sizeof(Elemento)*PQntuples(querty));
#ifdef DEBUG
  char s[] = "SCR";
  validaMemoria(vec[0], s);
#endif

  //se define el número de filas
  // printf("m%d,", m1);
  if(m1 == 0){
    m1 = atoi(PQgetvalue(querty, PQntuples(querty)-1, tipo));
    *tam = m1;
    // printf("m%d,", m1);
  }
  //memoria para ind
  ind[0] = (int *) malloc(sizeof(int)*(1+m1));
#ifdef DEBUG
  validaMemoria(ind[0], s);
#endif
  
  //colocar como vacias las posiciones iniciales 
  //es posible tener los primeros x elementos vacios
  //todos se inicializan en la pocisión 0 de la matriz
  for (i = 0; i <= atoi(PQgetvalue(querty, 0, tipo))-1; i++) {
    // printf("%d_", i);
    ind[0][i] = 0;
  }
  j = i-1;

  //se inicia el primer elemento
  vec[0][0].ind = atoi(PQgetvalue(querty, 0, 1-tipo)) - 1;
  vec[0][0].val = atof(PQgetvalue(querty, 0, 2));
  
  //se itera en la matriz dispersa
  for (i = 1; i < PQntuples(querty); i++) {
    u = atoi(PQgetvalue(querty, i, tipo))-1;
    vec[0][i].ind = atoi(PQgetvalue(querty, i, 1-tipo)) - 1;
    vec[0][i].val = atof(PQgetvalue(querty, i, 2));

    if (j == u) continue;
    for(int l=j+1; l <= u; l++){
      if(l >= m1) printf("%d.%d..",l,u);
      ind[0][l] = i;
    }
    j = u;
  }
  for(int l=j; l <= m1; l++){
      // printf("%d_", l);
      ind[0][l] = PQntuples(querty);
  }

  // printf("%d,%d,", PQntuples(querty),u+1);

#ifdef ALLOW_PRINTS
  printf("Datos SCR: %d\n", PQntuples(querty));
#endif

  PQclear(querty);
  return;
}

Eval *leerDB(int tipo) {
  char *table = getNombreTabla(tipo);
  Eval *res = DB_to_eval(table, 0, ALL_PARTS, 0, 0);
  free(table);
  return res;
}


Eval *leerCrossSet(int tipo, int k, int parte) {
  char *table = getNombreTabla(tipo);
  Eval *res = DB_to_eval(table, 0, PARTIAL, k, parte);
  free(table);
  return res;
}

//generación de tabla de tipo EVAL (info (user, item, rating), tamInfo, tamUsers, tamItems, etc.)
Eval *DB_to_eval(char *tabla, int registros, int atributos, int k, int parte) {

  //revisa coneccion
  PGconn *conn = PQconnectdb(DATOS_CONECCION);
  checkPGconn(conn);

  PGresult *res = PQexec(conn, "DELETE FROM tempa");
  PQclear(res);

  //genera cadena
  char querty[300];
  if(atributos == ALL_PARTS)
    sprintf(querty, "INSERT INTO tempa (SELECT user_id, item_id, rating FROM %s)", tabla);
  else if(atributos == PARTIAL)
    sprintf(querty, "INSERT INTO tempa (SELECT * FROM (SELECT user_id, item_id, rating FROM %s WHERE (rand <= %.2f OR rand > %.2f) AND rating > 0) A WHERE user_id IN (SELECT user_id FROM %s GROUP BY user_id HAVING count(*) > 2))", tabla, (float) (k-1-parte)/k, (float) (k-parte)/k, tabla);
  res = PQexec(conn, querty);
  PQclear(res);

  res = PQexec(conn, "select * from tempa order by user_id, item_id");
  checkPGresult(conn, res);

  Eval *Nod = (Eval *) malloc(sizeof(Eval));
#ifdef DEBUG
  char s[] = "DB_to_eval_A";
  validaMemoria(Nod, s);
#endif
  
  Nod->REC = (EvalR *) malloc(PQntuples(res) * sizeof(EvalR));
#ifdef DEBUG
  validaMemoria(Nod->REC, s);
#endif

  for (int i = 0; i < PQntuples(res); i++) {
    Nod->REC[i].us = atoi(PQgetvalue(res, i, 0)) - 1;
    Nod->REC[i].it = atoi(PQgetvalue(res, i, 1)) - 1;
    Nod->REC[i].rat = atof(PQgetvalue(res, i, 2));
#ifdef DEBUG
    validaMemoria(Nod->REC, s);
#endif
  }

  Nod->tREC = PQntuples(res);

#ifdef ALLOW_PRINTS
  printf("Datos EvalR: %d\n", PQntuples(res));
#endif
  PQclear(res);


  //Establece el número de usuarios así como el número de ítems
  res = PQexec(conn, "select max(user_id), max(item_id) from tempa");
  checkPGresult(conn, res);
  Nod->numU = atoi(PQgetvalue(res, 0, 0));
  Nod->numI = atoi(PQgetvalue(res, 0, 1));
  PQclear(res);

  Nod->LU = NULL;
  Nod->LI = NULL;

  PQfinish(conn);

  return Nod;
}


//recopilación de información para ItemKNN
void getCrossSets(int dataset, int k, int parte, Eval **setData,  
  Elemento **test, int **indTest){
  char *tabla = getNombreTabla(dataset);
  
  setData[0] = DB_to_eval(tabla, 0, PARTIAL, k, parte);
  
  PGconn *conn = PQconnectdb(DATOS_CONECCION);
  checkPGconn(conn);
  
  char querty[300];
  int tTest;
  
  PGresult *res = PQexec(conn, "DELETE FROM test");
  PQclear(res);
  
  sprintf(querty, "INSERT INTO test (SELECT * FROM (SELECT user_id, item_id, rating FROM %s WHERE rand > %.3f AND rand <= %.3f AND rating = %d) A WHERE user_id IN (SELECT user_id FROM tempa GROUP BY user_id) AND item_id IN (SELECT item_id FROM tempa GROUP BY item_id) ORDER BY user_id, item_id)", tabla, (float) (k-1-parte)/k, (float) (k-parte)/k, 5);
  PQexec(conn, querty);

  res = PQexec(conn, "select * from test order by user_id, item_id");
  generarSCR(conn, res, USER, setData[0]->REC[setData[0]->tREC - 1].us + 1, test, indTest, &tTest);
  
#ifdef ALLOW_PRINTS
  printf("\n");
#endif

  PQfinish(conn);
  free(tabla);
  return;
}



//recopilación de información para ItemKNN_SP
void getCrossSets_SP(int dataset, int k, int parte, Elemento **vUs, int **indUs, int *tUs, 
  Elemento **vIt, int **indIt, int *tIt, Elemento **test, int **indTest){
  //Esta función retorna todos los elementos necesarios para la implementación del método:
  //itemKNN_SP y itemKNN_SP_GPU
  char *tabla = getNombreTabla(dataset);

  PGconn *conn = PQconnectdb(DATOS_CONECCION);
  checkPGconn(conn);

  PGresult *res = PQexec(conn, "DELETE FROM tempa");
  PQclear(res);

  res = PQexec(conn, "DELETE FROM tempb");
  PQclear(res);
  
  char querty[300];
  sprintf(querty, "INSERT INTO tempa (SELECT * FROM (SELECT user_id, item_id, rating FROM %s WHERE (rand <= %.3f OR rand > %.3f) AND rating > 0) A WHERE user_id IN (SELECT user_id FROM %s GROUP BY user_id HAVING count(*) > 2))", tabla, (float) (k-1-parte)/k, (float) (k-parte)/k, tabla);
  res = PQexec(conn, querty);
  PQclear(res);
  
  res = PQexec(conn, "select * from tempa order by user_id, item_id");
  generarSCR(conn, res, USER, 0, vUs, indUs, tUs);

  // sprintf(querty, "INSERT INTO tempb (SELECT * FROM (SELECT user_id, item_id, rating FROM %s WHERE (rand <= %.3f OR rand > %.3f) AND rating > 0) A WHERE user_id IN (SELECT user_id FROM %s GROUP BY user_id HAVING count(*) > 2))", tabla, (float) (k-1-parte)/k, (float) (k-parte)/k, tabla);
  // res = PQexec(conn, querty);
  // PQclear(res);

  res = PQexec(conn, "select * from tempa order by item_id, user_id");
  generarSCR(conn, res, ITEM, 0, vIt, indIt, tIt);

  int tTest;

  res = PQexec(conn, "DELETE FROM test");
  PQclear(res);

  int maxRat = 5;
  sprintf(querty, "INSERT INTO test (SELECT * FROM (SELECT user_id, item_id, rating FROM %s WHERE rand > %.3f AND rand <= %.3f AND rating = %d) A WHERE user_id IN (SELECT user_id FROM tempa GROUP BY user_id) AND item_id IN (SELECT item_id FROM tempa GROUP BY item_id))", tabla, (float) (k-1-parte)/k, (float) (k-parte)/k, maxRat);
  res = PQexec(conn, querty);
  PQclear(res);
  
  res = PQexec(conn, "select * from test order by user_id, item_id");  
  generarSCR(conn, res, USER, *tUs, test, indTest, &tTest);

#ifdef ALLOW_PRINTS
  printf("\n");
#endif

  PQfinish(conn);
  free(tabla);
  return;
}

//Obtención de vector de medias por medio de consulta en PSQL
void getMediaPsql(int *ind, int t, float **med, int tipo){
  //retorna la media de cada iten/ususario con una consulta
  PGconn *conn = PQconnectdb(DATOS_CONECCION);
  checkPGconn(conn);

  PGresult *res;
  // char querty[300];
  if(tipo == USER){
    res = PQexec(conn, "SELECT user_id, AVG(rating) FROM tempa GROUP BY user_id ORDER BY user_id");
  }
  else if(tipo == ITEM){
    res = PQexec(conn, "SELECT item_id, AVG(rating) FROM tempa GROUP BY item_id ORDER BY item_id");
  }
  checkPGresult(conn, res);

  med[0] = (float *) calloc(t, sizeof(float));
#ifdef DEBUG
  char s[] = "getMediaPsql";
  validaMemoria(med[0], s);
#endif
  
  int a;
  float b;
  for(int i=0; i< PQntuples(res); i++){
    a = atoi(PQgetvalue(res, i, 0)) - 1;
    b = atof(PQgetvalue(res, i, 1));
    med[0][a] = b;
  }

  PQclear(res);
  PQfinish(conn);
  return;
}



//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------Evaluación----------------------------------------
void evaluacion(Elemento *recomendaciones, int nU, int nRecomendaciones, Elemento *test, 
            int* indTest, float *PRECISION, float *RECALL, float *NDCG) {
  // esta función se encarga de:
  // 1.- Comparar para cada usuario:
  // Precision, Recall, NDCG

  int i, j, k;

  int cont, puntos;
  float sumaP, sumaR, sumaNDCG, pNDCG, pMAX;

  sumaP = 0;
  sumaR = 0;
  sumaNDCG = 0;
  cont = 0;
  for (i = 0; i < nU; i++) {
    if (indTest[i+1] - indTest[i] == 0)
      continue;

    //divisor de NDCG y contador para PRESICION y RECALL
    puntos = 0;
    pNDCG = 0;
    for (j = 0; j < nRecomendaciones; j++) {
      for (k = 0; k < indTest[i+1] - indTest[i]; k++) {
        if (recomendaciones[i * nRecomendaciones + j].ind == test[indTest[i] + k].ind) {
          puntos++;
          if (j >= 2)
            pNDCG += 1.0 / (log2(j + 1));
          else
            pNDCG++;
        }
        // printf("%d %d\n", restaultados.d[i* restaultados.w + j],
        // lista[i][k]);
      }
    }

    //divisor en NDCG
    if (indTest[i+1] - indTest[i] >= 2) {
      pMAX = 2;

      for (j = 0; j < indTest[i + 1] - indTest[i] - 2; j++) {
        pMAX += 1.0 / log2(j + 3);
      }
    } 
    else
      pMAX = indTest[i+1] - indTest[i];
    

    // printf("%d\n", (indTest[i+1] - indTest[i]));
    sumaR += (float)puntos / (indTest[i+1] - indTest[i]);
    sumaP += (float)puntos / nRecomendaciones;
    sumaNDCG += (float)pNDCG / pMAX;

    cont++;
  }

#ifdef ALLOW_PRINTS
  // printf("Precision= %f\n", sumaP/cont);
  // printf("Recall= %f\n", sumaR/cont);
  // printf("NDCG= %f\n", sumaNDCG/cont);
  printf("Usuarios aplicables= %d\n", cont);
  printf("Elementos evaluados= %d\n", indTest[nU]);
#endif

  *PRECISION += sumaP / cont;
  *RECALL += sumaR / cont;
  *NDCG += sumaNDCG / cont;

  return;
}


// PQflush
// PQfreemem
// PQconninfoFree