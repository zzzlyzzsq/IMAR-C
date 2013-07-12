/** @author Fabien ROUALDES (institut Mines-Télécom)
 *  @file naosvm.h
 *  @date 26/06/2013 
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

#define NB_MEANS 100

using namespace std;

// SVM prediction
void printProbability(struct svm_model* pModel, struct svm_node* nodes);

// Import / Print
struct svm_problem importProblem(char* file);
void printProblem(struct svm_problem svmProblem);
//void printNodes(struct svm_node* nodes);
//struct svm_node* importNodes(char* file);

// Other
int nrOfLines(std::string filename);
#endif
