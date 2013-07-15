/** @author Fabien ROUALDES (institut Mines-Télécom)
 *  @file  main.c
 *  @date 26/06/2013 
 *  Program permiting to use kmeans and svm without writing on hard drive and linked to dynamic libraries 
 */
#include "naosvm.h"
#include "naokmeans.h"
#include "naodensetrack.h"
#include "integration.h"
#include "Box.h"
#include <iostream>
#include <fstream>

using namespace std;

int main(int argc, char* argv[]){
  exec_time execTime;
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
  
  std::cout << "Extracting the STIPs..." << std::endl;
  execTime.begin = times(&execTime.sbegin);
  KMdata dataPts(dim,maxPts);
  int nPts = extractSTIPs(video.c_str(), dim, maxPts, &dataPts);
  //int nPts = importSTIPs(stip.c_str(),dim,maxPts, &dataPts);
  dataPts.setNPts(nPts);			// set actual number of pts
  dataPts.buildKcTree();			// build filtering structure
  execTime.end = times(&execTime.send);
  std::cout << nPts << " vectors imported!" << std::endl;
  printTime(&execTime);
  
  std::cout << "Importing centers..."  << std::endl;
  execTime.begin = times(&execTime.sbegin); 
  KMfilterCenters ctrs(k, dataPts);		// allocate centers
  importCenters(means.c_str(), dim, k, &ctrs);
  execTime.end = times(&execTime.send);
  printTime(&execTime); 
  
  std::cout << "Computing Bag Of Words..." << std::endl;
  execTime.begin = times(&execTime.sbegin);
  struct svm_problem svmProblem = computeBOW(0,dataPts,ctrs);
  execTime.end = times(&execTime.send);
  printTime(&execTime);
  printProblem(svmProblem);

  std::cout << "Importing the SVM model..."  << std::endl;
  struct svm_model* pSvmModel = svm_load_model(model.c_str());
  
  std::cout << "Predicting activity..." << std::endl;
  execTime.begin = times(&execTime.sbegin);
  // printProbability(pSvmModel,svmProblem.x[0]);
  std::cout << "Label predicted: " << svm_predict(pSvmModel,svmProblem.x[0]) <<  std::endl;
  execTime.end = times(&execTime.send);
  printTime(&execTime);

  svm_free_and_destroy_model(&pSvmModel);
  return EXIT_SUCCESS;
}
