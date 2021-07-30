//-----------------------------------------------------
//--------------psql----------------------------------

#ifndef _LECTURA_PSQL_H
#define _LECTURA_PSQL_H
#endif

#ifndef _LECTURA_RS_H
#include "lecturaRS.h"
#endif

#ifndef _STDIO_H
#include <stdio.h>
#endif

#ifndef _STDLIB_H
#include <stdlib.h>
#endif

#ifndef _STRING_H
#include <string.h>
#endif

#ifndef _TIME_H
#include <time.h>
#endif

#ifndef LIBPQ_FE_H
#include <postgresql/libpq-fe.h>
#endif

#include "lecturaPSQL.h"


int foo(int a, int b)
{
    return a+b;
}

void DB_exit(PGconn *conn) {
  PQfinish(conn);
  exit(1);
}

Eval *DB_to_eval(char *tabla, int registros, int atributos) {

  PGconn *conn = PQconnectdb("user=emmanuel dbname=recomendaciones");

  if (PQstatus(conn) == CONNECTION_BAD) {
    fprintf(stderr, "Falló coneción a la base de datos: %s\n",
            PQerrorMessage(conn));
    DB_exit(conn);
  }

  char querty[100];
  sprintf(querty, "SELECT user_id, item_id, rating FROM %s", tabla);
  PGresult *res = PQexec(conn, querty);

  if (PQresultStatus(res) != PGRES_TUPLES_OK) {
    printf("No se encontró la tabla. Saliendo.\n");
    PQclear(res);
    DB_exit(conn);
  }

  int i; //, j, nFields;

  Eval *Nod = (Eval *)malloc(sizeof(Eval));
  Nod->REC = (EvalR *)malloc(sizeof(EvalR));
  Nod->LU = NULL;
  Nod->LI = NULL;

  // nFields = PQnfields(res);

  for (i = 0; i < PQntuples(res); i++) {
    Nod->REC[i].us = atoi(PQgetvalue(res, i, 0))-1;
    Nod->REC[i].it = atoi(PQgetvalue(res, i, 1))-1;
    Nod->REC[i].rat = atof(PQgetvalue(res, i, 2));
    // Nod->REC[i].time = atoi(PQgetvalue(res, i, 3));
    Nod->REC = (EvalR *)realloc(Nod->REC, (i + 2) * sizeof(EvalR));
  }

  Nod->tREC = i;

  PQclear(res);
  PQfinish(conn);

  return Nod;
}

Eval *leerDB(int tipo) {
  char table[20];
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
  return DB_to_eval(table, 0, PSQL_UIR);
}

//---------------------------------------------


void getCrossSets(int dataset, int k, int parte, Elemento **vUs, int **indUs, int *tUs, 
  Elemento **vIt, int **indIt, int *tIt, EvalR **test, int *tTest){
  //Esta función retorna todos los elementos necesarios para la implementación del método:
  //itemKNN_SP y itemKNN_SP_GPU

  char tabla[20];
  char tabla2[30];
  if (dataset == 1)
    strcpy(tabla, "ml100k");
  else if (dataset == 2)
    strcpy(tabla, "ml1m_index");
  else if (dataset == 3)
    strcpy(tabla, "ml10m_index");
  else if (dataset == 4)
    strcpy(tabla, "ml25m_index");
  else if (dataset == 5)
    strcpy(tabla, "bookcrossing_index");
  else if (dataset == 6)
    strcpy(tabla, "anime_index");
  else if (dataset == 7)
    strcpy(tabla, "netflix");
  else if (dataset == 8)
    strcpy(tabla, "good_netflix_index");
  else if (dataset == 9)
    strcpy(tabla, "amazon");
  else if (dataset == 10)
    strcpy(tabla, "good_amazon_index");
  else {
    printf("conjunto invalido\n");
    exit(1);
  }
  strcpy(tabla2, tabla);
  strcat(tabla2, "_inv");

  // printf("%s, %s\n", tabla, tabla2);

  PGconn *conn = PQconnectdb("user=emmanuel dbname=recomendaciones");

  if (PQstatus(conn) == CONNECTION_BAD) {
    fprintf(stderr, "Falló coneción a la base de datos: %s\n",
            PQerrorMessage(conn));
    DB_exit(conn);
  }

  PQexec(conn, "DELETE FROM tempa");
  PQexec(conn, "DELETE FROM tempb");
  PQexec(conn, "DELETE FROM test");
  
  char querty[300];
  sprintf(querty, "INSERT INTO tempa (SELECT * FROM (SELECT user_id, item_id, rating FROM %s WHERE (rand <= %.2f OR rand > %.2f) AND rating > 0) A WHERE user_id IN (SELECT user_id FROM %s GROUP BY user_id HAVING count(*) > 2) ORDER BY user_id, item_id)", tabla, (float) (k-1-parte)/k, (float) (k-parte)/k, tabla);
  PQexec(conn, querty);
  PGresult *res = PQexec(conn, "select * from tempa");

  if (PQresultStatus(res) != PGRES_TUPLES_OK) {
    printf("No se encontró la tabla. Saliendo.\n");
    PQclear(res);
    DB_exit(conn);
  }
  int i, j, u, m1, m2, maxRat;

  maxRat = 1;
  vUs[0] = (Elemento *) malloc(sizeof(Elemento)*PQntuples(res));
  vIt[0] = (Elemento *) malloc(sizeof(Elemento)*PQntuples(res));
  
  vUs[0][0].ind = atoi(PQgetvalue(res, 0, 1)) - 1;
  vUs[0][0].val = atof(PQgetvalue(res, 0, 2));

  m1 = atoi(PQgetvalue(res, PQntuples(res)-1, 0));
  // printf("%d\n", m1);
  
  indUs[0] = (int *) malloc(sizeof(int)*(1+m1));
  
  //ndIt[0] = (int *) malloc(sizeof(int)*m1);
  // printf("%d", atoi(PQgetvalue(res, 0, 0))-1);
  for (i = 0; i <= atoi(PQgetvalue(res, 0, 0))-1; i++) {
    indUs[0][i] = 0;
  }
  // printf("%d\n", atoi(PQgetvalue(res, 0, 0))-1);
  // printf("%d\n", indUs[0][0]);
  
  j = i-1;
  for (i = 1; i < PQntuples(res); i++) {
    u = atoi(PQgetvalue(res, i, 0))-1;
    vUs[0][i].ind = atoi(PQgetvalue(res, i, 1)) - 1;
    vUs[0][i].val = atof(PQgetvalue(res, i, 2));
    if(vUs[0][i].val > maxRat) maxRat = vUs[0][i].val;

    // printf("%d %d %.1f\n", atoi(PQgetvalue(res, i, 0)), atoi(PQgetvalue(res, i, 1)), atof(PQgetvalue(res, i, 2)));
  
    if (j == u) continue;
    if(j + 1 < u){
      for(int l=j+1; l <= u; l++){
        indUs[0][l] = i;
        // printf("%d.......\n", u);
        
      }
    }
    else{
        indUs[0][u] = i;
    }
    j = u;
  }
  indUs[0][u+1] = PQntuples(res);
  *tUs = m1;

  PQclear(res);



  sprintf(querty, "INSERT INTO tempb (SELECT * FROM (SELECT user_id, item_id, rating FROM %s WHERE (rand <= %.3f OR rand > %.3f) AND rating > 0) A WHERE user_id IN (SELECT user_id FROM %s GROUP BY user_id HAVING count(*) > 2) ORDER BY item_id, user_id)", tabla2, (float) (k-1-parte)/k, (float) (k-parte)/k, tabla2);
  PQexec(conn, querty);
  res = PQexec(conn, "select * from tempb");

  if (PQresultStatus(res) != PGRES_TUPLES_OK) {
    printf("No se encontró la tabla. Saliendo.\n");
    PQclear(res);
    DB_exit(conn);
  }
  
  m2 = atoi(PQgetvalue(res, PQntuples(res)-1, 1));
  indIt[0] = (int *) malloc(sizeof(int)*(1+m2));
  vIt[0][0].ind = atoi(PQgetvalue(res, 0, 0)) - 1;
  vIt[0][0].val = atof(PQgetvalue(res, 0, 2));
  for (i = 0; i <= atoi(PQgetvalue(res, 0, 1))-1; i++) {
    indIt[0][i] = 0;
  }
  j = i-1;
  for (i = 1; i < PQntuples(res); i++) {
    u = atoi(PQgetvalue(res, i, 1)) - 1;
    vIt[0][i].ind = atoi(PQgetvalue(res, i, 0)) - 1;
    vIt[0][i].val = atof(PQgetvalue(res, i, 2));
    if (j == u) continue;
    if(j + 1 < u){
      for(int l=j+1; l <= u; l++){
        indIt[0][l] = i;
      }
    }
    else{
      indIt[0][u] = i;
    }
    j = u;
  }
  indIt[0][u+1] = PQntuples(res);
  *tIt = m2;

  PQclear(res);


  sprintf(querty, "INSERT INTO test SELECT * FROM (SELECT user_id, item_id FROM %s WHERE rand > %.3f AND rand <= %.3f AND rating = %d) A WHERE user_id IN (SELECT user_id FROM tempa GROUP BY user_id) AND item_id IN (SELECT item_id FROM tempa GROUP BY item_id) ORDER BY user_id, item_id", tabla, (float) (k-1-parte)/k, (float) (k-parte)/k, maxRat);
  PQexec(conn, querty);
  res = PQexec(conn, "select * from tempb");

  test[0] = (EvalR*) malloc(sizeof(EvalR)*PQntuples(res));
  for(i=0; i< PQntuples(res); i++){
    test[0][i].us = atoi(PQgetvalue(res, i, 0));
    test[0][i].it = atoi(PQgetvalue(res, i, 1));
    test[0][i].rat = (float) maxRat;
  }
  *tTest = PQntuples(res);

  PQclear(res);
  PQfinish(conn);
  return;
}



















