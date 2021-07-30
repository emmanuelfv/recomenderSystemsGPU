#include <stdio.h>
#include <gsl/gsl_statistics.h>

typedef struct elem{
    int a;
    int b;
}ELEM;

int main (void) {
    ELEM *x = (ELEM*) malloc(sizeof(ELEM)*6);
    for(int i=0; i<6; i++) {
        x[i].a = i;
        x[i].b = 10*i;
    }

    printf("mean: %f\n", gsl_stats_int_mean( (int *) &x[0].b, 2, 6));
    return 0;
}
