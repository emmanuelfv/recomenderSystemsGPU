#define PSQL_UI 1
#define PSQL_UIR 2  // elimina elementos implicitos
#define PSQL_UIRT 3 // elimina elementos implicitos
#define PSQL_UIT 4

Eval *leerDB(int tipo);

void getCrossSets(int dataset, int k, int parte, Elemento **vUs, int **indUs, int *tUs, 
  Elemento **vIt, int **indIt, int *tIt, EvalR **test, int *tTest);
