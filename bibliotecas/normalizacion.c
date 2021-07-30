
#include "normalizacion.h"


void aleatoriza(EvalR *vec, int tamVec){
  int r;

  EvalR temp;
  for(int i=0; i<tamVec; i++){
    r = rand()%tamVec;
    temp = vec[i];
    vec[i] = vec[r];
    vec[r] = temp;
  }
}

//-----------------------------------------------------------------------
//funciones de comparación
int cmpfuncInt (const void * a, const void * b) { 
  //para enteros
  return ( *(int*)a - *(int*)b );
}

int cmpfuncElem (const void * a, const void * b) { 
  //para estructuras Elemento
  Elemento *a1 = (Elemento * ) a;
  Elemento *a2 = (Elemento * ) b;
  return ( a2->val*1000000 - a1->val*1000000);
}

int cmpfuncRatIt (const void * a, const void * b) { 
  //para items en estructuras Eval
  EvalR *a1 = (EvalR * ) a;
  EvalR *a2 = (EvalR * ) b;
  return (a1->it - a2->it);
}

int cmpfuncRatUs (const void * a, const void * b) { 
  //para usuarios en estructuras Eval
  EvalR *a1 = (EvalR * ) a;
  EvalR *a2 = (EvalR * ) b;
  return ( a1->us - a2->us);
}

//---------------------------------------------------------------------------
//funciones base
Elemento* mediaVec(EvalR *datos, int r, int *tam, int tipo) {
  //a partir de un vector de evaluaciones ordenado obtiene: 
  //un conjunto de usuarios así como su media
  //asigna tambien el tamaño n de los usuarios en el puntero tam
  //complejidad r
  Elemento *vec = (Elemento *) malloc (sizeof(Elemento)*100000);
  int i, j, sum, cont;

  j=0;
  //usuarios
  if(tipo == USER){
    vec[j].ind = datos[0].us;
    sum = datos[0].rat;
    cont = 1;
    for(i=1; i<r; i++){
      if(datos[i].us != vec[j].ind){
        vec[j].val = (float) sum/cont;
        sum = 0;
        cont = 0;
        j++;
        vec[j].ind = datos[i].us;
      }
      sum += datos[i].rat;
      cont++;
    }
    vec[j].val = (float) sum/cont;
  } 
  //items
  else if(tipo == ITEM){
    vec[j].ind = datos[0].it;
    sum = datos[0].rat;
    cont = 1;
    for(i=1; i<r; i++){
      if(vec[j].ind != datos[i].it){
        vec[j].val = (float) sum/cont;
        sum = 0;
        cont = 0;
        j++;
        vec[j].ind = datos[i].it;
      }
      sum += datos[i].rat;
      cont++;
    }
    vec[j].val = (float) sum/cont;
  }
  j++;
  *tam = j;

  vec = (Elemento *) realloc(vec, j*sizeof(Elemento));  
  return vec;
}

void normMedia(EvalR *datos, int r, Elemento *us, int tamUs){ 
  //ajusta los valores de las calificaciones de forma que resta la media de los usuarios
  //a la calificación orotgada por este. El vector de datos debe estar ordenado por usuario.
  //complejidad r
  int i, j;
  j=0;
  for (i=0; i<r; i++){
    if(datos[i].us == us[j].ind){
      datos[i].rat = datos[i].rat - us[j].val;
      continue;
    }
    j++;
    datos[i].rat = datos[i].rat - us[j].val;
  }
}

float **normB(EvalR *datos, int r, int tamUs, int tamIt, float lamda2, float lamda3){
  //Ajusta los datos de las evaluaciones con respecto a b_ui
  // printf("%d\n", r);
  
  
  //mu
  float mu = 0;
  for(int i=0; i<r; i++){
    mu += datos->rat;
  }
  mu = mu/r;

  //bi
  qsort(datos, r, sizeof(EvalR), cmpfuncRatIt);
  float *bi = (float *) malloc(sizeof(float)*tamIt);
  int j = 0;
  int cont = 1;
  int actual = datos[0].it;
  float sum = datos[0].rat - mu;
  for(int i=1; i<r; i++){
      if(actual != datos[i].it){
        bi[j] = sum/(lamda2 + cont);
        sum = 0;
        cont = 0;
        actual = datos[i].it;
        j++;
      }
      sum += datos[i].rat - mu;
      cont++;
  // printf("%d %d\n", i, j);
  }
  bi[j] = sum/(lamda2 + cont);

  //bu
  qsort(datos, r, sizeof(EvalR), cmpfuncRatUs);
  // for(int z=0; z<50; z++) printf("%d %d %f\n", datos[z].us, datos[z].it, datos[z].rat);
  float *bu = (float *) malloc(sizeof(float)*tamUs);
  j = 0; cont = 1;
  actual = datos[0].us;
  sum = datos[0].rat - mu - bi[datos[0].it];
  for(int i=1; i<r; i++){
    if(actual != datos[i].us){
      bu[j] = sum/(lamda3 + cont);
      sum = 0;
      cont = 0;
      actual = datos[i].us;
      j++;
    }
    sum = datos[i].rat - mu - bi[datos[i].it];
    cont++;
  }
  bu[j] = sum/(lamda3 + cont);

  //b_ui
  for(int i=0; i<r; i++){
    datos[i].rat = datos[i].rat - mu - bi[datos[i].it] - bu[datos[i].us];
  }

  float **b = (float **) malloc(sizeof(float*)*3);
  b[0]= (float *) malloc(sizeof(float));
  b[0][0] = mu;
  b[1] = bi;
  b[2] = bu;

  return b;
}

/*

    vec[j].ind = datos[0].it;
    sum = datos[0].rat;
    cont = 1;
    for(i=1; i<r; i++){
    }
    vec[j].val = (float) sum/cont;
  }
  j++;
  *tam = j;
  return vec;
  */


VL *getLista(EvalR *datos, int r, int tamVec, int tipo){ 
  //a partir de los datos ordenados por usuario (si tipo == USER) o
  //ordenados por item (si tipo == ITEM), llena un vector con user/item
  //donde cada uno tiene su lista de items/users así como el tamaño de 
  //estos y su calificación
  //complejidad r
  int i, j, tamLi;
  j=0;
  tamLi = 1;
  VL* vec = (VL*) malloc(sizeof(VL)*tamVec);
  vec[j].list = (int *) malloc(sizeof(int));
  vec[j].rat = (float *) malloc(sizeof(float));

  if(tipo == USER){
    vec[j].item = datos[0].us;
    vec[j].list[0] = datos[0].it;
    vec[j].rat[0] = datos[0].rat;
    for(i=1; i<r; i++){
      if(datos[i].us == vec[j].item){
        vec[j].list = (int *) realloc(vec[j].list, (++tamLi)*sizeof(int));
        vec[j].rat = (float*) realloc(vec[j].rat, tamLi*sizeof(float));
        vec[j].list[tamLi-1] = datos[i].it;
        vec[j].rat[tamLi-1] = datos[i].rat;
      } else {
        vec[j].tam = tamLi;
        j++;
        tamLi = 1;
        vec[j].item = datos[i].us;
        vec[j].list = (int*) malloc(sizeof(int));
        vec[j].rat = (float*) malloc(sizeof(float));
        vec[j].list[0] = datos[i].it;
        vec[j].rat[0] = datos[i].rat;
      }
    }
  }else if(tipo == ITEM){
    vec[j].item = datos[0].it;
    vec[j].list[0] = datos[0].us;
    vec[j].rat[0] = datos[0].rat;
    for(i=1; i<r; i++){
      if(vec[j].item == datos[i].it){
        vec[j].list = (int *) realloc(vec[j].list, ++tamLi*sizeof(int));
        vec[j].rat = (float*) realloc(vec[j].rat, tamLi*sizeof(float));
        vec[j].list[tamLi-1] = datos[i].us;
        vec[j].rat[tamLi-1] = datos[i].rat;
      } else {
        vec[j].tam = tamLi;
        j++;
        tamLi = 1;
        vec[j].item = datos[i].it;
        vec[j].list = (int*) malloc(sizeof(int));
        vec[j].rat = (float*) malloc(sizeof(float));
        vec[j].list[0] = datos[i].us;
        vec[j].rat[0] = datos[i].rat;
      }
    }
  }
  vec[j].tam = tamLi;
  j++;

  return vec;
}

//void contadores(Eval *datos, int r, int *U, int *I){
// void contadores(Eval *datos){
//   /*Esta función se encarga de contar los items, no necesita ningún ordenamiento ya que los datos
//   ya estan dispuestos de forma que ui = [1:n], i_j = [1:m] */

//   int maxU=0, maxI=0;
//   for(int i=0; i<datos->tREC; i++){
//     if(datos->REC[i].us>maxU) maxU = datos->REC[i].us;
//     if(datos->REC[i].it>maxI) maxI = datos->REC[i].it;
//   }
//   datos->numU = maxU;
//   datos->numI = maxI;
// }

////

//---------------------------------------------------------------------------
//busquedas binarias (cambiar por tablas de hash)
int busquedabinaria(void *vector, int m, int n, void *val, int tipo) {
  // de un vector de palabras retorna el indice encontrado
  if (n - m <= 1) {
    if (tipo == INT__) {
      int *vec = (int *)vector;
      int ent = *((int *)val);
      if (vec[m] == ent)
        return m;
      else
        return -1; // no es un usuario valido
    } else if (tipo == STRING__) {
      char **vec = (char **)vector;
      char *ent = (char *) val;
      if (strcmp(vec[m], ent) == 0)
        return m;
      else
        return -1;
    }
  }

  int mitad = (m + n) / 2;
  int temp;
  if (tipo == INT__) {
    int *vec = (int *)vector;
    int ent = *((int *)val);
    temp = ent < vec[mitad];
  } else if (tipo == STRING__) {
    char **vec = (char **)vector;
    char *ent = (char *) val;
    temp = strcmp(ent, vec[mitad]) < 0;
  }

  if (temp) {
    return busquedabinaria(vector, m, mitad, val, tipo);
  } else {
    return busquedabinaria(vector, mitad, n, val, tipo);
  }
}

int bbStr(char **vector, int m, int n, char *val) {
  // de un vector de palabras retorna el indice encontrado
  if (n - m <= 1) {
    if (strcmp(vector[m], val) == 0)
      return m;
    else
      return -1;
  }

  int mitad = (m + n) / 2;
  if (strcmp(val, vector[mitad]) < 0) { // val<vector[mitad]
    return bbStr(vector, m, mitad, val);
  } else {
    return bbStr(vector, mitad, n, val);
  }
}

int bb1(VL *vector, int m, int n, int val){
  //De un vector de tipo lista, retorna el indice del vector
  if(n-m <= 1){
    if(vector[m].item==val) return m;
    else return -1; //no es un usuario valido
  }

  int mitad = (m + n) / 2;
  if(val<vector[mitad].item){ 
    return bb1(vector, m, mitad, val);
  }
  else{
    return bb1(vector, mitad, n, val);
  }
}

int bb2(int *vector, int m, int n, int val){
  //de un vector de enteros retorna el indice encontrado

  if(n-m <= 1){
    if(vector[m]==val) return m;
    else return -1;
  }

  int mitad = (m+n) / 2;
  if(val<vector[mitad]){ 
    return bb2(vector, m, mitad, val);
  }
  else{
    return bb2(vector, mitad, n, val);
  }
}

int bb3(VIC *vector, int m, int n, int val){
  //de un vector de contadores, retorna el indice de estos
  if(n-m <= 1){
    if(vector[m].x==val) return m;
    else return -1; //no es un usuario valido
  }

  int mitad = (m + n) / 2;
  if(val<vector[mitad].x){ 
    return bb3(vector, m, mitad, val);
  }
  else{
    return bb3(vector, mitad, n, val);
  }
}

int bbElem(Elemento *vector, int m, int n, int val){
  //de un vector Elemento, retorna el indice de un valor espesifico
  if(n-m <= 1){
    if(vector[m].ind==val) return m;
    else return -1; //no es un usuario valido
  }

  int mitad = (m + n) / 2;
  if(val<vector[mitad].ind){ 
    return bbElem(vector, m, mitad, val);
  }
  else{
    return bbElem(vector, mitad, n, val);
  }
}


int bbEvalUs(EvalR *vector, int m, int n, int val){
  //de un vector EvalR, retorna el indice de val
  if(n-m <= 1){
    if(vector[m].it==val) return m;
    else return -1; //no es un usuario valido
  }

  int mitad = (m + n) / 2;
  if(val<vector[mitad].it){ 
    return bbEvalUs(vector, m, mitad, val);
  }
  else{
    return bbEvalUs(vector, mitad, n, val);
  }
}

void freeVL(VL *vl, int tam){
  for(int i=0; i< tam; i++){
    free(vl[i].list);
    free(vl[i].rat);
  }
  free(vl);
  return;
}