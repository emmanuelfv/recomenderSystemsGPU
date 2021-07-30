
#include "POP.h"

void getCrossSets_POP(int dataset, int kGrupos, int parte, int nRecomendaciones, int **pops, int *nU, Elemento **test, int **indTest) {
  char *tabla = getNombreTabla(dataset);

  pops[0] = (int*) malloc(sizeof(int)*nRecomendaciones);

  PGconn *conn = PQconnectdb(DATOS_CONECCION);
  checkPGconn(conn);

  PGresult *res;
  
  char querty[300];
  sprintf(querty, "SELECT item_id FROM %s GROUP BY item_id ORDER BY AVG(*) DESC LIMIT %d", tabla, nRecomendaciones);
  res = PQexec(conn, querty);

  for(int i=0; i<PQntuples(res); i++){
      pops[0][i] = atoi(PQgetvalue(res, i, 0));
  }

  PQclear(res);
  PQfinish(conn);
  free(tabla);
  return;
}


void simple_pop(int *pops, int nU, int nRecomendaciones, Elemento **recomendaciones){
    recomendaciones[0] = (Elemento *) malloc(sizeof(Elemento) * nU * nRecomendaciones);

    for(int i=0; i<nU; i++){
        for(int j=0; j<nRecomendaciones; j++){
            recomendaciones[0][i*nRecomendaciones + j].val = pops[j];
            recomendaciones[0][i*nRecomendaciones + j].ind = 0;
        }
    }
}
