


// int bb1(VL *vector, int m, int n, int val);
// int bb2(int *vector, int m, int n, int val);
// int bb3(VIC *vector, int m, int n, int val);
// int bbVM(Elemento *vector, int m, int n, int val);


double similitud_p1(VL *listaU, int tamUs, int u1, int u2);
double similitud_p2(VL *listaU, int tamUs, int u1, int u2, int humbral);
double similitud_p3(VL *listaU, int tamUs, int u1, int u2, int humbral, VIC *listaI, int tamIt);
double similitud_euc(VL *listaU, int tamUs, int u1, int u2);
double similitud_adjCos(VL *listaI, int tamIt, int i1, int i2, int h);

float recomendacion(float **M, VL *listaU, Elemento* medUs, int tamUs, int U, int I, int noUs);
float evaluacionSistema(EvalR *Test, int n, float **M, VL *listaU, Elemento *medUs, int tamUs, int noUs);
float recomendacionItem(float **M, VL *listaI, Elemento* medUs, int tamIt, int U, int I, int noIt, int tamUs);
float evaluacionSistemaItem(EvalR *Test, int n, float **M, VL *listaI, Elemento *medUs, int tamIt, int noIt, int tamUs);

void crossValidationT1(Eval *vecLectura, int k, int tam);
float **entrenamiento(EvalR *train, int trainTam, LISTS *trainL);
EvalR *limpiezaTest(EvalR *test, int *testTam, LISTS *trainL);
float *evaluacion(EvalR *test, int testTam, float **mat, LISTS *trainL);


Elemento **construccion_SP(int numUs, int numIt, VL *listaIt, int nVecinos);
//Elemento *topN(float *vecOrigen, int tVec, int n);
void aplicarModelo_SP(Elemento **mat, int numIt, 
                      int nVecinos, VL *listaUs, int nRecomendaciones, 
                      MatrixINT rec, float **b);