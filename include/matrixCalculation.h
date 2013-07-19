#ifndef _CALCUL_MATRICIEL_H
#define _CALCUL_MATRICIEL_H 1

#include <stdlib.h>
#include <stdio.h>

double** transposeMatrix(double** matrix, int rawNb, int columnNb);
double** multiplyMatrix(double** matrixA, int rawNbA, int columNbA, double** matrixB, int rawNbB, int columnNb);
double** memoryAllocation(int rawNb, int columnNb);
double **matrixDesallocation(double** matrix);

#endif
