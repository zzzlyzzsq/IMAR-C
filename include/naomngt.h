/**
 * @file naomngt.h
 * @author Fabien ROUALDES (Institut Mines-Télécom)
 * @date 17/07/2013
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

#include "naokmeans.h"
#include "naosvm.h"
#include "naodensetrack.h"

using namespace std;

/** \class activitiesMap
 * \brief Correspondance between a label and an activity
 */
class activitiesMap{
 public:
  int label;
  std::string activity;
};

void listBdds();
void listActivities(std::string bdd);
int mapActivities(std::string path2bdd, activitiesMap** am);
int nbOfFiles(std::string path);
bool fileExist(std::string file, std::string folder);

void addVideos(std::string bddName,std::string activity,int nbVideos, std::string* videoPaths, int dim, int maxPts);
std::string inttostring(int int2str);
void trainBdd(std::string bddName, int dim, int maxPts, int k);
void addLabel(int label, std::string file, int k);

void addBdd(std::string bddName);
void addActivity(std::string activityName, std::string bddName);

void deleteBdd(std::string bddName);
void deleteActivity(std::string activityName, std::string bddName);

void emptyFolder(std::string folder);
void refreshBdd(std::string bddName, int dim, int maxPts);
#endif
