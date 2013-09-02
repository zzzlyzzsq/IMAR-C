/**
 * \file naomngt.cpp
 * \brief Set of functions permiting to manage the activity recognition BDD of Bag Of Words.
 * \author ROUALDES Fabien (Télécom SudParis)
 * \author HUILONG He (Télécom SudParis)
 * \date 25/07/2013
 */
#include "naomngt.h"

/**
 * \fn int nbOfFiles(std::string path)
 * \brief Counts the number of files in a folder.
 * \param[in] path The path to the folder.
 * \return The number of files.
 */
int nbOfFiles(std::string path){
  int nbFiles = 0; 
  DIR * repertoire = opendir(path.c_str());
  if(!repertoire) std::cerr << "Impossible to open the directory!" << std::endl;
  else{
    struct dirent * ent;
    while ( (ent = readdir(repertoire)) != NULL){
      std::string file = ent->d_name;
      if(file.compare(".") != 0 && file.compare("..") != 0) nbFiles++;
    }
    closedir(repertoire);
  }
  return nbFiles;
}

/**
 * \fn bool fileExist(std::string file, std::string folder)
 * \brief Checks if the file name does not exist.
 * \param[in] file The path to the file.
 * \param[in] folder The path to the folder.
 * \return True or false.
 */
bool fileExist(std::string file, std::string folder){
  DIR * repertory = opendir(folder.c_str());
  if (repertory == NULL) std::cerr << "Impossible to open the directory!" << std::endl;
  else{
    struct dirent * ent;
    while ( (ent = readdir(repertory)) != NULL) if(((std::string)ent->d_name).compare(ent->d_name) == 0) return true;
    closedir(repertory);
  }
  return false;
}

/**
 * \fn void addVideos(std::string bddName, std::string activity, int nbVideos, std::string* videoPaths, int dim, int maxPts)
 * \brief Adds a new video in the choosen activity of the specified BDD.
 * \param[in] bddName The name of the BDD.
 * \param[in] activity The name of the activity.
 * \param[in] nbVideos The number of videos we want to add.
 * \param[in] videoPaths The different paths to the videos.
 */
void addVideos(std::string bddName, std::string activity, int nbVideos, std::string* videoPaths){
  std::string path2bdd("bdd/" + bddName);
  
  //int desc = getDescID(path2bdd);
  //int dim = getDim(desc);
  
  // Loading the bdd
  IMbdd bdd(bddName,path2bdd);
  bdd.load_bdd_configuration(path2bdd.c_str(),"imconfig.xml");
  
  // Saving its parameters
  int maxPts = bdd.getMaxPts();
  int scale_num = bdd.getScaleNum();
  std::string descriptor = bdd.getDescriptor();
  int dim = bdd.getDim();
    
  // Loading the mapping file to get the video label
  activitiesMap *am;
  int nbActivities = mapActivities(path2bdd,&am);
  int i = 0;
  while(am[i].activity.compare(activity) != 0 && i < nbActivities) i++;
  if(am[i].activity.compare(activity) != 0){
    std::cerr << "Activity not found!\n" << std::endl;
    exit(EXIT_FAILURE);
  }
  int label = am[i].label;
  delete []am;
  
  // Import videos in the selected database
  string strlabel = inttostring(label);
  
  std::string copypath(path2bdd + "/" + strlabel + "/avi");
  int nbFiles = nbOfFiles(copypath);
  int j = nbFiles + 1;
  for(int i=0 ; i<nbVideos ; i++){
    string idFile = inttostring(j);
    string cmd("cp " + videoPaths[i] + " " + copypath + "/" + strlabel + idFile + ".avi");
    system(cmd.c_str());
    j++;
  }
  
  // Extract STIPs from the videos and save them in the repertory /path/to/bdd/label/
  string fpointspath(path2bdd + "/" + strlabel + "/fp");
  j = nbFiles + 1;
  
  for(int i=0 ; i<nbVideos ; i++){
    KMdata dataPts(dim,maxPts);
    string idFile = inttostring(j);
    string videoInput(copypath + "/" + strlabel + idFile + ".avi");
    string fpOutput(fpointspath + "/" + strlabel + "-" + idFile + ".fp");
    int nPts;
    
    nPts = extract_feature_points(videoInput,
				  scale_num, descriptor, dim,
				  maxPts, dataPts);		
    if(nPts != 0){
      dataPts.setNPts(nPts);
      exportSTIPs(fpOutput, dim,dataPts);
    }
    j++;
  }
}

/**
 * \fn std::string inttostring(int int2str)
 * \brief Converts an int into a string.
 * \param[in] int2str The int to convert.
 * \return The string converted.
 */
std::string inttostring(int int2str){
  // créer un flux de sortie
  std::ostringstream oss;
  
  // écrire un nombre dans le flux
  oss << int2str;
  
  // récupérer une chaîne de caractères
  std::string result = oss.str(); 
  return result;
}
/** \fn void addActivity(std::string activityName, std::string bddName)
 * \brief Creates a new activity in the specified BDD.
 *
 * \param[in] activityName The name of the new activity.
 * \param[in] bddName The name of the BDD.
 */
void addActivity(std::string activityName, std::string bddName){/*
  string path2bdd("bdd/" + bddName);
  IMbdd bdd = IMbdd(bddName,path2bdd);
  
  // On récupère les données des activités préexistantes
  bdd.load_bdd_configuration(path2bdd.c_str(),"imconfig.xml");
  std::vector<
  
  // Création du dossier spécifique à l'activité et de ses sous-dossiers
  int idActivity = nbActivities + 1;
  string strID = inttostring(idActivity);
  string activityFolder(path2bdd + "/" + strID);
  string activityAVIFolder(path2bdd + "/" + strID + "/avi");
  string activitySTIPSFolder(path2bdd + "/" + strID + "/fp");
  mkdir(activityFolder.c_str(),S_IRWXU|S_IRGRP|S_IXGRP); // rwx pour user
  mkdir(activityAVIFolder.c_str(),S_IRWXU|S_IRGRP|S_IXGRP); // rwx pour user
  mkdir(activitySTIPSFolder.c_str(),S_IRWXU|S_IRGRP|S_IXGRP); // rwx pour user
  
  // Ajout de l'activité dans le fichier mapping.txt
  string file(path2bdd + "/mapping.txt");
  std::ofstream out(file.c_str(),ios::out | ios::app);
  if(!out){
    std::cerr << "Can't open file mapping.txt!" << std::endl;
    exit(EXIT_FAILURE);
  }
  //  out.seekg(0, ios::end);
  out << idActivity << ":" << activityName << endl;
  out.close();*/
}

/**
 * \fn void deleteActivity(std::string activityName, std::string bddName)
 * \brief Deletes an existant activity in the specified BDD.
 *
 * \param[in] activityName The name of the activity to delete.
 * \param[in] bddName The name of the BDD.
 */
void deleteActivity(std::string activityName, std::string bddName){/*
  string path2bdd("bdd/" + bddName);
  
  // On récupère les données des activités préexistantes
  activitiesMap *am;
  int nbActivities = mapActivities(path2bdd,&am);
  // Checking that the activity exists
  int i = 0;
  bool exist = false;
  int idActivity;
  while(i < nbActivities){
    if(am[i].activity.compare(activityName) == 0){
      idActivity = am[i].label;
      exist = true;
    }
    i++;
  }
  if (!exist){
    std::cout << "The activity does not exist!" << std::endl;
    exit(EXIT_FAILURE);
  }
  std::string strID = inttostring(idActivity);
  // Recursive deletion of the activity file
  std::string activityFolder(path2bdd + "/" + strID);
  std::string activityAVIFolder(path2bdd + "/" + strID + "/avi");
  std::string activitySTIPSFolder(path2bdd + "/" + strID + "/fp");
  // avi
  emptyFolder(activityAVIFolder);
  rmdir(activityAVIFolder.c_str());

  // stips
  emptyFolder(activitySTIPSFolder);
  rmdir(activitySTIPSFolder.c_str());
  // ACTIVITE
  rmdir(activityFolder.c_str());
  
  // Rewriting the file mapping.txt
  string file(path2bdd + "/" + "mapping.txt");
  ofstream out(file.c_str(), ios::out | ios::trunc);  // ouverture en écriture avec effacement du fichier ouvert
  if(!out){
    std::cerr << "Impossible to create the file mapping.txt!" <<std::endl;
    exit(EXIT_FAILURE);
  }
  for(int i=0 ; i<nbActivities ; i++){
    if(am[i].label != idActivity)
      out << am[i].label << ":" << am[i].activity << endl;
  }
  out.close();
  delete []am;*/
}

/**
 * \fn void addBdd(std::string bddName,int desc)
 * \brief Creates a new BDD.
 *
 * \param[in] bddName The name of the BDD we want to create.
 * \param[in] descriptor The descriptor used for extracting the feature points
 */
void addBdd(std::string bddName, int scale_num, std::string descriptor){/*
  // On vérifie que la BDD n'existe pas
  DIR * repertoire = opendir("bdd");
  if (repertoire == NULL){
    std::cerr << "Impossible to open the BDDs directory!" << std::endl;
  }
  else{
    struct dirent * ent;
    while ( (ent = readdir(repertoire)) != NULL){
      std::string file = ent->d_name;
      if (file.compare(bddName) == 0){
	std::cerr << "La base de donnée entrée existe déjà!" << std::endl;
	exit(EXIT_FAILURE);
      }
    }
    closedir(repertoire);
  }
  
  // On créé le dossier contenant la base de donnée
  std::string path2bdd("bdd/" + bddName);
  mkdir(path2bdd.c_str(),S_IRWXU|S_IRGRP|S_IXGRP); // rwx pour user
  
  // création du fichier mapping.txt
  std::string file = (path2bdd+ "/" + "mapping.txt");
  
  ofstream out(file.c_str(), ios::out);  // ouverture en écriture avec effacement du fichier ouvert
  if(!out){
    std::cerr << "Impossible to create the file mapping.txt!" <<std::endl;
    exit(EXIT_FAILURE);
  }
  out.close();
  
  // Saving parameters
  IMbdd bdd = IMbdd(bddName,path2bdd);
  std::vector<std::string> activities;
  std::vector<std::string> people;
  bdd.changeDataSettings(activities,
			 people,
			 "reject");
  bdd.changeDenseTrackSettings(scale_num,
			       descriptor,
			       getDim(descriptor));
  
			       bdd.write_bdd_configuration(path2bdd.c_str(),"imconfig.xml");*/
}
/**
 * \fn void deleteBdd(std::string bddName)
 * \brief Deletes a BDD
 *
 * \param[in] bddName The name of the bdd.
 */
void deleteBdd(std::string bddName){
  string path2bdd("bdd/" + bddName);
  
  // On récupère les données des activités préexistantes
  activitiesMap *am;
  int nbActivities = mapActivities(path2bdd,&am);
  int i = 0;
  while(i < nbActivities){
    deleteActivity(am[i].activity, bddName);
    i++;
  }
  delete []am;
  emptyFolder(path2bdd);
  rmdir(path2bdd.c_str());
}

/**
 * \fn void emptyFolder(std::string folder)
 * \brief Deletes all files present in the folder.
 *
 * \param[in] folder The path to the folder.
 */
void emptyFolder(std::string folder){
  DIR* repertoire = opendir(folder.c_str());
  if (repertoire == NULL){
    std::cerr << "Impossible to open the directory!" << std::endl;
  }
  else{
    struct dirent * ent;
    while ( (ent = readdir(repertoire)) != NULL){
      std::string file = ent->d_name;
      if(file.compare(".") != 0 && file.compare("..") != 0){
	std::string f(folder + "/" + file);
	remove(f.c_str());
      }
    }
    closedir(repertoire);
  }
}

/**
 * \fn void refreshBdd(std::string bddName, int desc, int maxPts)
 * \brief Deletes all files excepted videos and extracts STIPs again.
 *
 * \param[in] bddName The name of the BDD containing videos.
 * \param[in] dim The STIPs dimension.
 * \param[in] maxPts The maximum number of STIPs we can extract.
 */
void im_refresh_bdd(std::string bddName,
		    int scale_num,
		    std::string descriptor){
  std::string path2bdd("bdd/" + bddName);
  IMbdd bdd = IMbdd(bddName,path2bdd);
  bdd.load_bdd_configuration(path2bdd,"imconfig.xml");
  
  // Saving the new new descriptor with its dimension
  int dim = getDim(descriptor);
  bdd.changeDenseTrackSettings(scale_num,
			       descriptor,
			       dim);
  bdd.write_bdd_configuration(path2bdd.c_str(),"imconfig.xml");
  
  // Deleting testing files and training files 
  DIR* repBDD = opendir(path2bdd.c_str());
  if (repBDD == NULL){
    std::cerr << "Impossible top open the BDD directory"<< std::endl;
    exit(EXIT_FAILURE);
  }
  struct dirent *ent;
  while ( (ent = readdir(repBDD)) != NULL){
    std::string file = ent->d_name;
    std::string f = path2bdd + "/" + file;
    if((file.compare("concatenate.fp.test") == 0) || (file.compare("concatenate.fp.train") == 0) ||
       (file.compare("concatenate.bow.test") == 0) || (file.compare("concatenate.bow.train") == 0) || 
       (file.compare("files.test") == 0) || (file.compare("files.train") == 0))
      remove(f.c_str());
  }
  closedir(repBDD);
  
  std::vector<std::string> people = bdd.getPeople();
  // Deleting feature points of each activities 
  for(vector<string>::iterator person = people.begin() ; person != people.end() ; ++person){
    std::string personFolder(path2bdd + "/" + *person);
    im_refresh_folder(bdd, personFolder);
  }
}

/**
 * \fn void im_refresh_folder(std::string folder, std::vector<std::string> activities, int scale_num, int dim, int maxPts)
 * \brief Deletes all files excepted videos and extracts STIPs again.
 *
 * \param[in] folder the path to the folder containing videos.
 * \param[in] activities the vector containing all the activities.
 * \param[in] scale_num the number of scales used for the feature points extraction.
 * \param[in] dim the dimension of the feature points.
 * \param[in] maxPts the maximum number of feature points we can extract.
 */
void im_refresh_folder(const IMbdd& bdd, std::string folder){
  std::vector<std::string> activities = bdd.getActivities();
  int scale_num = bdd.getScaleNum();
  std::string descriptor(bdd.getDescriptor());
  int dim = bdd.getDim();
  int maxPts = bdd.getMaxPts();

  
  // Deleting all features points
  for(std::vector<std::string>::iterator activity = activities.begin() ;
      activity != activities.end() ;
      ++activity){
    string rep(folder + "/" + *activity + "/fp");
    DIR * repertoire = opendir(rep.c_str());
    std::cout << rep << std::endl;
    if (repertoire == NULL){
      std::cerr << "Impossible to open the fp folder for deletion!" << std::endl;
      exit(EXIT_FAILURE);
    }
    struct dirent *ent;
    while ( (ent = readdir(repertoire)) != NULL){
      if(strcmp(ent->d_name,".") != 0 && strcmp(ent->d_name,"..") != 0)
	remove(ent->d_name);
    }
    closedir(repertoire);
  } 
  
  // Extracting feature points for each videos
  for(std::vector<std::string>::iterator activity = activities.begin() ;
      activity != activities.end() ;
      ++activity){
    std::string avipath(folder + "/" + *activity + "/avi");
    std::string FPpath(folder + "/" +  *activity + "/fp");
    DIR * repertoire = opendir(avipath.c_str());
    if (repertoire == NULL){
      std::cerr << "Impossible to open the avi folder for extraction!" << std::endl;
      exit(EXIT_FAILURE);
    }
    struct dirent * ent;
    int j = 1;
    while ( (ent = readdir(repertoire)) != NULL){
      std::string file = ent->d_name;
      if(file.compare(".") != 0 && file.compare("..") != 0){
	string idFile = inttostring(j);
        // Extract feature points from the videos and save them in the repertory /path/to/folder/activity/fp
        KMdata dataPts(dim,maxPts);
        string videoInput(avipath + "/" + file);
        string stipOutput(FPpath + "/" + *activity + "-" + idFile + ".fp");
	int nPts;
	nPts = extract_feature_points(videoInput,
				      scale_num, descriptor, dim,
				      maxPts, dataPts);		
	if(nPts != 0){
	  dataPts.setNPts(nPts);
	  exportSTIPs(stipOutput, dim, dataPts);
	}
	j++;	
      }
    }
    closedir(repertoire);
    // The extraction of the videos feature points of the activity is terminated.
  }
  
  im_concatenate_bdd_feature_points(bdd.getFolder(),
				    bdd.getPeople(),
				    bdd.getActivities());
}

/**
 * \fn void predictActivity(std::string, std::string bddName, int maxPts)
 * \brief Predict the activity done in a video with an existant trained BDD.
 *
 * \param[in] The path to the video to predict.
 * \param[in] bddName The name of the BDD containing videos.
 * \param[in] maxPts The maximum number of STIPs we can extract.
 * \return The name of the activity.
 */
void predictActivity(std::string videoPath,
		     std::string bddName
		     ){
  std::string path2bdd("bdd/" + bddName);   
  
  // Loading parameters
  IMbdd bdd(bddName,path2bdd);
  bdd.load_bdd_configuration(path2bdd.c_str(),"imconfig.xml");
  int scale_num = bdd.getScaleNum();
  std::string descriptor = bdd.getDescriptor();
  int dim = bdd.getDim();
  int k = bdd.getK();
  int maxPts = bdd.getMaxPts();
  
  //double p = getTrainProbability(path2bdd);
  
  // Computing feature points
  KMdata dataPts(dim,maxPts);
  int nPts = 0;
  nPts = extract_feature_points(videoPath,
				scale_num, descriptor, dim,
				maxPts, dataPts);		
  if(nPts == 0){
    std::cerr << "No activity detected !" << std::endl;
    exit(EXIT_FAILURE);
  }
  std::cout << nPts << " vectors extracted..." << std::endl;
  dataPts.setNPts(nPts);
  dataPts.buildKcTree();
  
  KMfilterCenters ctrs(k, dataPts);  
  importCenters(bdd.getFolder() + "/" + bdd.getKMeansFile(), dim, k, &ctrs);
  std::cout << "KMeans centers imported..." << std::endl;
  
  activitiesMap *am;
  int nbActivities = mapActivities(path2bdd,&am);
  
  
  struct svm_problem svmProblem = computeBOW(0,
					     dataPts,
					     ctrs);
  double means[k], stand_devia[k];
  load_gaussian_parameters(bdd, means, stand_devia);
  // simple, gaussian, both, nothing
  if(bdd.getNormalization().compare("") != 0)
    bow_normalization(bdd,svmProblem);
  std::cout << "Bag of words normalized..." << std::endl;
  
  struct svm_model** pSVMModels = new svm_model*[nbActivities];
  std::vector<std::string> modelFiles(bdd.getModelFiles());
  int i=0;
  for (std::vector<std::string>::iterator it = modelFiles.begin() ; it != modelFiles.end() ; ++it){
    pSVMModels[i]= svm_load_model((*it).c_str());
    i++;
  }
  std::cout << "SVM models imported..." << std::endl;
  double probs[nbActivities];
  double label = svm_predict_ovr_probs(pSVMModels,
				       svmProblem.x[0],
				       nbActivities,
				       probs,
				       2);
  std::cerr<<"Probs: ";
  for(int j=0 ; j<nbActivities ; j++){
    std::cout << setw(2) << setiosflags(ios::fixed) << probs[j]*100<<" "; 
  }
  
  destroy_svm_problem(svmProblem);
  int index = searchMapIndex(label, am, nbActivities);
  std::cout << "Activity predicted: ";
  std::cout << am[index].activity << "(" << am[index].label << ")";
  std::cout << std::endl;
  
  for(int m=0 ; m<nbActivities ; m++){
    svm_free_and_destroy_model(&pSVMModels[m]);
  }
}

#ifdef TRANSFER_TO_ROBOT_NAO
/**
 * \fn void transferBdd(std::string bddName, std::string login, std::string roboIP, std::string password)
 * \brief Transfers the file svm.model, training.means and mapping.txt on the robot Nao.
 * 
 * \param[in] bddName The name of te BDD to transfer.
 * \param[in] login Your user name on the robot.
 * \param[in] robotIP The IP adress of the robot.
 * \param[in] password The password of the user on the robot.
 *
 * Training files are sent on the robot Nao via ftp so you have
 * to precise your username on the robot and yout password. 
 *
 * The server ftp must be configured with the following folders:
 * "/data" and "/data/activity_recognition".
 * 
 * Then the program uses the library ftplib to send the training files.
 * It sends mapping.txt, training.means and svm.model which are present 
 * in the folder "./bdd/<bdd_name>".
 *
 */
void transferBdd(std::string bddName, std::string login, std::string robotIP, std::string password){
  /*
  netbuf* nControl = NULL;
  
  if(FtpConnect(robotIP.c_str(), &nControl) != 1){
    perror("Impossible to connect to the ftp server!\n");
    exit(EXIT_FAILURE);
  }
  if(FtpLogin(login.c_str(),password.c_str(),nControl) != 1){
    perror("Impossible to log to the ftp server!\n");
    exit(EXIT_FAILURE);
  }
  
  std::string localFolder(bdd.getFolder());
  std::string remoteFolder("/data/activity_recognition");
  
  std::string localFile(localFolder + "/" + bdd.getKMeansFile());
  std::string remoteFile(remoteFolder + "/" + bdd.getKMeansFile());
  FtpPut(localFile.c_str(),remoteFile.c_str(),FTPLIB_ASCII,nControl); // must be equal to 1
  
  // Mapping label <-> activity
  localFile = localFolder + "/" + bdd.getMappingFile();
  remoteFile = remoteFolder + "/" + bdd.getMappingFile();
  FtpPut(localFile.c_str(),remoteFile.c_str(),FTPLIB_ASCII,nControl); // must be equal to 1
  
  // KMEANS
  // KMeans centers
  localFile = localFolder + "/" + bdd.getKMeansFile();
  remoteFile = remoteFolder + "/" + bdd.getMappingFile();
  std::string remoteFile(remoteFolder + "/" + bdd.getKMeansFile());
  
  // NORMALIZATION
  // Means
  localFile = localFolder + "/" + bdd.getMeansFile();
  remoteFile = remoteFolder + "/" + bdd.getMeansFile();
  FtpPut(localFile.c_str(),remoteFile.c_str(),FTPLIB_ASCII,nControl); // must be equal to 1
  // Standard Deviation
  localFile = localFolder + "/" + bdd.getStandradDeviationFile();
  remoteFile = remoteFolder + "/" + bdd.getStandardDeviationFile();
  FtpPut(localFile.c_str(),remoteFile.c_str(),FTPLIB_ASCII,nControl); // must be equal to 1

  
  
  FtpQuit(nControl); */
}
#endif // TRANSFER_TO_ROBOT_NAO
void im_concatenate_bdd_feature_points(std::string path2bdd,
				       std::vector<std::string> people,
				       std::vector<std::string> activities){
  for(std::vector<std::string>::iterator person = people.begin() ;
      person != people.end() ;
      ++person)
    im_concatenate_folder_feature_points(path2bdd + "/" + *person, activities);  
}

// Concatenate the feature points of each activities of a folder
void im_concatenate_folder_feature_points(std::string folder,
					  std::vector<std::string> activities){
  // First we delete the file if it exists
  for(std::vector<std::string>::iterator activity = activities.begin() ; activity != activities.end() ; ++activity){
    std::string rep(folder + "/" + *activity);
    std::string path2concatenate(rep + "/concatenate." + *activity + ".fp");
    // We open the directory folder/label
    DIR * folder = opendir(rep.c_str());
    if (folder == NULL){
      std::cerr << "Impossible to open the folder for deletion!"<< std::endl;
      exit(EXIT_FAILURE);
    }
    struct dirent *ent;
    while ( (ent = readdir(folder)) != NULL){
      std::string file(ent->d_name);
      if(file.compare("concatenate." + *activity + ".fp") == 0)
	remove(path2concatenate.c_str());
    }
    closedir(folder);
  }
  
  // Then we concatenate the feature points per activities (for KMeans)
  for(std::vector<std::string>::iterator activity = activities.begin() ;
      activity != activities.end() ;
      ++activity){
    string rep(folder + "/" + *activity + "/fp");
    DIR * directory = opendir(rep.c_str());
    if (directory == NULL){
      std::cerr << "Impossible to open the feature points folder for concatenation!" << std::endl;
      exit(EXIT_FAILURE);
    }
    std::string activityOutPath(folder + "/" + *activity + "/concatenate." + *activity + ".fp");
    ofstream activityOut(activityOutPath.c_str());
    struct dirent * ent;
    while ( (ent = readdir(directory)) != NULL){
      std::string file = ent->d_name;
      if(file.compare(".") != 0 && file.compare("..") != 0){
	std::string path2fp(rep + "/" + file);
	ifstream in(path2fp.c_str());
	std::string line;
        while (std::getline(in, line)){
	  activityOut << line << std::endl;
	}
      }
    }
    closedir(directory);
  }
}					

int im_create_specifics_training_means(IMbdd bdd,
				       const std::vector<std::string>& trainingPeople 
				       //std::vector <std::string> rejects
				       ){
  std::string path2bdd(bdd.getFolder());
  int dim = bdd.getDim();
  int maxPts = bdd.getMaxPts();
  
  std::vector <std::string> activities = bdd.getActivities();
  int nr_class = activities.size();
  
  int nr_people = trainingPeople.size();
  
  // The total number of centers
  int k = bdd.getK();
  int subK = k/nr_class;
  if(k%nr_class != 0){
    std::cerr << "K is no divisible by the number of activities !!" << std::endl;
    exit(EXIT_FAILURE);
  }
  double ***vDataPts = new double**[nr_class];
  int nrFP[nr_class]; // number of feature points for each class
  int currentActivity = 0;
  // For each activity
  for(std::vector<std::string>::iterator activity = activities.begin() ;
      activity != activities.end() ;
      ++activity){
    nrFP[currentActivity] = 0;
    
    // We concatenate all the training people
    int nrFPpP[nr_people]; // number of feature points per person
    double*** activityDataPts = new double**[nr_people];
    int currentPerson = 0;
    for(std::vector<std::string>::const_iterator person = trainingPeople.begin() ;
	person != trainingPeople.end() ;
	++person){
      nrFPpP[currentPerson] = 0;
      std::string rep(path2bdd + "/" + *person + "/" + *activity);
      DIR * repertoire = opendir(rep.c_str());
      if (!repertoire){
	std::cerr << "Impossible to open the feature points directory!" << std::endl;
	exit(EXIT_FAILURE);
      }
      
      // Checking that the file concatenate.<activity>.fp exists
      struct dirent * ent = readdir(repertoire);
      std::string file(ent->d_name);
      while (ent && (file.compare("concatenate." + *activity + ".fp")) != 0){
	ent = readdir(repertoire);
	file = ent->d_name;
      }
      if(!ent){
	std::cerr << "No file concatenate.<activity>.fp" << std::endl;
	exit(EXIT_FAILURE);
      }
      
      // Importing the feature points
      std::string path2FP(rep + "/" + file);
      KMdata kmData(dim,maxPts);
      nrFPpP[currentPerson] = importSTIPs(path2FP, dim, maxPts, &kmData);
      if(nrFPpP[currentPerson] != 0){
	activityDataPts[currentPerson] = new double*[nrFPpP[currentPerson]];
	for(int n = 0 ; n<nrFPpP[currentPerson] ; n++){
	  activityDataPts[currentPerson][n] = new double [dim];
	  for(int d = 0 ; d<dim ; d++)
	    activityDataPts[currentPerson][n][d] = kmData[n][d];
	}
      } // else the current person does not participate in this activity
      nrFP[currentActivity] += nrFPpP[currentPerson];
      currentPerson++;
    } // ++person
    
    // Saving people in vDataPts
    vDataPts[currentActivity] = new double*[nrFP[currentActivity]];
    int index=0;
    for(int p=0 ; p<nr_people ; p++){
      for(int fp=0 ; fp<nrFPpP[p] ; fp++){
	vDataPts[currentActivity][index] = new double[dim];
	for(int d=0 ; d<dim ; d++){
	  vDataPts[currentActivity][index][d] = activityDataPts[p][fp][d];
	}
	index++;
      }
    } // index must be equal to nrFP[currentActivity] - 1
    
    // Deleting activityDataPts
    for(int p=0 ; p<nr_people ; p++){
      for(int fp=0 ; fp < nrFPpP[p] ; fp++)
	delete[] activityDataPts[p][fp];
      delete[] activityDataPts[p];
    }
    delete[] activityDataPts;
    currentActivity++;
  } // ++activity
  
  // Total number of feature points
  int ttFP = 0;
  for(int a=0 ; a<nr_class ; a++){
    ttFP += nrFP[a];
  }
  
  // Memory allocation of the centers
  double** vCtrs = new double*[k];
  for(int i=0 ; i<k ; i++){
    vCtrs[i] = new double[dim];
  }
  
  // Doing the KMeans algorithm for each activities
  int ic = 3; // the iteration coefficient (Ivan's algorithm)
  int currCenter = 0;
  for(int i=0 ; i<nr_class ; i++){
    KMdata kmData(dim,nrFP[i]);
    for(int n=0 ; n<nrFP[i]; n++)
      for(int d=0 ; d<dim ; d++)
	kmData[n][d] = vDataPts[i][n][d];
    kmData.setNPts(nrFP[i]);
    kmData.buildKcTree();
    KMfilterCenters kmCtrs(subK,kmData);
    kmIvanAlgorithm(ic, dim, kmData, subK, kmCtrs);
    for(int n=0 ; n<subK ; n++){
      for(int d=0 ; d<dim ; d++){
	vCtrs[currCenter][d] = kmCtrs[n][d];
      }
      currCenter++;
    }
  }
  
  std::cout << "Concatenate KMdata" << std::endl;
  // Concatenate all the KMdata
  /* In reality it is not necessary
     but the objectif KMfilterCenters must be initialized with
     the KMdata */
  KMdata dataPts(dim,ttFP);
  int nPts = 0;
  for (int i=0 ; i<nr_class ; i++){
    for(int n=0 ; n<nrFP[i]; n++){
      for(int d=0 ; d<dim ; d++){
	dataPts[nPts][d] = vDataPts[i][n][d];
      }
      nPts++;
    }
  }
  // Releasing vDataPts
  for(int i=0 ; i<nr_class ; i++){
    for(int n=0 ; n<nrFP[i] ; n++)
      delete [] vDataPts[i][n];
    delete [] vDataPts[i];
  }
  delete[] vDataPts;
  
  dataPts.buildKcTree();
  // Returning the true centers
  KMfilterCenters ctrs(k,dataPts);
  for(int n=0 ; n<k ; n++){
    for(int d=0 ; d<dim ; d++){
      ctrs[n][d] = vCtrs[n][d];
    }
  }
  
  // Releasing vCtrs
  for(int i=0 ; i<k ; i++)
    delete [] vCtrs[i];
  delete[]vCtrs;
  
  exportCenters(bdd.getFolder() + "/" + bdd.getKMeansFile(),
		dim, k, ctrs);
  
  return k;
}

/**
 * \fn void trainBdd(std::string bddName, int dim, int maxPts, int k)
 * \brief Trains the specified BDD.
 *
 * \param[in] bddName The name of the BDD.
 * \param[in] dim The dimension of the STIPs.
 * \param[in] maxPts The maximum number of points we want to compute.
 * \param[in] k The number of cluster (means).
 */
void im_train_bdd(std::string bddName, int k){
  std::string path2bdd("bdd/" + bddName);
  std::string KMeansFile(path2bdd + "/" + "training.means");
  
  //std::cout << path2bdd << std::endl;
  //int desc = getDescID(path2bdd);
  //int dim = getDim(desc);
  // Loading the DenseTrack settings
  
  // Loading BDD
  IMbdd bdd(bddName,path2bdd);
  bdd.load_bdd_configuration(path2bdd.c_str(),"imconfig.xml");
  
  // In im_train_bdd all the people are the training people !!
  std::vector<std::string> trainingPeople = bdd.getPeople();
  
  // Loading activities
  std::vector<std::string> activities = bdd.getActivities();
  int nrActivities = activities.size();
  int labels[nrActivities];
  int index = 0;
  for(std::vector<std::string>::iterator activity = activities.begin();
      activity != activities.end();
      ++activity){
    labels[index] = index + 1;
    index++;
  }
  if(k%nrActivities != 0){
    std::cerr << "k is not divisible by nrActivities !" << std::endl;
    exit(EXIT_FAILURE);
  }
  
  // Saving KMeans settings
  bdd.changeKMSettings("specifical",
		       k,
		       "training.means");
  im_create_specifics_training_means(bdd, trainingPeople);
  
  // SVM train
  MatrixC trainMC = MatrixC(nrActivities,labels);
  std::vector<std::string> testingPeople;
  MatrixC testMC = MatrixC(nrActivities, labels);
  double crossValidationAccuracy = im_svm_train(bdd,
						trainingPeople, trainMC,
						testingPeople, testMC);
  trainMC.calculFrequence();
  trainMC.exportMC(path2bdd,"training_confusion_matrix.txt");
  bdd.write_bdd_configuration(path2bdd.c_str(),"imconfig.xml");
  
  std::cout << "Resume of the training phase:" << std::endl;
  bdd.show_bdd_configuration();
  std::cout << "Train recognition rate:" << std::endl;
  std::cout << "\t cross validation accuracy=" << crossValidationAccuracy << std::endl;
  std::cout << "\t tau_train=" << trainMC.recognitionRate*100 << "%" << std::endl;
  std::cout << "\t total number of train BOWs=" << trainMC.nrTest << std::endl;
}

void im_leave_one_out(std::string bddName,
		      int k){
  std::string path2bdd("bdd/" + bddName);
  std::string KMeansFile(path2bdd + "/" + "training.means");
  
  //  std::cout << path2bdd << std::endl;
  // Loading BDD
  IMbdd bdd(bddName,path2bdd);
  bdd.load_bdd_configuration(path2bdd.c_str(),"imconfig.xml");
  
  // Saving KMeans settings
  bdd.changeKMSettings("specifical", k, "training.means");
  
  // Loading feature points settings
  std::string descriptor = bdd.getDescriptor();
  
  // Loading activities
  std::vector<std::string> activities = bdd.getActivities();
  int nrActivities = activities.size();
  int labels[nrActivities];
  int index = 0;
  for(std::vector<std::string>::iterator activity = activities.begin();
      activity != activities.end();
      ++activity){
    labels[index] = index + 1;
    index++;
  }
  if(k%nrActivities != 0){
    std::cerr << "k is not divisible by nrActivities !" << std::endl;
    exit(EXIT_FAILURE);
  }
  
  MatrixC trainMC = MatrixC(nrActivities,labels);
  MatrixC testMC = MatrixC(nrActivities,labels);
  double crossValidationAccuracy = 0;
  std::vector<std::string> people = bdd.getPeople();
  // Leave One Person Out 
  for(std::vector<std::string>::iterator person = people.begin() ;
      person != people.end();
      ++person){
    std::vector<std::string> testingPeople;
    std::vector<std::string> trainingPeople;
    testingPeople.push_back(*person);
    // Filling trainingPeople
    for(std::vector<std::string>::iterator trainingPerson = people.begin() ;
	trainingPerson != people.end();
	++trainingPerson){
      if((*trainingPerson).compare(*person) != 0)
	trainingPeople.push_back(*trainingPerson);
    }
    std::cout << "Testing" << *person << std::endl;
    im_create_specifics_training_means(bdd, trainingPeople);
    crossValidationAccuracy += im_svm_train(bdd,
					    trainingPeople, trainMC,
					    testingPeople, testMC);
    std::cout << "Test " << *person << " ok!" << std::endl;
  }
  crossValidationAccuracy /= people.size();
  
  trainMC.calculFrequence();
  trainMC.exportMC(path2bdd,"training_confusion_matrix.txt");
  trainMC.output();
  testMC.calculFrequence();
  testMC.exportMC(path2bdd,"testing_confusion_matrix.txt");
  testMC.output();
  
  std::cout << "#######################################" << std::endl;
  std::cout << "######## RESUME OF THE TEST ###########" << std::endl;
  std::cout << "#######################################" << std::endl;
  std::cout << "Number of people: " << people.size() << std::endl;
  std::cout << "Descriptor ID: " << descriptor << std::endl;
  std::cout << "Number of means: " << k << std::endl;
  std::cout << "Train recognition rate:" << std::endl;
  std::cout << "\t cross validation accuracy=" << crossValidationAccuracy << std::endl;
  std::cout << "\t tau_train=" << trainMC.recognitionRate*100 << "%" << std::endl;
  std::cout << "\t total number of train BOWs=" << trainMC.nrTest << std::endl;
  std::cout << "Test recognition rate:" << std::endl;
  std::cout << "\t tau_test=" << testMC.recognitionRate*100 << "%" << std::endl;
  std::cout << "\t total number of test BOWs=" << testMC.nrTest << std::endl;
}
double im_training_leave_one_out(const IMbdd& bdd,
				 const std::vector<std::string>& trainingPeople,
				 const std::map <std::string, struct svm_problem>& peopleBOW,
				 int& minC, int& maxC,
				 int& minG, int& maxG,
				 struct svm_parameter& svmParameter){
  if(trainingPeople.size() < 2){
    std::cerr << "Impossible to make a leave-one-out-cross-validation with less than 2 people!" << std::endl;
    exit(EXIT_FAILURE);
  }
  int nrActivities = bdd.getActivities().size();
  std::string path2bdd(bdd.getFolder());
  int C, gamma;
  int bestC, bestG;
  double bestAccuracy = -1;
  for(C=minC; C<=maxC ; C++){
    for(gamma = minG ; gamma <= maxG ; gamma++){
      svmParameter.C = pow(2,C);
      svmParameter.gamma = pow(2,gamma);
      double XValidationAccuracy;
      int total_correct = 0;
      int nrBOW = 0;
      // For each couple (C,gamma) we do people.size() leave one outs
      for(std::vector<std::string>::const_iterator person = trainingPeople.begin();
	  person != trainingPeople.end();
	  ++person){
	// This person will be the testing person
	
	// We do the training with the others
	struct svm_problem trainingProblem;
	trainingProblem.l = 0;
	trainingProblem.x = NULL;
	trainingProblem.y = NULL;
	for(std::vector<std::string>::const_iterator trainingPerson = trainingPeople.begin();
	    trainingPerson != trainingPeople.end();
	    ++ trainingPerson){
	  // Training with the other person
	  if((*trainingPerson).compare(*person) != 0){
	    // For each person we train the rest and we test the person
	    // Then compute the mean of the cross validation accuracy 
	    for(int i=0 ; i < peopleBOW.at(*trainingPerson).l ; i++){
	      struct svm_node* bow = peopleBOW.at(*trainingPerson).x[i];
	      addBOW(bow, peopleBOW.at(*trainingPerson).y[i],trainingProblem);
	    }
	  }
	}
	struct svm_model** svmModels = svm_train_ovr(&trainingProblem,&svmParameter);
	destroy_svm_problem(trainingProblem);
	
	// Making test
	for(int i=0 ; i<peopleBOW.at(*person).l ; i++){
	  double* probs = new double[nrActivities];
	  double lab_in = peopleBOW.at(*person).y[i];
	  double lab_out = svm_predict_ovr_probs(svmModels,peopleBOW.at(*person).x[i],
						 nrActivities,probs,2);
	  delete []probs;
	  if(lab_in == lab_out)
	    total_correct++;
	  nrBOW++;
	}
	// Releasing svmModels memory
	for(int i=0 ; i<nrActivities ; i++){
	  svm_free_and_destroy_model(&svmModels[i]);
	}
	delete[] svmModels;
      } // leave one out
      if((XValidationAccuracy = total_correct * 1.0 / nrBOW) > bestAccuracy){
	bestAccuracy = XValidationAccuracy;
	bestC = C;
	bestG = gamma;
      }
    } // gamma loop 
  } // C loop
  svmParameter.C = pow(2,bestC);
  svmParameter.gamma = pow(2,bestG);
  
  return bestAccuracy;
}

double im_svm_train(IMbdd& bdd,
		    const std::vector<std::string>& trainingPeople,
		    MatrixC& trainMC,
		    const std::vector<std::string>& testingPeople,
		    MatrixC& testMC){
  std::string path2bdd(bdd.getFolder());
  int k = bdd.getK();
  
  // Computing Bag Of Words for each people
  std::cout << "Computing BOWs..." << std::endl;
  std::map<std::string, struct svm_problem> peopleBOW;
  std::map<std::string, int> activitiesLabel;
  std::cout << "Computing the SVM problem (ie. BOW) of all the people..." << std::endl;
  im_compute_bdd_bow(bdd,peopleBOW); // all the BOW are saved in peopleBOW
  // Normalization: do not forget to change the normalization before this step
  //bdd.changeNormalizationSettings(normalization,
  //				      "means.txt",
  //"stand_devia.txt");
  for(std::vector<std::string>::const_iterator trainingPerson = trainingPeople.begin();
      trainingPerson != trainingPeople.end();
      ++ trainingPerson){
    for(int i=0 ; i<peopleBOW[*trainingPerson].l ; i++)
      std::cout << peopleBOW[*trainingPerson].y[i] << std::endl;
  }
  
  std::cout << "Normalizing the BOW..." << std::endl; 
  bdd.changeNormalizationSettings("simple",
				  "means.txt",
				  "stand_devia.txt");
  im_normalize_bdd_bow(bdd,trainingPeople,peopleBOW); 
  
  std::cout << "Not exporting the problem..." << std::endl;
  // Export ?? OR NOT export ???????? THAT IS THE QUESTION
  // Exporting problem
  //exportProblem(svmTrainProblem, path2bdd + "/concatenate.bow.train");
  //if(testingPeople != {""}) 
  //exportProblem(svmTestProblem, path2bdd + "/concatenate.bow.test");
  
  // * SVM *
  // (One Versus the Rest (OVR))
  // SVM parameters
  struct svm_parameter svmParameter;
  get_svm_parameter(k,svmParameter);
  std::cout << "Searching the best C and the best Gamma..." << std::endl;
  // We have to do a leave one out with the training data
  // For the moment we use the same training centers to find Gamma and C
  int minC = -5, maxC = 5;
  int minG = -10, maxG = 3;
  double crossValidationAccuracy =
    im_training_leave_one_out(bdd,
			      trainingPeople, peopleBOW,
			      minC, maxC, // ln(min/max C)
			      minG, maxG, // ln(min/max G)
			      svmParameter);
  // N.B: when we export the model to the robot, peopleBOW index = trainingPeople
  
  // Generating the SVM model
  std::cout << "Generating the SVM model..." << std::endl;
  struct svm_problem trainingProblem;
  trainingProblem.l = 0;
  trainingProblem.x = NULL;
  trainingProblem.y = NULL;
  for(std::vector<std::string>::const_iterator trainingPerson = trainingPeople.begin();
      trainingPerson != trainingPeople.end();
      ++ trainingPerson){
    for(int i=0 ; i < peopleBOW.at(*trainingPerson).l ; i++){
      struct svm_node* bow = peopleBOW.at(*trainingPerson).x[i];
      addBOW(bow, peopleBOW.at(*trainingPerson).y[i],trainingProblem);
    }
  }
  
  struct svm_model** svmModels = svm_train_ovr(&trainingProblem,&svmParameter);
  
  // Exporting models
  std::cout << "Saving the SVM model..." << std::endl;
  std::vector <std::string> modelFiles;
  int nrActivities = bdd.getActivities().size();
  for(int i=0 ; i< nrActivities ; i++){
    std::string fileToSaveModel = path2bdd;
    std::stringstream ss;
    ss << i;
    fileToSaveModel = fileToSaveModel + "/svm_ovr_" + ss.str() + ".model";
    svm_save_model(fileToSaveModel.c_str(),svmModels[i]);
    modelFiles.push_back(fileToSaveModel);
  }
  bdd.changeSVMSettings(nrActivities,
			modelFiles);
  
  // Calculate the confusion matrix and the probability estimation
  std::cout << "Filling the training confusion matrix..." << std::endl;
  im_fill_confusion_matrix(bdd,trainingProblem,svmModels, trainMC);
  destroy_svm_problem(trainingProblem);
  
  if(testingPeople.size() > 0){
    std::cerr << "Entering in Hell..." << std::endl;
    struct svm_problem testingProblem;
    trainingProblem.l = 0;
    trainingProblem.x = NULL;
    trainingProblem.y = NULL;
    for(std::vector<std::string>::const_iterator testingPerson = testingPeople.begin();
	testingPerson != testingPeople.end();
	++ testingPerson){
      for(int i=0 ; i < peopleBOW.at(*testingPerson).l ; i++){
	std::cerr << "HELlllll" << std::endl;
	struct svm_node* bow = peopleBOW.at(*testingPerson).x[i];
	std::cerr << peopleBOW.at(*testingPerson).y[i] << std::endl;
	std::cerr << "YOOOO man" << std::endl;
	addBOW(bow, peopleBOW.at(*testingPerson).y[i],testingProblem);
	std::cerr << "Why ?!!!" << std::endl;
      }
    }
    std::cout << "Filling the testing confusion matrix..." << std::endl;
    im_fill_confusion_matrix(bdd,testingProblem,svmModels, testMC);
    destroy_svm_problem(testingProblem);
  }
  
  std::cout << "Releasing peopleBOW" << std::endl;
  // Releasing peopleBOW
  std::vector<std::string> people = bdd.getPeople();
  for(std::vector<std::string>::iterator person = people.begin();
      person != people.end();
      ++ person){
    destroy_svm_problem(peopleBOW[*person]);
  }
  
  // Releasing OVR models
  for(int i=0;i<nrActivities;i++){
    svm_free_and_destroy_model(&svmModels[i]);}
  delete [] svmModels;
  svmModels = NULL;
  
  return crossValidationAccuracy;
}
void im_compute_bdd_bow(const IMbdd& bdd, 
			std::map <std::string, struct svm_problem>& peopleBOW){
  std::string path2bdd(bdd.getFolder());
  std::vector<std::string> activities = bdd.getActivities();
  std::vector<std::string> people = bdd.getPeople();

  int dim = bdd.getDim();
  int maxPts = bdd.getMaxPts();
  int k = bdd.getK();
  
  for(std::vector<std::string>::iterator person = people.begin();
      person != people.end();
      ++person){
    int currentActivity = 1;
    struct svm_problem svmPeopleBOW;
    svmPeopleBOW.l = 0;
    svmPeopleBOW.x = NULL;
    svmPeopleBOW.y = NULL;
    std::cout << "Computing the svmProblem of " << *person << std::endl;
    for(std::vector<std::string>::iterator activity = activities.begin();
	activity != activities.end();
	++activity){
      string rep(path2bdd + "/" + *person + "/" + *activity + "/fp");
      DIR * repertoire = opendir(rep.c_str());
      if (!repertoire){
	std::cerr << "Impossible to open the feature points directory!" << std::endl;
	exit(EXIT_FAILURE);
      }
      struct dirent * ent;
      while ( (ent = readdir(repertoire)) != NULL){
	std::string file = ent->d_name;
	if(file.compare(".") != 0 && file.compare("..") != 0){
	  std::string path2FPs(rep + "/" + file);
	  KMdata dataPts(dim,maxPts);
	  int nPts = importSTIPs(path2FPs, dim, maxPts, &dataPts);
	  if(nPts != 0){
	    dataPts.setNPts(nPts);
	    dataPts.buildKcTree();
	    
	    KMfilterCenters ctrs(k, dataPts);
	    importCenters(path2bdd + "/" + "training.means", dim, k, &ctrs);
	    
	    // Only one BOW
	    struct svm_problem svmBow = computeBOW(currentActivity,
						   dataPts,
						   ctrs);
	    addBOW(svmBow.x[0], svmBow.y[0], svmPeopleBOW);
	    destroy_svm_problem(svmBow);	  
	  }
	}
      }
      closedir(repertoire);
      currentActivity++;
    }
    peopleBOW.insert(std::make_pair<std::string, struct svm_problem>((*person), svmPeopleBOW));
  }
}
void im_normalize_bdd_bow(const IMbdd& bdd, const std::vector<std::string>& trainingPeople,
			  std::map<std::string, struct svm_problem>& peopleBOW){
  int k = bdd.getK();
  // Extract and export gaussian parameters
  struct svm_problem svmProblem;
  svmProblem.l = 0;
  svmProblem.x = NULL;
  svmProblem.y = NULL;
  for(std::vector<std::string>::const_iterator person = trainingPeople.begin();
      person != trainingPeople.end();
      ++person){
    for(int i=0 ; i<peopleBOW[*person].l ; i++){
      svm_node *bow = peopleBOW[*person].x[i];
      addBOW(bow, peopleBOW[*person].y[i],  svmProblem);
    }
  }
  double *means=NULL, *stand_devia=NULL;
  means = new double[k];
  stand_devia = new double[k];
  std::cout << "Extracting gaussian parameters..." << std::endl;
  get_gaussian_parameters(k,
			  svmProblem,
			  means,
			  stand_devia);
  destroy_svm_problem(svmProblem);
  std::cout<<"Exporting gaussian parameters..." << std::endl;
  save_gaussian_parameters(bdd,
			   means,
			   stand_devia);
  delete []means;
  delete []stand_devia;
  
  std::vector<std::string> people = bdd.getPeople();
  std::string normalization(bdd.getNormalization());
  if(normalization.compare("") !=0){
    std::cout << "Doing the " << normalization << " normalization..." << std::endl;
    for(std::vector<std::string>::iterator person = people.begin();
	person != people.end();
	++person){
      bow_normalization(bdd,peopleBOW[*person]);
    }
  }
}
void bow_normalization(const IMbdd& bdd, struct svm_problem& svmProblem){
  std::string normalization(bdd.getNormalization());
  if(normalization.compare("simple") == 0 || normalization.compare("both") == 0)
    bow_simple_normalization(svmProblem);
  if(normalization.compare("gaussian") == 0|| normalization.compare("both") == 0){
    int k = bdd.getK();
    double means[k], stand_devia[k];
    load_gaussian_parameters(bdd,
			     means,
			     stand_devia);
    bow_gaussian_normalization(k,
			       means,
			       stand_devia,
			       svmProblem);
  }
}

void im_fill_confusion_matrix(const IMbdd& bdd,
			      const struct svm_problem& svmProblem,
			      struct svm_model** svmModels,
			      MatrixC& MC){
  int nrActivities = bdd.getActivities().size();
  double* py = svmProblem.y;
  int pnum = svmProblem.l;
  struct svm_node** px = svmProblem.x;
  for(int i=0 ; i<pnum ; i++){
    double* probs = new double[nrActivities];
    double lab_in = py[i];
    double lab_out = svm_predict_ovr_probs(svmModels,px[i],nrActivities,
					   probs,2);

    std::cout << "in=" << lab_in << " out=" << lab_out << std::endl;
    MC.addTransfer(lab_in,lab_out);
    std::cout << "Probs: ";
    for(int j=0; j<nrActivities; j++){
      std::cout << setw(5) << setiosflags(ios::fixed) << probs[j]<<" "; 
    }
    std::cout << std::endl;
    delete [] probs;
  }
}
