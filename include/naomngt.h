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
using namespace std;

int nbOfFiles(std::string path);
bool fileExist(std::string file, std::string folder);

void addVideos(std::string bddName,std::string activity,int nbVideos, std::string* videoPaths, int maxPts);
std::string inttostring(int int2str);
void trainBdd(std::string bddName, int maxPts, int k);
void addLabel(int label, std::string file, int k);

void addBdd(std::string bddName,int desc);
void addActivity(std::string activityName, std::string bddName);

void deleteBdd(std::string bddName);
void deleteActivity(std::string activityName, std::string bddName);

void emptyFolder(std::string folder);
void refreshBdd(std::string bddName, int dim, int maxPts);
void predictActivity(std::string videoPath, std::string bddName, int maxPts);

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
#endif
