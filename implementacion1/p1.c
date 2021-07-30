#include <stdio.h>

#include <postgresql/libpq-fe.h>

#include "../bibliotecas/lecturaRS.h"

#include "../bibliotecas/lecturaPSQL.h"

int main(){
    Eval *a = leerDB(1);

    printf("%d\n", a->tREC);
    return 0;
}