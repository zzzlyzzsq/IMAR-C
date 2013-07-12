/** @author Fabien ROUALDES (institut Mines-Télécom)
 *  @file naosvm.h
 *  @date 09/07/2013 
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

// SVM prediction
void printProbability(struct svm_model* pModel, struct svm_node* nodes);

// Import / Print
struct svm_problem importProblem(std::string file, int k);
struct svm_problem computeBOW(int label, const KMdata& dataPts, KMfilterCenters& ctrs);
void exportProblem(struct svm_problem svmProblem, std::string file);
struct svm_problem computeBOW(int label, const KMdata& dataPts, KMfilterCenters& ctrs);
void printProblem(struct svm_problem svmProblem);
//void printNodes(struct svm_node* nodes);
//struct svm_node* importNodes(char* file);

// Other
int nrOfLines(std::string filename);
#endif
