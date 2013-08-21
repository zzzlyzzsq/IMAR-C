#ifndef _IMBDD_H_
#define _IMBDD_H_

#include <iostream>
#include <string>
#include "tinyxml.h"

class IMbdd{
 private:
  std::string bddName;
  std::string folder;
  
  // DenseTrack
  int scale_num;
  std::string descriptor;
  int dim;

  // KMeans
  int maxPts;
  std::string km_algorithm;
  int k;
  std::string KMeansFile;

  // Normalization
  std::string normalization;
  std::string meansFile;
  std::string standardDeviationFile;
  
  // SVM
    std::string model;
  
 public:
  IMbdd(){};
  IMbdd(std::string bddName);
  ~IMbdd(){};
  void write_bdd_configuration(std::string pFolder, std::string pFilename);
  void load_bdd_configuration(std::string pFolder, std::string pFilename);
  void show_bdd_configuration();
  
  std::string getName() const {return bddName;};
  std::string getFolder() const {return folder;};
  void saveName(std::string bddName);
  int getScaleNum() const {return scale_num;};
  std::string getDescriptor() const {return descriptor;};
  std::string getKMAlgorithm() const {return km_algorithm;};
  int getK() const {return k;}
  int getDim() const {return dim;};
  int getMaxPts() const {return maxPts;};
  std::string getKMeansFile() const {return KMeansFile;};
  std::string getNormalization() const {return normalization;};
  std::string getMeansFile() const {return meansFile;};
  std::string getStandardDeviationFile() const {return standardDeviationFile;};
  std::string getModel() const {return model;};
  
  void changeDenseTrackSettings(int scale_num,
				std::string descriptor,
				int dim);
  void changeKMSettings(std::string algorithm,
			int k,
			std::string KMeansFile);
  void changeNormalizationSettings(std::string normalization,
				   std::string meansFile,
				   std::string standardDeviationFile);  
  void changeSVMSettings(std::string model);
};

#endif // _IMBDD_H_
