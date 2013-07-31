/** @author Fabien ROUALDES (institut Mines-Télécom)
 *  @file  main.c
 *  @date 26/06/2013
 *  Program permiting to use kmeans and svm without writing on hard drive and linked to dynamic libraries
 */
#include "integration.h"

using namespace std;

std::string integration(std::string video, std::string folder){
  exec_time execTime;

  /* KMeans BOWing	*/
  int k = getK(folder);
  int desc = getDescID(folder);
  int dim = getDim(desc);
  int maxPts = 500000; // max number of data points

  std::cout << "Importing the STIPs..." << std::endl;
  execTime.begin = times(&execTime.sbegin);
  KMdata dataPts(dim,maxPts);
  int nPts = 0;
  switch(desc){
  case 0: //HOG HOF
    nPts = extractHOGHOF(video, dim, maxPts, &dataPts);
    break;
  case 1: //MBH
    nPts = extractMBH(video, dim, maxPts, &dataPts);
    break;
  }
  if(nPts != 0){
    //int nPts = importSTIPs(stip.c_str(),dim,maxPts, &dataPts);
    dataPts.setNPts(nPts);			// set actual number of pts
    dataPts.buildKcTree();			// build filtering structure
    execTime.end = times(&execTime.send);
    std::cout << nPts << " vectors imported!" << std::endl;
    printTime(&execTime);

    std::cout << "Importing centers..."  << std::endl;
    execTime.begin = times(&execTime.sbegin);
    KMfilterCenters ctrs(k, dataPts);		// allocate centers

    std::string means(folder + "/training.means");
    importCenters(means.c_str(), dim, k, &ctrs);
    execTime.end = times(&execTime.send);
    printTime(&execTime);

    std::cout << "Creating the BOW histogram..."  << std::endl;
    execTime.begin = times(&execTime.sbegin);
    struct svm_problem svmProblem = computeBOW(0,dataPts,ctrs);
    execTime.end = times(&execTime.send);
    printTime(&execTime);

    std::cout << "Importing the SVM model..."  << std::endl;
    std::string model(folder + "/svm.model");
    struct svm_model* pSvmModel = svm_load_model(model.c_str());

    std::cout << "Predicting activity..." << std::endl;
    execTime.begin = times(&execTime.sbegin);
    int label = svm_predict(pSvmModel,svmProblem.x[0]);

    cout << "Label predicted: " << label << endl;
    execTime.end = times(&execTime.send);
    printTime(&execTime);

    svm_free_and_destroy_model(&pSvmModel);

    activitiesMap* am;
    int nbActivities = mapActivities(folder,&am);

    int i = 0;
    while(i<nbActivities && am[i].label != label){
      i++;
    }
    std::string activity (am[i].activity);
    delete[] am;
    return activity;
  }
  return "Nothing";
}
void printTime(exec_time *tmps){
  int top = sysconf(_SC_CLK_TCK); // number of tips per seconds
  std::cout << "[" << (tmps->end - tmps->begin)/top << "s]" <<std::endl;
}
