/**
 * \file main.cpp
 * \brief Programme permettant d'analyser des données de Bag Of Words 
 * et de visualiser certaines données.
 *
 * \author Fabien ROUALDES (Institut Mines-Télécom)
 * \version 0.2
 * \date 30/06/2013
 *
 * Programme de calcul de moyennes, variances, de matrix de covariance de données Bag Of Words.
 * Il permet d'exporter ces données pour être lues par un logiciel de représentation graphique de données (en partiluer Gnuplot).
 */
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <math.h>
#include "matrixCalculation.h"

using namespace std;

typedef struct _bow{
  int label;
  float* histogram;
} Bow;

typedef struct _covarianceMatrix{
  int label;
  double** matrix;
} CovarianceMatrix;

int nombreLignes(std::string filename);

void importBows(char* file,int nrSequences, int nrCenters, Bow **pBows);
void exportBow(std::string type, int nrCenters, Bow bow);
void exportCovarianceMatrix(int nrCenters, CovarianceMatrix sigma);
void exportMeansWithSigma(int nrCenters, Bow meanBow, Bow varianceBow);

void printBows(int nrSequences,int nbCenters, Bow *bows);
int extractLabels(int nrSequences, Bow *bows, int **pLabels);
Bow* computeMeanBows(int nrSequences,int nbCenters, Bow *bows, int nrLabels, int *labels);
Bow* computeCenteredBows(int nrSequences,int nrCenteres, Bow *bows, int nrLabels, Bow *meanBows);
Bow* computeVarianceBows(int nrSequences,int nrCenters, Bow *centeredBows, int nrLabels, int *labels);
void freeBows(int nbBows, Bow* bows);
CovarianceMatrix* computeCovarianceMatrix(int nrSequences, int nrCenters, Bow* centeredBows,
					  int nrLabels, int* labels);

int main(int argc, char* argv[]){
  int nrCenters = 100;
  
  // Lecture du fichier et sommation de chaque lignes
  // au format :
  // label 1:value 2:value 3:value
  
  Bow* bows;
  int nrSequences = nombreLignes(argv[1]);
  // cout << "Importing BOWs..";
  importBows(argv[1],nrSequences,nrCenters,&bows);
  //cout << "Done!" << endl;
  //printBows(nrSequences,nrCenters,bows);
  
  int* labels;
  int nrActivities = extractLabels(nrSequences,bows,&labels);
  Bow* meanBows = computeMeanBows(nrSequences,nrCenters,bows,nrActivities,labels);
  Bow* centeredBows = computeCenteredBows(nrSequences,nrCenters,bows,nrActivities,meanBows);
  Bow* varianceBows = computeVarianceBows(nrSequences,nrCenters,centeredBows,nrActivities,labels);
  
  // Then, for each activities, we calculate the covariance matrix
  //sigma(k,k) = Var(bow(k,1)) = E[(bow(k,1) - ub)()T]
  CovarianceMatrix* sigmas = computeCovarianceMatrix(nrSequences,nrCenters,centeredBows,nrActivities,labels);
  
  for(int i=0; i<nrActivities ; i++){
    exportCovarianceMatrix(nrCenters,sigmas[i]);
    //exportBow("mean",nrCenters, meanBows[i]);
    exportBow("variance",nrCenters, varianceBows[i]);
    if(meanBows[i].label != varianceBows[i].label){
      cerr << "Error, labels do not match!" << endl;
      return EXIT_FAILURE;
    }
    exportMeansWithSigma(nrCenters,meanBows[i],varianceBows[i]);
  }
    
  /* RELEASE MEMORY */
  for(int i=0 ; i<nrActivities ; i++){
    sigmas[i].matrix = matrixDesallocation(sigmas[i].matrix);
  }
  free(sigmas);
  
  free(labels);
  freeBows(nrSequences,bows);
  freeBows(nrActivities,meanBows);
  freeBows(nrActivities,centeredBows);
  
  
// ouverture en écriture avec effacement du fichier ouvert


  return EXIT_SUCCESS;
}
void importBows(char* file,int nrSequences, int nrCenters, Bow **pBows){
  ifstream in(file, ios::in);
  int sequence = 0;
  
  // Initialization du tableau de structures puis des structures
  *pBows = (Bow*) malloc(nrSequences * sizeof(Bow));
  for(int i=0 ; i<nrSequences ; i++){
    (*pBows)[i].histogram = (float*) malloc(nrCenters * sizeof(float));
  }
  
  while(!(in.eof()) && (sequence < nrSequences)){  
    int center = 0;
    in >> (*pBows)[sequence].label;
    
    bool endOfLine = false;
    while(!endOfLine && (center < nrCenters)){
      string bowBuff;
      if(in >> bowBuff){ // on enregistre le flux dans le buffer s'il y a bien un caractère, puis on le traite
	// On converti la chaîne "dim:valeur" en valeur dans le float f
	istringstream iss(bowBuff);
	string token;
	float f;
	int i = 0;
	while(getline(iss,token,':')){
	  if (i==1){
	    std::stringstream ss(token);
	    ss >> f;
	  }
	  i++;
	}
	(*pBows)[sequence].histogram[center] = f; // then it is saved in the bow histogram
	center++;
      }
      else
	endOfLine = true; // s'il n'y a pas de caractère c'est qu'on a fini la ligne
    }
    sequence++;
  }
  in.close();
}
int extractLabels(int nrSequences, Bow* bows, int** pLabels){
  int* tmpLabels = (int *) malloc(nrSequences * sizeof(int));
  int index = 0;
  
  // Extraction des labels
  for (int seq = 0 ; seq < nrSequences ; seq++){
    bool labelExist = false;
    int label = bows[seq].label;
    int i = 0;
    while(i<index && !labelExist){
      if(tmpLabels[i] == label){
	labelExist = true;
      }
      i++;
      tmpLabels[index] = label;
    }
    if(!labelExist){
      tmpLabels[index] = label;
      index++;
    }
  }
  index--;
  
  *pLabels = (int *) malloc((index+1) * sizeof(int));
  int i = 0;
  while(i<(index+1)){
    (*pLabels)[i] = tmpLabels[i];
    i++;
  }
  free(tmpLabels);
  
  return index + 1;
}
Bow* computeMeanBows(int nrSequences, int nrCenters, Bow* bows,int nrLabels, int* labels){
  // Allocation of the mean bag of words
  Bow* meanBows = (Bow *) malloc(nrLabels * sizeof(Bow));
  for(int i=0 ; i<nrLabels; i++){
    meanBows[i].histogram = (float*) malloc(nrCenters * sizeof(float));
    for(int c= 0 ; c<nrCenters ; c++){
      meanBows[i].histogram[c] = 0;
    }
  }
  
  // pour chaque labels on fait la moyenne
  for(int lab = 0 ; lab < nrLabels ; lab++){
    int currentLab = labels[lab];
    meanBows[lab].label = currentLab;
    int nrActivities = 0;
    // on parcourt le tableau de BOWs
    for(int i=0 ; i<nrSequences ; i++){
      // If the BOW corresponds to the current label
      // it is summed to the current mean BOW
      if(bows[i].label == currentLab){
	for(int c=0 ; c<nrCenters ; c++){
	  meanBows[lab].histogram[c] += bows[i].histogram[c];
	}
	nrActivities++;
      }
    }
    // Then it is divided by the number of BOWs corresponding to the 
    // current label
    for(int c=0 ; c<nrCenters ; c++){
      meanBows[lab].histogram[c] /= nrActivities;
    }
  } 
  return meanBows;

}
Bow* computeVarianceBows(int nrSequences,int nrCenters, Bow* centeredBows, int nrLabels, int* labels){
  // Allocation of the mean bag of words
  Bow* varianceBows = (Bow *) malloc(nrLabels * sizeof(Bow));
  for(int i=0 ; i<nrLabels; i++){
    varianceBows[i].histogram = (float*) malloc(nrCenters * sizeof(float));
    for(int c= 0 ; c<nrCenters ; c++){
      varianceBows[i].histogram[c] = 0;
    }
  }
  
  // pour chaque labels on fait la moyenne
  for(int i = 0 ; i < nrLabels ; i++){
    int currentLab = labels[i];
    varianceBows[i].label = currentLab;
    int nrActivities = 0; // It is actually the numer of BOWs per activities
    // on parcourt le tableau de BOWs
    for(int s=0 ; s<nrSequences ; s++){
      // If the BOW corresponds to the current label
      // it is summed to the current mean BOW
      if(centeredBows[s].label == currentLab){
	for(int c=0 ; c<nrCenters ; c++){
	  varianceBows[i].histogram[c] += (centeredBows[s].histogram[c])*(centeredBows[s].histogram[c]);
	}
	nrActivities++;
      }
    }
    // Then it is divided by the number of BOWs corresponding to the 
    // current label
    for(int c=0 ; c<nrCenters ; c++){
      varianceBows[i].histogram[c] /= nrActivities;
    }
  } 
  return varianceBows;
}
Bow* computeCenteredBows(int nrSequences,int nbCenters, Bow *bows, int nrLabels, Bow *meanBows){
  Bow* centeredBows = (Bow*) malloc(nrSequences * sizeof(Bow));
  for(int i=0 ; i<nrSequences ; i++){
    centeredBows[i].histogram = (float*) malloc(nbCenters * sizeof(float));
    for(int c= 0 ; c<nbCenters ; c++){
      centeredBows[i].histogram[c] = 0;
    }
  }
  int seq = 0;
  while(seq < nrSequences){
    centeredBows[seq].label = bows[seq].label;
    // We search the meanBow associated to the bow
    int l = 0; 
    while(l<nrLabels && meanBows[l].label != centeredBows[seq].label){
      l++;
    }
    if(meanBows[l].label != centeredBows[seq].label){
      cerr << "computeCenteredBows: label does not exist!" << endl;
      exit(EXIT_FAILURE);
    }
    
    // And we soustract it from the bow. The result is savec in the centeredBows[seq].histogram
    int center = 0;
    while(center < nbCenters){
      centeredBows[seq].histogram[center] = bows[seq].histogram[center] - meanBows[l].histogram[center];
      center++;
    }
    seq++;
  }
  
  return centeredBows;
}
CovarianceMatrix* computeCovarianceMatrix(int nrSequences, int nrCenters, Bow* centeredBows,
					  int nrLabels, int* labels){
  CovarianceMatrix* tmpCovMatrix = (CovarianceMatrix*) malloc(nrSequences * sizeof(CovarianceMatrix));
  for(int seq = 0; seq<nrSequences ; seq++){
    double** X = memoryAllocation(nrCenters,1);
    for(int c=0 ; c<nrCenters ; c++){
      X[c][1] = centeredBows[seq].histogram[c];
    }
    double** XT = transposeMatrix(X,nrCenters,1);
    double** XXT = multiplyMatrix(X,nrCenters,1,XT,1,nrCenters);
    X = matrixDesallocation(X);
    XT = matrixDesallocation(XT);
    tmpCovMatrix[seq].label = centeredBows[seq].label;
    tmpCovMatrix[seq].matrix = XXT;
  }
  
  CovarianceMatrix* covMatrix = (CovarianceMatrix*) malloc(nrLabels * sizeof(CovarianceMatrix));
 
  // Initialisation de la matrice
  for(int lab = 0 ; lab < nrLabels ; lab++){
    int currentLab = labels[lab];
    covMatrix[lab].label = currentLab;
    covMatrix[lab].matrix = memoryAllocation(nrCenters,nrCenters);
    for(int ci=0 ; ci<nrCenters ; ci++){
      for(int cj=0 ; cj<nrCenters ; cj++){
	covMatrix[lab].matrix[ci][cj] = .0;
      }
    }
  }
  
  // pour chaque labels on fait la moyenne
  for(int lab = 0 ; lab < nrLabels ; lab++){
    int currentLab = labels[lab];
    covMatrix[lab].label = currentLab;
    int nrActivities = 0;
    // on parcourt le tableau de matrices
    for(int i=0 ; i<nrSequences ; i++){
      // If the matrix corresponds to the current label
      // it is summed to the current covMatrix
      if(tmpCovMatrix[i].label == currentLab){
	for(int ci=0 ; ci<nrCenters ; ci++){
	  for(int cj=0 ; cj<nrCenters ; cj++){
	    covMatrix[lab].matrix[ci][cj] += tmpCovMatrix[i].matrix[ci][cj];
	  }
	}
	nrActivities++;
      }
    }
    // Then it is divided by the number of matrix corresponding to the 
    // current label
    for(int ci=0 ; ci<nrCenters ; ci++){
      for(int cj=0 ; cj<nrCenters ; cj++){
	covMatrix[lab].matrix[ci][cj] /= nrActivities;
      }
    } 
  }
  for(int i =0 ; i<nrSequences ; i++){
    tmpCovMatrix[i].matrix = matrixDesallocation(tmpCovMatrix[i].matrix);
  }
  free(tmpCovMatrix);
  
  return covMatrix;
}

void printBows(int nbBows,int nbCenters, Bow *bows){
  for(int i=0 ; i<nbBows ; i++){
    cout << bows[i].label << " ";
     for(int center=0 ; center < nbCenters ; center++){
       cout << center+1 << ":" << bows[i].histogram[center] << " ";
     }
     cout << endl;
  }
}
int nombreLignes (std::string filename) {
  std::ifstream fichier(filename.c_str());
  if(!fichier){
    std::cout << "Ne peut ouvrir " << filename << std::endl;
    exit(EXIT_FAILURE);
  }
  
  std::string s;
  unsigned int count = 0;
  while(std::getline(fichier,s)) ++count;
  fichier.close();
  
  return count;
}
void freeBows(int nbBows, Bow* bows){
  // Release bows
  for(int b=0 ; b<nbBows ; b++){
    free(bows[b].histogram);
  }
  free(bows);
}
void exportBow(std::string type, int nrCenters, Bow bow){
  std::ostringstream oss;
  oss << bow.label;
  std::string bowFile(type + "_bow." + oss.str() + ".dat");
  
  std::ofstream out(bowFile.c_str(), ios::out | ios::trunc); 
  if(!out){
    cerr << "Impossible d'ouvrir le fichier en écriture!" << endl;
    exit(EXIT_FAILURE);
  }
  out << "# label: " << bow.label << endl;
  out << "# means:" << endl;
  out << "#";
  for(int c=0; c<nrCenters ; c++){
    out << c + 1 << "\t";
  }
  out << endl;
  for(int c= 0 ; c<nrCenters ; c++){
    out << bow.histogram[c] << "\t"; 
  }
  out.close();
}
void exportCovarianceMatrix(int nrCenters, CovarianceMatrix sigma){
  std::ostringstream oss;
  oss << sigma.label;
  std::string sigmaOut("bow_covariance." + oss.str() + ".dat");
  //ouverture en écriture avec effacement du fichier ouvert
  std::ofstream out(sigmaOut.c_str(), ios::out | ios::trunc); 
  if(!out){
    cerr << "Impossible d'ouvrir le fichier en écriture!" << endl;
    exit(EXIT_FAILURE);
  }    
  for(int ci=0 ; ci<nrCenters ; ci++){
    for(int cj=0 ; cj<nrCenters ; cj++){ 
      out << sigma.matrix[ci][cj] << " ";
    }
    out << endl;
  }
  out.close();
}
void exportMeansWithSigma(int nrCenters, Bow meanBow, Bow varianceBow){
  std::ostringstream oss;
  oss << meanBow.label;
  std::string file("bow_means_with_sigma." + oss.str() + ".dat");
  //ouverture en écriture avec effacement du fichier ouvert
  std::ofstream out(file.c_str(), ios::out | ios::trunc); 
  if(!out){
    cerr << "Impossible d'ouvrir le fichier en écriture!" << endl;
    exit(EXIT_FAILURE);
  }
  out << "# label: " << meanBow.label << endl;
  out << "# means:" << endl;
  out << "#";
  for(int c=0; c<nrCenters ; c++){
    out << c + 1 << "\t";
  } 
  out << endl;
  
  // The Bow less 2*square(variance)

  for(int c=0 ; c<nrCenters ; c++){
    double value = meanBow.histogram[c] - 2*sqrt(varianceBow.histogram[c]);
    if(value < 0)
      value = 0;
    out << value << " "; 
  }
  out << endl;
  
  // The BOW
  for(int c= 0 ; c<nrCenters ; c++){
    out << meanBow.histogram[c] << "\t"; 
  }
  out << endl;
  
  // The Bow plus 2*square(variance)
  for(int c=0 ; c<nrCenters ; c++){
    out << meanBow.histogram[c] + 2*sqrt(varianceBow.histogram[c]) << " ";
  }
  out << endl;
  out.close();
}
