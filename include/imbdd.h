#ifndef _IMBDD_H_
#define _IMBDD_H_

#include <iostream>
#include <string>

#include "tinyxml.h"

class IMbdd{
 private:
  std::string bddName;
  
  // DenseTrack
  int scale_num;
  std::string descriptor;
  int dim;

  // KMeans
  int maxPts;
  std::string km_algorithm;
  int k;
  std::string meansFile;
  
  // SVM
  std::string normalization;
  std::string model;
  
 public:
  IMbdd(){};
  IMbdd(std::string bddName);
  ~IMbdd(){};
  void write_bdd_configuration(const char* pFilename);
  void load_bdd_configuration(const char* pFilename);
  void show_bdd_configuration();
  
  std::string getName(){return bddName;};
  void saveName(std::string bddName);
  std::string getDescriptor(){return descriptor;};
  std::string getKMAlgorithm(){return km_algorithm;};
  int getK(){return k;}
  int getDim(){return dim;};
  int getMaxPts(){return maxPts;};
  std::string getMeansFile(){return meansFile;};
  std::string getNormalization(){return normalization;};
  std::string getModel(){return model;};
  
  void changeDenseTrackSettings(int scale_num,
				std::string descriptor,
				int dim);
  void changeKMSettings(std::string algorithm,
			int k,
			std::string meansFile);
  void changeSVMSettings(std::string normalization);  
};

#endif // _IMBDD_H_
  
