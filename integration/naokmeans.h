/** @author Fabien ROUALDES (institut Mines-Télécom)
 *  @file naokmeans.h
 *  @date 02/07/2013 
 *  Set of function permiting to execute KMeans algorithms
 */
#ifndef _NAOKMEANS_H_
#define _NAOKMEANS_H_
#include <cstdlib>			// C standard includes
#include <iostream>			// C++ I/O
#include <string.h>			// C++ strings
#include <fstream>
#include "KMlocal.h"			// k-means algorithms

using namespace std;		

int importSTIPs(std::string stip, int dim, int maxPts, KMdata* dataPts);
void importCenters(std::string centers, int dim, int k, KMfilterCenters* ctrs);
void exportCenters(std::string centers, int dim, int k, KMfilterCenters ctrs);
void kmIvanAlgorithm(int ic, int dim,  const KMdata& dataPts, int k, KMfilterCenters& ctrs);
void kmLloydAlgorithm(int iterations, int dim,  const KMdata& dataPts, int k, KMfilterCenters& ctrs);
#endif
