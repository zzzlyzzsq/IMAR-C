#include "matrixCalculation.h"

double** transposeMatrix(double** matrix, int rawNb, int columnNb){
  int i; // raw i
  int j; // column j
  double** transposedMatrix = NULL;
  transposedMatrix = memoryAllocation(columnNb,rawNb); 
  for(j=0 ; j < columnNb ; j++){
    for(i=0 ; i < rawNb ; i++){
      transposedMatrix[j][i] = matrix[i][j];
    }
  }
  return transposedMatrix;
}

double** multiplyMatrix(double** matrixA, int rawNbA, int columnNbA, double** matrixB, int rawNbB, int columnNbB){
  int i;
  int j;
  double** multipliedMatrix = NULL;
  multipliedMatrix = memoryAllocation(rawNbA,columnNbB);
  if(columnNbA != rawNbB){
    perror("Les deux matrices à multiplier n'ont pas la bonne dimension.\n");
    perror("Fin du programme.\n");
    exit(EXIT_FAILURE);
  }
  for(i=0 ; i<rawNbA ; i++){
    for(j=0 ; j<columnNbB ; j++){
      double sum = 0;
      int k;
      for(k=0 ; k < columnNbA ; k++){
	sum += (matrixA[i][k])*(matrixB[k][j]);
      }
      multipliedMatrix[i][j] = sum;
    }
  } 
  return multipliedMatrix;
}

double** memoryAllocation(int rawNb, int columnNb){
  double** matrix = NULL;
  int i;
  
  if ((matrix = (double**) malloc(rawNb * sizeof(double *)))==NULL){
    perror("Memory allocation error\n");
    exit(EXIT_FAILURE);
  }
  if ((matrix[0] = (double *) malloc(rawNb*columnNb*sizeof(double)))==NULL){
    perror("Memory allocation error\n");
    exit(EXIT_FAILURE);
  }
  // initialisation du tableau de pointeurs sur double
  for (i=1 ; i<rawNb ; i++)
    matrix[i]=matrix[0]+i*(columnNb);
  
  return matrix;
}

double **matrixDesallocation(double** matrix){
  free(matrix[0]);
  free(matrix);
  return NULL;
}
