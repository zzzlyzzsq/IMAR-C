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
struct svm_model* create_svm_model(int k, struct svm_problem svmProblem);
void printProbability(struct svm_model* pModel, struct svm_node* nodes);

// Import / Print
struct svm_problem importProblem(std::string file, int k);
void exportProblem(struct svm_problem svmProblem, std::string file);
void exportProblemZero(struct svm_problem svmProblem, std::string file, int k);
void printProblem(struct svm_problem svmProblem);
//void printNodes(struct svm_node* nodes);
//struct svm_node* importNodes(char* file);

struct svm_problem computeBOW(int label, const KMdata& dataPts, KMfilterCenters& ctrs);


// Other
int nrOfLines(std::string filename);
int print_scores(const struct svm_model *model,
		 const struct svm_node *x);
double entropy(double x, double lambda);
SvmProbability* svm_calculate_probability(int* labels,
					  double* dec_values,
					  int nr_class);
// return index of label elected
int svm_vote(int nrClass,
	      int votes[],
	      double decisionValues[]
	      );

void destroy_svm_problem(struct svm_problem svmProblem);
void addBOW(const struct svm_problem& svmBow, struct svm_problem& svmProblem);

//confusion matrix
class MatrixC{
 public:
  MatrixC(const svm_model* model);
  MatrixC(int nr_class, int* labels);
  ~MatrixC();
  void output();
  void exportMC(std::string folder, std::string file);
  void calculFrequence();
  int getIndex(double lab);
  void addTransfer(double lab_in,double lab_out);
  double** getMatrix();
 private:
  int** m;
  double* labels;
  double** m_fre;
 public:
  int num_labels;
  int nrTest;
  int nrRecognition;
  double recognitionRate;
};

// Normalization
// Simple normalization which depends only of one bag of words
void bow_simple_normalization(struct svm_problem &svmProblem);

// Gaussian normalization
void get_gaussian_parameters(int k,
			     struct svm_problem svmProblem,
			     double* means,
			     double* stand_devia);
void save_gaussian_parameters(std::string path2bdd,
			      int k,
			      double* means,
			      double* stand_devia);
void load_gaussian_parameters(std::string path2bdd,
			      int k,
			      double* means,
			      double* stand_devia);
void bow_gaussian_normalization(int k,
				double* means,
				double* stand_devia,
				struct svm_problem &svmProblem);

// Extra functionalities
struct svm_problem equalizeSVMProblem(const struct svm_problem& svmProblem,
				      struct svm_problem& svmTest);
int get_svm_problem_labels(const struct svm_problem& svmProblem, int* labels);
int getMaxIndex(const struct svm_problem& svmProblem);
int getMinNumVideo(const struct svm_problem& svmProblem);
svm_model **svm_train_ovr(const svm_problem *prob, const svm_parameter *param);
void get_svm_parameter(int k, struct svm_parameter &svmParameter);
std::vector<double> get_labels_from_prob(const svm_problem *prob);
#endif
