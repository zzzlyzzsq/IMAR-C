/**
 * \file naomngt.h
 * \author Fabien ROUALDES
 * \author Huilong HE
 * \date 20/08/2013
 */
#ifndef _NAOMNGT_H_
#define _NAOMNGT_H_ 

#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h> // mkdir 
#include <sys/types.h> // mkdir
#include <iostream>
#include <fstream> // ifstream
#include <sstream>
#include <vector>
#include <string>
#include <utility> // make_pair
#include <map>
#include <algorithm> // tri dans l'odre croissant

#include <ftplib.h> // ftp transfer

#include "naokmeans.h"
#include "naosvm.h"
#include "naodensetrack.h"
#include "imconfig.h"
#include "imbdd.h"

using namespace std;

int nbOfFiles(std::string path);
bool fileExist(std::string file, std::string folder);

void addVideos(std::string bddName,std::string activity,int nbVideos, std::string* videoPaths);
std::string inttostring(int int2str);
void trainBdd(std::string bddName, int k);
void addLabel(int label, std::string file, int k);
void testBdd(std::string bddName,
	     int k,
	     int nrTests);

void addBdd(std::string bddName, int scale_num, std::string descriptor);
void addActivity(std::string activityName, std::string bddName);

void deleteBdd(std::string bddName);
void deleteActivity(std::string activityName, std::string bddName);

void emptyFolder(std::string folder);
void im_refresh_bdd(std::string bddName,
		    int scale_num,
		    std::string descriptor);
void im_refresh_folder(const IMbdd& bdd, std::string folder);

void predictActivity(std::string videoPath, std::string bddName);

#ifdef TRANSFER_TO_ROBOT_NAO
void transferBdd(std::string bddName, std::string login, std::string robotIP, std::string password);
#endif

void im_concatenate_bdd_feature_points(std::string path2bdd,
				       std::vector<std::string> people,
				       std::vector<std::string> activities);
void im_concatenate_folder_feature_points(std::string folder,
					  std::vector<std::string> activities);
int im_create_specifics_training_means(IMbdd bdd,
				       const std::vector<std::string>& trainingPeople 
				       //std::vector <std::string> rejects
				       );
void im_leave_one_out(std::string bddName, 
		      int k);
double im_training_leave_one_out(const IMbdd& bdd,
				 const std::vector<std::string>& trainingPeople,
				 const std::map <std::string, struct svm_problem>& peopleBOW,
				 int& minC, int& maxC,
				 int& minG, int& maxG,
				 struct svm_parameter& svmParameter);
double im_svm_train(IMbdd& bdd,
		    const std::vector<std::string>& trainingPeople,
		    MatrixC& trainMC,
		    const std::vector<std::string>& testingPeople,
		    MatrixC& testMC);

void im_train_bdd(std::string bddName, int k);
void im_compute_bdd_bow(const IMbdd& bdd, 
			std::map <std::string, struct svm_problem>& peopleBOW);
void im_normalize_bdd_bow(const IMbdd& bdd,
			  const std::vector<std::string>& trainingPeople,
			  std::map<std::string, struct svm_problem>& peopleBOW);
void bow_normalization(const IMbdd& bdd, struct svm_problem& svmProblem);
void im_fill_confusion_matrix(const IMbdd& bdd,
			      const svm_problem& svmProblem,
			      struct svm_model** svmModels,
			      MatrixC& MC);
#endif
