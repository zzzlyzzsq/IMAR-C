/**
 * \function main.cpp
 * \author HUILONG He (Télécom SudParis)
 * \author ROUALDES Fabien (Télécom SudParis)
 * \date 25/07/2013
 * \brief Create and modify BDDs of activity recognition.
 */
#include <stdlib.h>
#include <string>
#include "naomngt.h"
#include <fstream>

using namespace std;

void help();

int main(int argc, char* argv[]){
  int dim,k;
  
  if(argc<3){
    help();
    return EXIT_FAILURE;
  }
  
  std::string function = argv[1];
  if(function.compare("test") == 0){ // test BDD
    if(argc != 4){
      std::cerr << "Test: Bad arguments!" << std::endl;
      return EXIT_FAILURE;
    }
    char* bddName = argv[2];
    k = atoi(argv[3]);
    testBdd(bddName, k, 100);    
  }
  else if(function.compare("refresh") == 0){ // delete all files but not videos and recompute stips
    if(argc == 4){
      std::string desc = argv[3];
      refreshBdd(argv[2],desc);
    }
    else{
      std::cerr << "Refresh: Bad arguments!" << std::endl;
      return EXIT_FAILURE;
    }
  }
  else if(function.compare("list") == 0){ // It permits to inform user
    if(argc == 3)
      listBdds();
    else if(argc == 4)
      listActivities(argv[3]);
    else
      std::cerr << "list: Bad arguments!" << std::endl;
  } 
  else if(function.compare("add") == 0){ // in order to add video + stips in db
    std::string subfunction(argv[2]);
    if(argc == 5 && (subfunction.compare("bdd") == 0)){
      // add bdd
      std::string bddName(argv[3]);
      std::string desc = argv[4];
      addBdd(bddName,desc);
      // saveDescInfo(bddName,desc);
    }
    else if(argc == 5 && subfunction.compare("activity") == 0){
      // add activity
      std::string activityName(argv[3]);
      std::string bddName(argv[4]);
      addActivity(activityName,argv[4]);
    }
    else if(argc >= 6 && subfunction.compare("videos") == 0){
      // add video
      int nbVideos = argc - 5;
      if(nbVideos == 0){
	std::cerr << "There is no path to video!" << std::endl;
	return EXIT_FAILURE;
      }
      std::string videoPaths[nbVideos];
      int j = 0;
      for(int i = 3; i < argc - 2;i++){
	videoPaths[j] = argv[i];
	j++;
      }
      std::string bddName = argv[argc-2];
      std::string activity = argv[argc-1];
      //std::string desc = getDescID(bddName);
      addVideos(bddName,activity,nbVideos,videoPaths);
    }
    else{
      std::cerr << "add: Bad arguments!" << std::endl;
      return EXIT_FAILURE;
    }
  }
  else if(function.compare("compute") == 0){ // in order to add video + stips in db
    if(argc != 4){
      std::cerr << "compute: bad arguments!" << std::endl;
      return EXIT_FAILURE;
    }
    char* bddName = argv[2];
    k = atoi(argv[3]);
    trainBdd(bddName, k);
  }
  else if(function.compare("delete") == 0){ 
    std::string todelete(argv[2]);
    if(argc == 5 && todelete.compare("activity") == 0){
      deleteActivity(argv[3],argv[4]);
    }
    else if (argc == 4 && todelete.compare("bdd") == 0){
      deleteBdd(argv[3]);
    }
    else{
      std::cerr << "Delete: bad arguments!" << std::endl;
      return EXIT_FAILURE;
    }
  }
#ifdef TRANSFER_TO_ROBOT_NAO
  else if(function.compare("transfer") == 0){
    if(argc != 6){
      std::cerr << "Transfer: bad arguments!" << std::endl;
      return EXIT_FAILURE;
    }
    std::string bddName = argv[2];
    std::string login = argv[3];
    std::string robotIP = argv[4];
    std::string password = argv[5];
    transferBdd(bddName,login,robotIP,password);
  }
#endif // TRANSFER_TO_ROBOT_NAO
  else if(function.compare("ar") == 0){
    if(argc != 4){
      std::cerr << "Activity recognition: bad arguments!" << std::endl;
      return EXIT_FAILURE;
    }
    std::string videoPath = argv[2];
    std::string bddName = argv[3];
    std::cout << bddName << std::endl;
    predictActivity(videoPath,bddName);
  }
  return EXIT_SUCCESS;
}
void help(){
  std::cout <<"Lister les activités ou BDDs préexistantes :"
	    << "\t ./naomngt list activities <bdd_name>"
	    << "\t ./naomngt list bdds" << std::endl;

  std::cout << "Ajouter des vidéos / créer une nouvelle bdd / créer une nouvelle activité : " << std::endl;
  std::cout << "\t ./naomngt add /path/to/video1.avi ... /path/to/video_n.avi <bdd_name> <activity_name>" << std::endl;
  std::cout << "\t ./naomngt add activity <activity_name> <bdd_name>" << std::endl;
  std::cout << "\t ./naomngt add bdd <bdd_name> <descriptor type>" << std::endl;

  std::cout << "Effectuer les algorithmes d'apprentissage :" << std::endl;
  std::cout << "\t ./naomngt compute <bdd_name> <nr_centers>" << std::endl;
  
  std::cout << "Suppression de BDD / activités :" << std::endl;
  std::cout << "\t ./naomngt delete activity <activity_name> <bdd_name>" << std::endl;
  std::cout << "\t ./naomngt delete bdd <bdd_name>" << std::endl;
  
  std::cout << "Rafraichir une base de données" << std::endl;
  std::cout << "(supression de toutes les données sauf les vidéos et réextraction des STIPs)" << std::endl;
  std::cout << "\t ./naomngt refresh <bdd_name> <descriptor type>" << std::endl;
	
  std::cout << "Descriptor type:" << std::endl;
  std::cout << "\t hoghof : HOG and HOF" << std::endl;
  std::cout << "\t mbh : MBHx and MBHy" << std::endl;
  std::cout << "\t all : HOG, HOF, MBHx and MBHy" << std::endl;
}
