/** \author Fabien ROUALDES (institut Mines-Télécom)
 *  \file naosvm.h
 *  \date 09/07/2013 
 *  Set of function permiting to import/ predict a svm problem, import/create a svm model
 */
#ifndef _NAOSVM_H_
#define _NAOSVM_H_
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include "svm.h"
#include "KMlocal.h"

using namespace std;

typedef struct _svm_probability{
  int label;
  double probability;
} SvmProbability;

// SVM prediction
struct svm_model* createSvmModel(std::string path2bdd/*bowFile*/, int k);
// Gaussian normalization
void gauss_normalization(std::string path2bdd,struct svm_problem &svmProblem,int k);
void printProbability(struct svm_model* pModel, struct svm_node* nodes);

// Import / Print
struct svm_problem importProblem(std::string file, int k);
struct svm_problem computeBOW(int label, const KMdata& dataPts, KMfilterCenters& ctrs);
void exportProblem(struct svm_problem svmProblem, std::string file);
void exportProblemZero(struct svm_problem svmProblem, std::string file, int k);
struct svm_problem computeBOW(int label, const KMdata& dataPts, KMfilterCenters& ctrs);
void printProblem(struct svm_problem svmProblem);
//void printNodes(struct svm_node* nodes);
//struct svm_node* importNodes(char* file);

// Other
int nrOfLines(std::string filename);
int print_scores(const struct svm_model *model,
		 const struct svm_node *x);
double entropy(double x, double lambda);
SvmProbability* svm_calculate_probability(int* labels,
					  double* dec_values,
					  int nr_class);
SvmProbability svm_vote(int* labels,
			double* dec_values,
			int nr_class);
//confusion matrix
class MatrixC{
 public:
  MatrixC(const svm_model* model);
  ~MatrixC();
  void output();
  void calculFrequence();
  int getIndex(double lab);
  void addTransfer(double lab_in,double lab_out);
  double** getMatrix();
 private:
  int** m;
  double* labels;
  int num_labels;
  double** m_fre;
};

#endif
