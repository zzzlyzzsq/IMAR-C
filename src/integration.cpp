/** @author Fabien ROUALDES (institut Mines-Télécom)
 *  @file  main.c
 *  @date 26/06/2013
 *  Program permiting to use kmeans and svm without writing on hard drive and linked to dynamic libraries
 */
#include "integration.h"

using namespace std;

std::string integration(std::string video, std::string means, std::string model){
  exec_time execTime;
 /* if(argc < 4){
    cerr << "Usage: ./main <file to video> <kmeans center> <svm model>" << endl;
    exit(EXIT_FAILURE);
  }*/
  // FILES
  //std::string video(argv[1]);
  //std::string stip(argv[1]);
 // std::string means(argv[2]);
  //std::string model(argv[3]);

  /* KMeans BOWing	*/
  int k = 200;
  int dim = 192; // STIPs dimension
  int maxPts = 500000; // max number of data points

  std::cout << "Importing the STIPs..." << std::endl;
  execTime.begin = times(&execTime.sbegin);
  KMdata dataPts(dim,maxPts);
  int nPts =  extractMBH(video.c_str(), dim, maxPts, &dataPts);
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
  importCenters(means.c_str(), dim, k, &ctrs);
  execTime.end = times(&execTime.send);
  printTime(&execTime);

  std::cout << "Creating te BOW histogram..."  << std::endl;
  execTime.begin = times(&execTime.sbegin);
  KMctrIdxArray closeCtr = new KMctrIdx[dataPts.getNPts()]; // dataPts = 1 label
  double* sqDist = new double[dataPts.getNPts()];
  ctrs.getAssignments(closeCtr, sqDist);

  // Initializing the BOW histogram
  float* bowHistogram = NULL;
  bowHistogram = new float[k];
  for(int centre = 0; centre<k; centre++)
    bowHistogram[centre]=0;
  // remplissage de l'histogramme
  for(int point = 0; point < nPts ; point++){
    bowHistogram[closeCtr[point]]++;
  }
  for(int centre = 0 ; centre < k ; centre++){
    bowHistogram[centre] /= nPts;
  }
  delete closeCtr;
  delete[] sqDist;
  execTime.end = times(&execTime.send);
  printTime(&execTime);

  std::cout << "Exporting the BOW histogram in the svm_problem structure..."  << std::endl;
  struct svm_problem svmProblem;
  int l = 1;
  svmProblem.l = l;
  svmProblem.y = (double*) malloc(svmProblem.l * sizeof(double));
  svmProblem.x = (struct svm_node **) malloc(svmProblem.l * sizeof(struct svm_node *));

  int label;
  int idActivity = 0;
  while(idActivity < l){
    int notZero = 0;
    int center = 0;
    while(center < k){
      if(bowHistogram[center] != 0){
        notZero++;
      }
      center++;
    }
    int i = 0;
    svmProblem.x[idActivity] = (svm_node *) malloc((notZero + 1) * sizeof(svm_node));
    center = 0;
    while(center < k){
      if(bowHistogram[center] != .0){
        svmProblem.x[idActivity][i].index = center + 1;
        svmProblem.x[idActivity][i].value = bowHistogram[center];
        i++;
      }
      center++;
    }
    svmProblem.x[idActivity][(notZero-1)+1].index = -1;
    // it is the end of the table,
    // we do not need to add a value
    idActivity++;
  }

  std::cout << "Importing the SVM model..."  << std::endl;
  struct svm_model* pSvmModel = svm_load_model(model.c_str());

  std::cout << "Predicting activity..." << std::endl;
  execTime.begin = times(&execTime.sbegin);
  label = svm_predict(pSvmModel,svmProblem.x[0]);
  cout << "Label predicted: " << label << endl;
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
  std::cout << "[" << (tmps->end - tmps->begin)/top << "s]" <<std::endl;
}
