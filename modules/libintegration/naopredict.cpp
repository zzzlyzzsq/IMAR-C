/** \author Fabien ROUALDES (institut Mines-Télécom)
 *  \file  main.c
 *  \date 26/06/2013
 *
 *  Program permiting to use kmeans and svm without writing on hard drive and linked to dynamic libraries
 */
#include "naopredict.h"

using namespace std;

std::string integration(std::string video, std::string modelFolder){
  std::string means(modelFolder + "/training.means");
  std::string model(modelFolder + "/svm.model");
  exec_time execTime;

  /* KMeans BOWing	*/
  int k = 200;
  int desc = getDesc(modelFolder.c_str());
  int dim = getDim(desc); // STIPs dimension
  int maxPts = 500000; // max number of data points

  qiLogInfo("Tactil") << "Extracting the feature points..." << std::endl;
  execTime.begin = times(&execTime.sbegin);
  KMdata dataPts(dim,maxPts);
  int nPts;
  switch(desc){
    case 0: //HOG HOF
        qiLogInfo("Tactil") << "(HOG HOF selected)" << std::endl;
    nPts = extractHOGHOF(video, dim, maxPts, &dataPts);
    break;
   case 1: //MBH
          qiLogInfo("Tactil") << "(MBH selected)" << std::endl;
       nPts = extractMBH(video, dim, maxPts, &dataPts);
      break;
    }
  if(nPts > 0){
  dataPts.setNPts(nPts);			// set actual number of pts
  dataPts.buildKcTree();			// build filtering structure
  execTime.end = times(&execTime.send);
  qiLogInfo("Tactil") << nPts << " vectors imported!" << std::endl;
  printTime(&execTime);

  qiLogInfo("Tactil") << "Importing centers..."  << std::endl;
  execTime.begin = times(&execTime.sbegin);
  KMfilterCenters ctrs(k, dataPts);		// allocate centers
  importCenters(means.c_str(), dim, k, &ctrs);
  execTime.end = times(&execTime.send);
  printTime(&execTime);

  qiLogInfo("Tactil") << "Creating te BOW histogram..."  << std::endl;
  execTime.begin = times(&execTime.sbegin);
  struct svm_problem svmProblem = computeBOW(0, dataPts, ctrs);
  
  qiLogInfo("Tactil") << "Importing the SVM model..."  << std::endl;
  struct svm_model* pSvmModel = svm_load_model(model.c_str());
  
  qiLogInfo("Tactil") << "Predicting activity..." << std::endl;
  execTime.begin = times(&execTime.sbegin);
  int label = svm_predict(pSvmModel,svmProblem.x[0]);
  qiLogInfo("Tactil") << "Label predicted: " << label << endl;
  execTime.end = times(&execTime.send);
  printTime(&execTime);

  svm_free_and_destroy_model(&pSvmModel);

  activitiesMap* am;
  int nbActivities = mapActivities("/home/nao/data/activity_recognition",&am);

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
  qiLogInfo("Tactil") << "[" << (tmps->end - tmps->begin)/top << "s]" <<std::endl;
}
