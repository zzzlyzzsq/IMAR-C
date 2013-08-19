/**
 * \file naomngt.h
 * \author Fabien ROUALDES (Institut Mines-Télécom)
 * \date 17/07/2013
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
void testBdd(std::string bddName, int k, int nrTests);

void addBdd(std::string bddName, std::string descriptor);
void addActivity(std::string activityName, std::string bddName);

void deleteBdd(std::string bddName);
void deleteActivity(std::string activityName, std::string bddName);

void emptyFolder(std::string folder);
void refreshBdd(std::string bddName, std::string descriptor);
void predictActivity(std::string videoPath, std::string bddName);

#ifdef TRANSFER_TO_ROBOT_NAO
void transferBdd(std::string bddName, std::string login, std::string robotIP, std::string password);
#endif


void concatenate_features_points(int nbActivities,
				 activitiesMap *am,
				 std::string path2bdd,
				 int nrVideosByActivities,
				 std::vector <std::string>& trainingFiles,
				 std::vector <std::string>& testingFiles);
void concatenate_features_points_per_activities(int nbActivities,
						activitiesMap *am,
						std::string path2bdd,
						int nrVideosByActivities,
						std::vector <std::string>& trainingFiles,
						std::vector <std::string>& testingFiles);
void concatenate_bag_of_words(int nbActivities,
			      activitiesMap *am,
			      std::string path2bdd);
int getMinNumVideo(int nbActivities, activitiesMap *am, std::string path2bdd);
int create_specifics_training_means(std::string path2bdd,
				    int dim,
				    int maxPts,
				    int subK,
				    int nr_class,
				    activitiesMap* am,
				    // std::vector <std::string> rejects,
				    std::string meansFile);
void km_svm_train(int nrVideosByActivities,
		  int dim, int maxPts, int k,
		  int nbActivities, activitiesMap *am,
		  std::string path2bdd, std::string meansFile,
		  MatrixC& trainMC, MatrixC& testMC
		  );
#endif
