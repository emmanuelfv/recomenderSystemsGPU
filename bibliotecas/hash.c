/*
La intención de esta actividad es mostrar el uso de tablas de hash para
la optimización de elementos de una tabla en cuanto al uso maximo de memoria
así como de tiempo de inserción, busqueda y eliminación de elementos.

Hecho por: Emmanuel Felix
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

typedef struct item{
  int k;    //clave
  char name[100]; //nombre de item
}ITEM;

typedef struct nodo{
  int k;
  ITEM * item;
  struct nodo * sig;
}HASH;


ITEM *leeCSV(int *n){
  int i, j, k, id;
  char name[190], str[200];
  FILE *f = fopen("anime.csv", "r+");

  ITEM *vec;
  vec = (ITEM*) malloc(sizeof(ITEM)*13000);

  //obtencion de datos de entrada Eval
  i = 0;
  
  fgets(str, 200, f);
  fgets(str, 200, f);
  while(!feof(f)){
    sscanf(str, "%d,%[^\n]%*c", &id, name);
    if(name[0] == '"'){
      j=1;
      k=0;
      while(name[j] != '"'){
        name[k++] = name[j++];
      }
      name[k] = '\0';
    } else {
      for(k=0; name[k] != ','; k++);
      name[k] = '\0';
    }
    
    vec[i].k = id;
    strcpy(vec[i].name, name);
    fgets(str, 200, f);
    i++;
    //printf("%6d... %s\n", id, name);
    //if(i>5) break;
  }
  sscanf(str, "%d,%[^\n]%*c", &id, name);
  if(name[0] == '"'){
    j=1;
    k=0;
    while(name[j] != '"') name[k++] = name[j++];
    name[k] = '\0';
  } else {
    for(k=0; name[k] != ','; k++);
    name[k] = '\0';
  }
  
  vec[i].k = id;
  strcpy(vec[i].name, name);
  fgets(str, 200, f);

  *n = i;

  return vec;
}

int esPrimo(int x){
  if(x==1) return 0;
  for(int i=2; i<=sqrt(x); i++){
    if(x%i==0) return 0;
  }
  return 1;
}

HASH ** generarTabla(ITEM * vec, int n, int *m){
  int i, x;

  *m = (int) ((float)n/0.8);
  while(!esPrimo((*m)++)); //se incrementa m hasta hacerlo un numero primo

  HASH **tablaH = (HASH**) malloc(sizeof(HASH*)*(*m)); //se crea un vector de nodos
  memset(tablaH, 0, *m);
  HASH * temp;

  for(i=0; i<n; i++){
    x = vec[i].k % *m;
    if(tablaH[x] == NULL){
      tablaH[x] = (HASH*) malloc(sizeof(HASH));
      tablaH[x]->k = vec[i].k;
      tablaH[x]->item = &vec[i];
      tablaH[x]->sig = NULL;
    } else {
      temp = tablaH[x];
      while(temp->sig != NULL) temp = temp->sig;
      temp->sig = (HASH*) malloc(sizeof(HASH));
      temp->sig->k = vec[i].k;
      temp->sig->item = &vec[i];
      temp->sig->sig = NULL;
    }
  }

  printf("Tabla generada correctamente\n");
  return tablaH;
}

ITEM * buscarObjeto(HASH ** tablaH, int k, int m){
  int i, x;
  HASH *temp;

  x = k % m;

  temp = tablaH[x];
  if(temp == NULL) return NULL; //no se encontró en la primera posición
  while(temp != NULL){
    if(temp->k == k) return temp->item;
    temp = temp->sig;
  }
  return NULL; //terminó de recorrer un vector y aún no se encuentra
}

void impresionItem(ITEM *item){
  if(item == 0){
    printf("No se encontró el item\n");
    return;
  }
  printf("clave %d, nombre item: %s\n", item->k, item->name);
  return;
}

void menu(void){
  printf("Menu:\n");
  printf("BuscarObjeto (1)\n");
  printf("InsertarObjeto (2)\n");
  printf("EliminarObjeto (3)\n");
  printf("ReindexarTabla (4)\n");
  printf("GenerarTabla (5)\n");
  printf("ver menu nuevamente (6)\n");
  printf("Salir (0)\n");
}

void main(){
  int op, n, i, k, m;

  ITEM *vecItems;
  HASH **tabla = NULL;
  ITEM *item = NULL;

  vecItems = leeCSV(&n); //de aquí se toman los atributos necesarios, que son id y nombre

  for(i=0; i<n; i++) printf("%5d  %s\n", vecItems[i].k, vecItems[i].name);
  //printf("%d...\n", n);
  menu();

  while(1){
    printf("Escoja función a utilizar:\n");
    scanf("%d", &op);

    if(op == 1){
      printf("Escoja la clave a buscar:\n");
      scanf("%d", &k);

      if(tabla != NULL)
        item = buscarObjeto(tabla, k, m);

      impresionItem(item);
      item = NULL;

    } else if(op == 2){

    } else if(op == 3){

    } else if(op == 4){

    } else if(op == 5){
      tabla = generarTabla(vecItems, n, &m);
      printf("%d\n", m);
    } else if(op == 6){
      
      menu();
    } else if(op == 0) return;
    printf("----------------------------------------------------------\n");
  }
}
