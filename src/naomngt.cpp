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
void addActivity(std::string activityName, std::string bddName){
  string path2bdd("bdd/" + bddName);
  
  // On récupère les données des activités préexistantes
  activitiesMap *am;
  int nbActivities = mapActivities(path2bdd,&am);
  // On vérifie que la nouvelle activité n'existe pas 
  int i = 0;
  while(i < nbActivities){
    if(am[i].activity.compare(activityName) == 0){
      std::cerr << "L'activité existe déjà !" << std::endl;
      delete []am;
      exit(EXIT_FAILURE);
    }
    i++;
  }
  delete []am;
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
  out.close();
}

/**
 * \fn void deleteActivity(std::string activityName, std::string bddName)
 * \brief Deletes an existant activity in the specified BDD.
 *
 * \param[in] activityName The name of the activity to delete.
 * \param[in] bddName The name of the BDD.
 */
void deleteActivity(std::string activityName, std::string bddName){
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
  delete []am;
}

/**
 * \fn void addBdd(std::string bddName,int desc)
 * \brief Creates a new BDD.
 *
 * \param[in] bddName The name of the BDD we want to create.
 * \param[in] descriptor The descriptor used for extracting the feature points
 */
void addBdd(std::string bddName, int scale_num, std::string descriptor){
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
  bdd.changeDenseTrackSettings(scale_num,
			       descriptor,
			       getDim(descriptor));
  
  bdd.write_bdd_configuration(path2bdd.c_str(),"imconfig.xml");
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
void refreshBdd(std::string bddName, int scale_num, std::string descriptor){
  std::string path2bdd("bdd/" + bddName);
  
  int dim = getDim(descriptor);
  
  // Saving the new new descriptor with its dimension
  IMbdd bdd = IMbdd(bddName,path2bdd);
  bdd.changeDenseTrackSettings(scale_num,
			       descriptor,
			       dim);
  bdd.write_bdd_configuration(path2bdd.c_str(),"imconfig.xml");
  
  int maxPts = bdd.getMaxPts();
  
  // Supression des fichiers concatenate.stips, concatenate.bow, svm.model et training.means
  DIR* repBDD = opendir(path2bdd.c_str());
  if (repBDD == NULL){
    std::cerr << "Impossible top open the BDD directory"<< std::endl;
    exit(EXIT_FAILURE);
  }
  struct dirent *ent;
  while ( (ent = readdir(repBDD)) != NULL){
    std::string file = ent->d_name;
    std::string f = path2bdd + "/" + file;
    if((file.compare("concatenate.fp.test") == 0) ||
       (file.compare("concatenate.fp.train") == 0) ||
       (file.compare("concatenate.bow") == 0) ||
       (file.compare("svm.model") == 0) ||
       (file.compare("training.means") == 0))
      remove(f.c_str());
  }
  closedir(repBDD);
  
  // Supression des fichier .bow et .stip de chaque activité
  activitiesMap *am;
  int nbActivities = mapActivities(path2bdd,&am);
  // Deleting feature points
  for(int i = 0 ; i< nbActivities ; i++){
    string label = inttostring(am[i].label);
    string rep(path2bdd + "/" + label + "/fp");
    string cmd("rm " + rep + "/*"); 
    system(cmd.c_str());
  }  
  
  // Extracting STIPs for each videos
  for(int i = 0 ; i< nbActivities ; i++){
    std::string label = inttostring(am[i].label);
    std::string avipath(path2bdd + "/" + label + "/avi");
    std::string stipspath(path2bdd + "/" +  label + "/fp");
    DIR * repertoire = opendir(avipath.c_str());
    if (repertoire == NULL){
      std::cerr << "Impossible to open the BDDs directory!" << std::endl;
      exit(EXIT_FAILURE);
    }
    struct dirent * ent;
    int j = 1;
    while ( (ent = readdir(repertoire)) != NULL){
      std::string file = ent->d_name;
      if(file.compare(".") != 0 && file.compare("..") != 0){
	string idFile = inttostring(j);
        // Extract STIPs from the videos and save them in the repertory /path/to/bdd/label/
        KMdata dataPts(dim,maxPts);
        string videoInput(avipath + "/" + file);
        string stipOutput(stipspath + "/" + label + "-" + idFile + ".fp");
	int nPts;
	std::cout << videoInput << std::endl;
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
    // The extraction of the videos STIPs of the activity i terminated.
  }
  delete []am;
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
  
  double p = getTrainProbability(path2bdd);
  
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
  dataPts.setNPts(nPts);
  dataPts.buildKcTree();
  
  KMfilterCenters ctrs(k, dataPts);  
  importCenters(path2bdd + "/" + "training.means", dim, k, &ctrs);
    
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
  
  std::string path2model (path2bdd + "/" + bdd.getModel());
  struct svm_model* pSVMModel = svm_load_model(path2model.c_str());
  
  int nr_class = svm_get_nr_class(pSVMModel);
  int nr_couples = nr_class*(nr_class-1)/2;
  
  double* dec_values = (double*) malloc(nr_couples * sizeof(double));
  int label = svm_predict_values(pSVMModel,
				 svmProblem.x[0],
				 dec_values);
  // Computing probabilities
  double* probabilities = new double[nbActivities];
  int* votes = new int[nbActivities];
  svm_vote(nbActivities, votes, dec_values);
  double sum=0;
  for(int i=0 ; i<nbActivities ; i++){
    probabilities[i] = pow(p*(1-p),nbActivities-votes[i]);
    sum += probabilities[i];
  }
  for(int i=0 ; i<nbActivities ; i++){
    probabilities[i] /= (double) sum;
    std::cout << "Label: " << pSVMModel->label[i];
    std::cout << "(" << probabilities[i] << ")" << std::endl;
  }
  delete probabilities;
  delete votes;
  
  free(dec_values);
  destroy_svm_problem(svmProblem);
  int index = searchMapIndex(label, am, nbActivities);
  std::cout << "Activity predicted: ";
  std::cout << am[index].activity << "(" << am[index].label << ")";
  std::cout << std::endl;
  svm_free_and_destroy_model(&pSVMModel);
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
  netbuf* nControl = NULL;
  
  if(FtpConnect(robotIP.c_str(), &nControl) != 1){
    perror("Impossible to connect to the ftp server!\n");
    exit(EXIT_FAILURE);
  }
  if(FtpLogin(login.c_str(),password.c_str(),nControl) != 1){
    perror("Impossible to log to the ftp server!\n");
    exit(EXIT_FAILURE);
  }
  
  std::string path2bdd("bdd/" + bddName);
  std::string remoteFolder("/data/activity_recognition");
  
  std::string meansFile(path2bdd + "/" + "training.means");
  std::string rMeansFile(remoteFolder + "/" + "training.means");
  
  std::string svmFile(path2bdd + "/" + "svm.model");
  std::string rSvmFile(remoteFolder + "/" + "svm.model");
  
  std::string mappingFile(path2bdd + "/" + "mapping.txt");
  std::string rMappingFile(remoteFolder + "/" + "mapping.txt");
  
  std::string descFile(path2bdd + "/" + "desc.txt");
  std::string rDescFile(remoteFolder + "/" + "desc.txt");
  
  std::string kmeansFile(path2bdd + "/" + "kmeans.txt");
  std::string rKMeansFile(remoteFolder + "/" + "kmeans.txt");

  std::string meansNormalizationFile(path2bdd + "/" + "");
  std::string rMeansNormalizationFile(remoteFolder+ "/" + "");
  
  
  
  if(FtpPut(meansFile.c_str(),rMeansFile.c_str(),FTPLIB_ASCII,nControl) != 1 ||
     FtpPut(svmFile.c_str(),rSvmFile.c_str(),FTPLIB_ASCII,nControl) != 1 ||
     FtpPut(mappingFile.c_str(),rMappingFile.c_str(),FTPLIB_ASCII,nControl) != 1 ||
     FtpPut(descFile.c_str(),rDescFile.c_str(),FTPLIB_ASCII,nControl) != 1 || 
     FtpPut(kmeansFile.c_str(),rKMeansFile.c_str(),FTPLIB_ASCII,nControl) != 1
     ){
    perror("Impossible to write on the robot!\n");
    return exit(EXIT_FAILURE);
  }
  FtpQuit(nControl); 
}
#endif // TRANSFER_TO_ROBOT_NAO

void concatenate_features_points(int nbActivities,
				 activitiesMap *am,
				 std::string path2bdd,
				 int nrVideosByActivities,
				 std::vector <std::string>& trainingFiles,
				 std::vector <std::string>& testingFiles){
  // First we calculate (randomly) the feature points per activities
  concatenate_features_points_per_activities(nbActivities,
					     am,
					     path2bdd,
					     nrVideosByActivities,
					     trainingFiles,
					     testingFiles);
  
  std::cout << "Creating files concatenate.fp.train and concatenate.fp.test" << std::endl;
  // first we delete them if they exist
  std::string path2CTrain(path2bdd + "/" + "concatenate.fp.train");
  std::string path2CTest(path2bdd + "/" + "concatenate.fp.test");
  DIR * repBDD = opendir(path2bdd.c_str());
  if (repBDD == NULL){
    std::cerr << "Impossible to open the BDD directory"<< std::endl;
    exit(EXIT_FAILURE);
  }
  struct dirent *ent;
  while ( (ent = readdir(repBDD)) != NULL){
    std::string file = ent->d_name;
    if(file.compare("concatenate.fp.test") == 0 ){
      remove(path2CTest.c_str());
    }
    if(file.compare("concatenate.fp.train") == 0 ){
      remove(path2CTrain.c_str());
    }
  }
  closedir(repBDD);
  
  for(int i = 0 ; i< nbActivities ; i++){
    string label = inttostring(am[i].label);
    string rep(path2bdd + "/" + label);
    // int nrVideos = nbOfFiles(rep);
    DIR * repertoire = opendir(rep.c_str());
    if (repertoire == NULL){
      std::cerr << "Impossible to open the feature points directory!" << std::endl;
    }
    else{
      struct dirent * ent;
      while ( (ent = readdir(repertoire)) != NULL){
	std::string file = ent->d_name;
	std::string cmd;
	if(file.compare("concatenate." + label + ".fp.train") == 0){
	  cmd = "cat " + path2bdd + "/" + label + "/" + file;
	  cmd = cmd + " >> " + path2bdd + "/" + "concatenate.fp.train";
	  system(cmd.c_str());
	}
	if(file.compare("concatenate." + label + ".fp.test" ) == 0){
	  cmd = "cat " + path2bdd + "/" + label + "/" + file;
	  cmd = cmd + " >> " + path2bdd + "/" + "concatenate.fp.test";
	  system(cmd.c_str());
	}
      }
      closedir(repertoire);
    }
  }
}
void concatenate_features_points_per_activities(int nbActivities,
						activitiesMap *am,
						std::string path2bdd,
						int nrVideosByActivities,
						std::vector <std::string>& trainingFiles,
						std::vector <std::string>& testingFiles){
  srand(time(NULL)); // initialisation of rand
  
  std::cout << "Creating files concatenate.label.fp.train";
  std::cout << " and concatenate.label.fp.test for each activities" << std::endl;
  
  // first we delete them if they exist
  for(int i=0 ; i<nbActivities ; i++){
    std::string label(inttostring(am[i].label));
    std::string rep(path2bdd + "/" + label);
    std::string path2CTrain(rep + "/concatenate." + label + ".fp.train");
    std::string path2CTest(rep +"/concatenate." + label + ".fp.test");

    // We open the directory folder/label
    DIR * repBDD = opendir(rep.c_str());
    if (repBDD == NULL){
      std::cerr << "Impossible to open the BDD directory"<< std::endl;
      exit(EXIT_FAILURE);
    }
    struct dirent *ent;
    while ( (ent = readdir(repBDD)) != NULL){
      std::string file = ent->d_name;
      if(file.compare("concatenate." + label + ".fp.test") == 0 ){
	remove(path2CTest.c_str());
      }
      if(file.compare("concatenate." + label + ".fp.train") == 0 ){
	remove(path2CTrain.c_str());
      }
    }
    closedir(repBDD);
  }
  for(int i = 0 ; i< nbActivities ; i++){
    string label = inttostring(am[i].label);
    string rep(path2bdd + "/" + label + "/fp");
    int nrVideos = nbOfFiles(rep);
    
    int randomVector[nrVideosByActivities];
    for(int s=0; s<nrVideosByActivities;s++){
      int r = (int) rand()%(nrVideos);
      int index = 0;
      while(index<s && randomVector[index] != r){
	index++;
      }
      if(s==0 || randomVector[index] != r)
	randomVector[s] = r;
      else{
	s--;
      }
    }
    
    DIR * repertoire = opendir(rep.c_str());
    if (repertoire == NULL){
      std::cerr << "Impossible to open the feature points directory!" << std::endl;
    }
    else{
      struct dirent * ent;
      int count=0;
      int trainCount=0;
      while ( (ent = readdir(repertoire)) != NULL){
	std::string file = ent->d_name;
	if(file.compare(".") != 0 && file.compare("..") != 0){
	  bool goodCount = false;
	  int s=0;
	  // We search if the file corresponds to the random tirage
	  while(s<nrVideosByActivities && !goodCount){
	    if(randomVector[s] == count){
	      goodCount = true;
	    }
	    s++;
	  }
	  std::string cmd;
	  // If it corresponds, we add the feature points in the the training file
	  if(trainCount < nrVideosByActivities && goodCount){
	    trainingFiles.push_back(file);
	    cmd = "cat " + path2bdd + "/" + label + "/fp/" + file;
	    cmd = cmd + " >> " + path2bdd + "/" + label + "/concatenate." + label + ".fp.train";
	    system(cmd.c_str());
	    trainCount++;
	  }
	  else{ // If not, we save the feature points in the testing file
	    testingFiles.push_back(file);
	    cmd = "cat " + path2bdd + "/" + label + "/fp/" + file;
	    cmd = cmd + " >> " + path2bdd + "/" + label + "/concatenate." + label + ".fp.test";
	    system(cmd.c_str());
	  }
	  count++;
	}
      }
      closedir(repertoire);
    }
  }
  
  // Saving trainingFiles and testingFiles in the folder
  std::string path2TrainingFile(path2bdd + "/files.train");
  std::string path2TestingFile(path2bdd + "/files.test");
  ofstream outTrain(path2TrainingFile.c_str(), ios::out);
  for (std::vector<string>::iterator it = trainingFiles.begin() ; it != trainingFiles.end(); ++it)
    outTrain << *it << std::endl;  
  ofstream outTest(path2TestingFile.c_str(), ios::out);
  for (std::vector<string>::iterator it = testingFiles.begin() ; it != testingFiles.end(); ++it)
    outTest << *it << std::endl;  
}

int create_specifics_training_means(IMbdd bdd,
				    int subK,
				    int nr_class,
				    activitiesMap* am
				    //std::vector <std::string> rejects,
				    ){
  std::string path2bdd(bdd.getFolder());
  int dim = bdd.getDim();
  int maxPts = bdd.getMaxPts();
  
  // The total number of centers
  int k = nr_class*subK;
  
  std::cout << "k=" << k << std::endl;
  double ***vDataPts = (double ***) malloc(nr_class*sizeof(double**));
  if(!vDataPts){
    std::cerr << "Memory allocation error: vDataPts" << std::endl;
    exit(EXIT_FAILURE);
  }
  int nrFP[nr_class]; // number of feature points for each class
  for(int i=0 ; i<nr_class ; i++){
    std::string label(inttostring(am[i].label));
    std::string rep(path2bdd + "/" + label); // FP = feature points
    DIR * repertoire = opendir(rep.c_str());
    if (!repertoire){
      std::cerr << "Impossible to open the feature points directory!" << std::endl;
      exit(EXIT_FAILURE);
    }
    // Searching the file concatenate.label.fp.train
    struct dirent * ent = readdir(repertoire);
    std::string file(ent->d_name);
    while (ent &&
	   (file.compare("concatenate." + label + ".fp.train")) != 0){
      ent = readdir(repertoire);
      file = ent->d_name;
    }
    if(!ent){
      std::cerr << "No file concatenate.label.fp.train" << std::endl;
      exit(EXIT_FAILURE);
    }
    std::string path2FP(rep + "/" + file);
    KMdata kmData(dim,maxPts);
    nrFP[i] = 0;
    // Importing the feature points
    if((nrFP[i] = importSTIPs(path2FP,dim,maxPts,&kmData)) != 0){
      vDataPts[i] = (double**) malloc(nrFP[i]*sizeof(double*));
      if(!vDataPts[i]){
	std::cerr << "Memory allocation error while importing features points" << std::endl;
	exit(EXIT_FAILURE);
      }
      // Saving them in vDataPts
      for(int n=0 ; n<nrFP[i] ; n++){
	vDataPts[i][n] = (double*) malloc(dim*sizeof(double));
	if(!vDataPts[i][n]){
	  std::cerr << "Memory allocation error" << std::endl;
	  exit(EXIT_FAILURE);
	}
	for(int d=0 ; d<dim ; d++){
	  vDataPts[i][n][d] = kmData[n][d];
	}
      }
    }
    else{ // ie. nrFP[i] == 0
      std::cerr << "A class does not contain features points!" << std::endl;
      exit(EXIT_FAILURE);
    }
  } // all the feature points are saved in vDataPts[activity][vectors(dim)]
  
  // Total number of feature points
  int ttFP = 0;
  for(int i=0 ; i<nr_class ; i++){
    ttFP += nrFP[i];
  }
  
  // Memory allocation of the centers
  double** vCtrs = (double**) malloc(k*sizeof(double*));
  if(!vCtrs){
    std::cerr << "Ctrs memory allocation error" << std::endl;
    exit(EXIT_FAILURE);
  }
  for(int i=0 ; i<k ; i++){
    vCtrs[i] = (double*) malloc(dim*sizeof(double));
    if(!vCtrs[i]){
      std::cerr << "Ctrs[i] memory allocation error" << std::endl;
      exit(EXIT_FAILURE);
    }
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
  for(int i=0 ; i<nr_class ; i++){
    for(int n=0 ; n<nrFP[i] ; n++){
      free(vDataPts[i][n]);
    }
    free(vDataPts[i]);
  }
  free(vDataPts);
  dataPts.buildKcTree();
  
  // Returning the true centers
  KMfilterCenters ctrs(k,dataPts);
  for(int n=0 ; n<k ; n++){
    for(int d=0 ; d<dim ; d++){
      ctrs[n][d] = vCtrs[n][d];
    }
  }
  for(int i=0 ; i<k ; i++){
    free(vCtrs[i]);
  }
  free(vCtrs);
  
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
void trainBdd(std::string bddName, int k){
  std::string path2bdd("bdd/" + bddName);
  std::string KMeansFile(path2bdd + "/" + "training.means");
  
  //std::cout << path2bdd << std::endl;
  //int desc = getDescID(path2bdd);
  //int dim = getDim(desc);
  // Loading the DenseTrack settings

  // Loading BDD
  IMbdd bdd = IMbdd(bddName,path2bdd);
  bdd.load_bdd_configuration(path2bdd.c_str(),"imconfig.xml");
  
  // Saving KMeans settings
  bdd.changeKMSettings("specifical",
		       k,
		       "training.means");
  // Loading feature points settings
  std::string descriptor = bdd.getDescriptor();

  // ouverture du fichier d'équivalence label <-> activités
  activitiesMap *am;
  int nbActivities = mapActivities(path2bdd,&am);
  int labels[nbActivities];
  for(int i=0 ; i<nbActivities ; i++){
    labels[i] = am[i].label;
  }
  
  // Creation of the file concatenate.stip
  // and the files concatenate.train.stip
  // and concatenate.test.stip per activities
  int nrVideosByActivities = 10; // as an option
  int minNrVideo = 8;
  if(nrVideosByActivities > minNrVideo - 1){
    nrVideosByActivities = minNrVideo - 1;
  }
  std::cout << "Using " << nrVideosByActivities;
  std::cout << " videos per activity for the training phase." << std::endl;
  
  MatrixC trainMC = MatrixC(nbActivities,labels);
  MatrixC testMC = MatrixC(nbActivities,labels);
  
  km_svm_train(nrVideosByActivities,
	       nbActivities, am,
	       bdd,
	       trainMC, testMC
	       );
  delete [] am;
  trainMC.calculFrequence();
  trainMC.exportMC(path2bdd,"training_confusion_matrix.txt");
  bdd.write_bdd_configuration(path2bdd.c_str(),"imconfig.xml");
  
  testMC.calculFrequence();
  testMC.exportMC(path2bdd,"testing_confusion_matrix.txt");
  
  std::cout << "#######################################" << std::endl;
  std::cout << "######## RESUME OF THE TEST ###########" << std::endl;
  std::cout << "#######################################" << std::endl;
  std::cout << "Descriptor ID: " << descriptor << std::endl;
  std::cout << "Number of means: " << k << std::endl;
  std::cout << "Train recognition rate:" << std::endl;
  std::cout << "\t tau_train=" << trainMC.recognitionRate*100 << "%" << std::endl;
  std::cout << "\t total number of train BOWs=" << trainMC.nrTest << std::endl;
  std::cout << "Test recognition rate:" << std::endl;
  std::cout << "\t tau_test=" << testMC.recognitionRate*100 << "%" << std::endl;
  std::cout << "\t total number of test BOWs=" << testMC.nrTest << std::endl;
}

void testBdd(std::string bddName, 
	     int k, 
	     int nrTests){
  std::string path2bdd("bdd/" + bddName);
  std::string KMeansFile(path2bdd + "/" + "training.means");
  
  //  std::cout << path2bdd << std::endl;
  // Loading BDD
  IMbdd bdd = IMbdd(bddName,path2bdd);
  bdd.load_bdd_configuration(path2bdd.c_str(),"imconfig.xml");
  
  // Saving KMeans settings
  bdd.changeKMSettings("specifical",
		       k,
		       "training.means");
  
  // Loading feature points settings
  std::string descriptor = bdd.getDescriptor();
  
  // ouverture du fichier d'équivalence label <-> activités
  activitiesMap *am;
  int nbActivities = mapActivities(path2bdd,&am);
  int labels[nbActivities];
  for(int i=0 ; i<nbActivities ; i++){
    labels[i] = am[i].label;
  }
  
  // Creation of the file concatenate.stip
  // and the files concatenate.train.stip
  // and concatenate.test.stip per activities
  int nrVideosByActivities = 10; // as an option
  int minNrVideo = 8;
  if(nrVideosByActivities > minNrVideo - 1){
    nrVideosByActivities = minNrVideo - 1;
  }
  std::cout << "Using " << nrVideosByActivities;
  std::cout << " videos per activity for the training phase." << std::endl;
  
  MatrixC trainMC = MatrixC(nbActivities,labels);
  MatrixC testMC = MatrixC(nbActivities,labels);
  
  for(int t=0 ; t<nrTests ; t++){
    km_svm_train(nrVideosByActivities,
		 nbActivities, am,
		 bdd,
		 trainMC, testMC
		 );
  }
  trainMC.calculFrequence();
  trainMC.exportMC(path2bdd,"training_confusion_matrix.txt");
  trainMC.output();
  testMC.calculFrequence();
  testMC.exportMC(path2bdd,"testing_confusion_matrix.txt");
  testMC.output();
    
  std::cout << "#######################################" << std::endl;
  std::cout << "######## RESUME OF THE TEST ###########" << std::endl;
  std::cout << "#######################################" << std::endl;
  std::cout << "Number of tests: " << nrTests << std::endl;
  std::cout << "Descriptor ID: " << descriptor << std::endl;
  std::cout << "Number of means: " << k << std::endl;
  std::cout << "Train recognition rate:" << std::endl;
  std::cout << "\t tau_train=" << trainMC.recognitionRate*100 << "%" << std::endl;
  std::cout << "\t total number of train BOWs=" << trainMC.nrTest << std::endl;
  std::cout << "Test recognition rate:" << std::endl;
  std::cout << "\t tau_test=" << testMC.recognitionRate*100 << "%" << std::endl;
  std::cout << "\t total number of test BOWs=" << testMC.nrTest << std::endl;
}
void km_svm_train(int nrVideosByActivities,
		  int nbActivities, activitiesMap *am,
		  IMbdd& bdd,
		  MatrixC& trainMC, MatrixC& testMC
		  ){
  std::vector <std::string> trainingFiles;
  std::vector <std::string> testingFiles;
  concatenate_features_points(nbActivities,
			      am,
			      bdd.getFolder(),
			      nrVideosByActivities,
			      trainingFiles,
			      testingFiles);
  int k = bdd.getK();
  // Creating the file training.means
  // It will only use concatenate files
  std::cout << "Computing KMeans..." << std::endl;
  /* createTrainingMeans(path2bdd + "/" + "concatenate.fp.train",
     dim,
     maxPts,
     k,
     meansFile);*/
  int subK = k/nbActivities;
  if(k%nbActivities != 0){
    std::cerr << "k is not divisible by nbActivities !" << std::endl;
    exit(EXIT_FAILURE);
  }
  create_specifics_training_means(bdd,
				  subK,
				  nbActivities, am);
    
  std::cout << "Computing BOWs..." << std::endl;
  // Finally we have to compute BOWs
  struct svm_problem svmTrainProblem;
  svmTrainProblem.l = 0;
  svmTrainProblem.x = NULL;
  svmTrainProblem.y = NULL;
  struct svm_problem svmTestProblem;
  svmTestProblem.l = 0;
  svmTestProblem.x = NULL;
  svmTestProblem.y = NULL;
  
  std::string path2bdd(bdd.getFolder());
  int dim = bdd.getDim();
  int maxPts = bdd.getMaxPts();
  for(int i = 0 ; i< nbActivities ; i++){
    std::string label = inttostring(am[i].label);
    std::string activity = am[i].activity;
    std::cout << "BOW of the activity ";
    std::cout<< activity << "(" << label << ")..." << std::endl;
    
    string rep(path2bdd + "/" + label + "/fp");
    DIR * repertoire = opendir(rep.c_str());
    if (!repertoire){
      std::cerr << "Impossible to open the stips directory!" << std::endl;
      exit(EXIT_FAILURE);
    }
    struct dirent * ent;
    while ( (ent = readdir(repertoire)) != NULL){
      std::string file = ent->d_name;
      if(file.compare(".") != 0 && file.compare("..") != 0){
	std::string path2STIPs(path2bdd + "/" + label + "/fp/" + file);
	
	KMdata dataPts(dim,maxPts);
	int nPts = importSTIPs(path2STIPs, dim, maxPts, &dataPts);
	if(nPts != 0){
	  dataPts.setNPts(nPts);
	  dataPts.buildKcTree();
	  
	  KMfilterCenters ctrs(k, dataPts);  
	  importCenters(path2bdd + "/" + "training.means", dim, k, &ctrs);
	  
	  struct svm_problem svmBow = computeBOW(am[i].label,
						 dataPts,
						 ctrs);
	  std::vector<std::string>::iterator inTrain =
	    std::find(trainingFiles.begin(), trainingFiles.end(), file);
	  std::vector<std::string>::iterator inTest =
	    std::find(testingFiles.begin(), testingFiles.end(), file);
	  if(inTrain != trainingFiles.end() && inTest != testingFiles.end()){
	    std::cout << *inTrain << " and " << *inTest << std::endl;
	    std::cerr << "Error: trainingFiles and testingFiles contain same files!" << std::endl;
	    exit(EXIT_FAILURE);
	  }
	  if(inTrain != trainingFiles.end()){
	    addBOW(svmBow,svmTrainProblem);
	  }
	  else if(inTest != testingFiles.end()){
	    addBOW(svmBow,svmTestProblem);
	  }
	  else{
	    std::cerr << "Error: the file does not exist!" << std::endl;
	    exit(EXIT_FAILURE);
	  }
	  destroy_svm_problem(svmBow);	  
	}
      }
    }
    closedir(repertoire);
  }
  
  
  //struct svm_problem svmProblem = importProblem(path2bdd + "/concatenate.bow", k);
  // Now all Bag Of Words are saved in svmProblem
  
  // Normalization step
  std::string normalization("gaussian"); // simple either gaussian either both either ""
  bdd.changeNormalizationSettings(normalization,
				  "means.txt",
				  "stand_devia.txt");
  bdd.show_bdd_configuration();
  double *means=NULL, *stand_devia=NULL;
  means = new double[k];
  stand_devia = new double[k];
  get_gaussian_parameters(k,
			  svmTrainProblem,
			  means,
			  stand_devia);
  save_gaussian_parameters(bdd,
			   means,
			   stand_devia);
  delete means;
  delete stand_devia;
  if(normalization.compare("") !=0){
    bow_normalization(bdd,svmTrainProblem);
    bow_normalization(bdd,svmTestProblem);
  }
  
  // Exporting problem
  exportProblem(svmTrainProblem, path2bdd + "/concatenate.bow.train");
  exportProblem(svmTestProblem,path2bdd + "/concatenate.bow.test");
  
  std::cout << "Done!" << std::endl;
  
  // SVM model one versus the rest
  std::cout << "Generating the SVM model..." << std::endl;
  struct svm_parameter svmParameter;
  get_svm_parameter(k,svmParameter);
  struct svm_model** svmModel = svm_train_ovr(&svmTrainProblem,&svmParameter);
    
  for(int i=0; i<nbActivities; i++){
    std::string fileToSaveModel = path2bdd;
    std::stringstream ss;
    ss << i;
    fileToSaveModel = fileToSaveModel + "/svm_ovr_" + ss.str() + ".model";
    std::cout << "Saving the SVM model..."<<i<< std::endl;
    svm_save_model(fileToSaveModel.c_str(),svmModel[i]);
  }
  
  /* Calculate the confusion matrix & the probability estimation */
  
  std::cerr<<"Training Data"<<std::endl;
  // 1- Training data
  double* py = svmTrainProblem.y;
  int pnum = svmTrainProblem.l;
  struct svm_node** px = svmTrainProblem.x;
  for(int i=0; i<pnum; i++){
    double* probs = new double[nbActivities];
    double lab_in = py[i];
    double lab_out = svm_predict_ovr_probs(svmModel,px[i],nbActivities,probs,2);
    trainMC.addTransfer(lab_in,lab_out);
    std::cerr<<"Probs: ";
    for(int j=0; j<pnum; j++){
      std::cerr<<setw(5)<<probs[j]<<" "; 
    }
    std::cerr<<std::endl;
    delete [] probs;
  }

  std::cerr<<"Testing Data"<<std::endl;
  // 2- Testing data
  py = svmTestProblem.y;
  pnum = svmTestProblem.l;
  px = svmTestProblem.x;
  for(int i=0; i<pnum; i++){
    double* probs = new double[nbActivities];
    double lab_in = py[i];
    double lab_out = svm_predict_ovr_probs(svmModel,px[i],nbActivities,probs,2);
    std::cerr<<"Probs: ";
    for(int j=0; j<pnum; j++){
      std::cerr<<setprecision(2)<<probs[j]<<" "; 
    }
    std::cerr<<std::endl;
    testMC.addTransfer(lab_in,lab_out);
    delete [] probs;
  }

  // Modified for ovr
  for(int i=0;i<nbActivities;i++){
    svm_free_and_destroy_model(&svmModel[i]);
  }
  delete [] svmModel;
  destroy_svm_problem(svmTrainProblem);
  destroy_svm_problem(svmTestProblem);
  svmModel = NULL;
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
