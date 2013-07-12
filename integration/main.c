/** @author Fabien ROUALDES (institut Mines-Télécom)
 *  @file  main.c
 *  @date 26/06/2013 
 *  Program permiting to use kmeans and svm without writing on hard drive and linked to dynamic libraries 
 */
#include "naosvm.h"
#include "naokmeans.h"
#include "naodensetrack.h"

#include "Box.h"
#include <iostream>
#include <fstream>

/*========== partie relative a la mesure du temps ===========*/
#include <sys/times.h>
#include <unistd.h>
typedef struct tms sTms;
typedef struct{
  int debut,fin;
  sTms sdebut, sfin;
}temps_exec;

/*========== partie relative a la mesure du temps ===========*/
void temp(temps_exec *tmps){
  printf("temps réel :\t %d\ntemps utilisateur :\t %ld\ntemps system :\t %ld\n",
	 tmps->fin - tmps->debut,
	 tmps->sfin.tms_utime - tmps->sdebut.tms_utime,
	 tmps->sfin.tms_stime - tmps->sdebut.tms_stime);
}

using namespace std;

int main(int argc, char* argv[]){
  temps_exec temps;
  if(argc < 4){
    cerr << "Usage: ./main <file to video> <kmeans center> <svm model>" << endl;
    exit(EXIT_FAILURE);
  }
  // FILES
  std::string video(argv[1]);
  //std::string stip(argv[1]);
  std::string means(argv[2]);
  std::string model(argv[3]);
  
  /* KMeans BOWing	*/
  int k = 100;
  int dim = 204; // STIPs dimension 
  int maxPts = 500000; // max number of data points
  
  std::cout << "Importing the STIPs...";
  
  KMdata dataPts(dim,maxPts);
  
  temps.debut = times(&temps.sdebut);
  int nPts = extractSTIPs(video.c_str(), dim, maxPts, &dataPts);
  //int nPts = importSTIPs(stip.c_str(),dim,maxPts, &dataPts);
  
  dataPts.setNPts(nPts);			// set actual number of pts
  dataPts.buildKcTree();			// build filtering structure
  temps.fin = times(&temps.sfin);
  std::cout << nPts << " vectors imported!" << std::endl;
  temp(&temps);
  
  std::cout << "Importing centers...";

  // IIMPORT TRAINING CENTERS
  //int nrCenters = importCenters();
  KMfilterCenters ctrs(k, dataPts);		// allocate centers
  temps.debut = times(&temps.sdebut); 
  importCenters(means.c_str(), dim, k, &ctrs);
  temps.fin = times(&temps.sfin);
  temp(&temps); 
  
  temps.debut = times(&temps.sdebut); 
 
  //ctrs.print();
  //  std::cout << "Sums: " << ctrs.getSumSqs() << std::endl;
  std::cout << "Done!" << std::endl;

  std::cout << "Creating te BOW histogram...";
  struct svm_problem svmProblem = computeBOW(0,dataPts, ctrs);
 
  cout << "Importing the SVM model...";
  struct svm_model* pSvmModel = svm_load_model(model.c_str());
  cout << "Done!" << endl;
  
  // SVM PREDICT PROBABILITY UNIQUEMENT SI LE MODELE A ETE CREE DE CETTE MANIERE
  //printProbability(pSvmModel,svmNodes);

  int label = svm_predict(pSvmModel,svmProblem.x[0]);
  cout << "Label predicted: " << label << endl; 
  temps.fin = times(&temps.sfin);
  temp(&temps);
  
  svm_free_and_destroy_model(&pSvmModel);
  return EXIT_SUCCESS;
}
