/**
 * \file naomngt.cpp
 * \brief Set of functions permiting to manage the activity recognition BDD of Bag Of Words.
 * \author Fabien ROUALDES (institut Mines-Télécom)
 * \date 17/07/2013
 */
#include "naomngt.h"

/**
 * \fn void listBdds()
 * \brief List BDDs present in the global database.
 */
void listBdds(){
  DIR * repertoire = opendir("bdd");
  if (repertoire == NULL){
    std::cerr << "Impossible to open the BDDs directory!" << std::endl;
  }
  else{
    struct dirent * ent;
    while ( (ent = readdir(repertoire)) != NULL){
      std::string file = ent->d_name;
      if(file.compare(".") != 0 && file.compare("..") != 0)
	std::cout << file << std::endl;
    }
    closedir(repertoire);
  }
}

/**
 * \fn void listActivities(std::string bdd)
 * \brief List activities present in the specified database.
 * \param[in] bdd The name of the bdd.
 */
void listActivities(std::string bdd){
  std::string path2bdd("bdd/" + bdd);   
  activitiesMap *am;
  int nbActivities = mapActivities(path2bdd,&am);
  for(int i=0 ; i<nbActivities ; i++){
    std::cout << am[i].activity << " " << "(";
    std::cout << am[i].label << ")";
    std::cout << std::endl;
  }
  delete []am;
}

/**
 * \fn int mapActivities(std::string path2bdd, activitiesMap **am)
 * \brief Fills the object activitiesMap which contain the equivalence Label-Activity.
 * \param[in] path2bdd The path to the BDD
 * \param[in,out] am A pointer to an object activitiesMap.
 * \return The number of activities.
 */
int mapActivities(std::string path2bdd, activitiesMap **am){
  path2bdd = path2bdd + "/mapping.txt";
  std::ifstream in(path2bdd.c_str(),ios::in);
  if(!in){
    std::cerr << "Can't open file mapping.txt!" << std::endl;
    exit(EXIT_FAILURE);
  }
  std::vector<string> mapString;
  int nbActivities = 0;
  bool endOfLine = false;
  while(!(in.eof())){
    while(!endOfLine){
      std::string tmp;
      if(!(in >> tmp)){
	endOfLine = true;
      }
      if (!endOfLine){
	mapString.push_back(tmp);
	nbActivities++;
      }
    }
  }
  in.close();
  *am = new activitiesMap[mapString.size()];

  for(unsigned int i=0 ; i<mapString.size() ; i++){
    std::istringstream iss(mapString[i]);
    std::string token;
    int j = 0;
    while(getline(iss, token,':')){
      if(j==0){
	(*am)[i].label = atoi(token.c_str());
      }
      if(j==1){
	(*am)[i].activity = token;
      }
      j++;
    }
  }
  return nbActivities;
} 

/**
 * \fn int nbOfFiles(std::string path)
 * \brief Counts the number of files in a folder.
 * \param[in] path The path to the folder.
 * \return The number of files.
 */
int nbOfFiles(std::string path){
  int nbFiles = 0;
  DIR * repertoire = opendir(path.c_str());
  if (repertoire == NULL){
    std::cerr << "Impossible to open the directory!" << std::endl;
  }
  else{
    struct dirent * ent;
    while ( (ent = readdir(repertoire)) != NULL){
      std::string file = ent->d_name;
      if(file.compare(".") != 0 && file.compare("..") != 0)
	nbFiles++;
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
  if (repertory == NULL){
    std::cerr << "Impossible to open the directory!" << std::endl;
  }
  else{
    struct dirent * ent;
    while ( (ent = readdir(repertory)) != NULL){
      std::string entityName = ent->d_name;
      if(entityName.compare(".") != 0 && entityName.compare("..") != 0){
	if(entityName.compare(file) == 0){
	  return true;
	}
      }
    }
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
 * \param[in] desc The descriptor number 
 * \param[in] maxPts The maximum vectors we want to compute.
 */
void addVideos(std::string bddName, std::string activity, int nbVideos, std::string* videoPaths, int desc, int maxPts){
	int dim = getDim(desc);
  std::string path2bdd("bdd/" + bddName);
  activitiesMap *am;
  int nbActivities = mapActivities(path2bdd,&am);
  int i = 0;
  while(am[i].activity.compare(activity) != 0 && i < nbActivities){
    i++;
  }
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
  string stipspath(path2bdd + "/" + strlabel + "/stips");
  j = nbFiles + 1;
  
  for(int i=0 ; i<nbVideos ; i++){
    KMdata dataPts(dim,maxPts);
    string idFile = inttostring(j);
    string videoInput(copypath + "/" + strlabel + idFile + ".avi");
    string stipOutput(stipspath + "/" + strlabel + idFile + ".stip");
		int nPts;
		switch(desc){
			case 0: //HOG HOF
				nPts = extractHOGHOF(videoInput, dim, maxPts, &dataPts);
				break;
			case 1: //MBH
				nPts = extractMBH(videoInput, dim, maxPts, &dataPts);		
				break;
		}
    dataPts.setNPts(nPts);
    exportSTIPs(stipOutput, dim,dataPts);
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

/**
 * \fn void trainBdd(std::string bddName, int dim, int maxPts, int k)
 * \brief Trains the specified BDD.
 *
 * \param[in] bddName The name of the BDD.
 * \param[in] dim The dimension of the STIPs.
 * \param[in] maxPts The maximum number of points we want to compute.
 * \param[in] k The number of cluster (means).
 */
void trainBdd(std::string bddName, int dim, int maxPts, int k){
  std::string path2bdd("bdd/" + bddName);
  
  // ouverture du fichier d'équivalence label <-> activités
  activitiesMap *am;
  int nbActivities = mapActivities(path2bdd,&am);

  
  // Création du fichier concatenate.stip
  std::cout << "Creating the file concatenate.stips...";
  // d'abord, on le supprime s'il existe  
  std::string path2concatenate(path2bdd + "/" + "concatenate.stips");
  DIR * repBDD = opendir(path2bdd.c_str());
  if (repBDD == NULL){
    std::cerr << "Impossible top open the BDD directory"<< std::endl;
    exit(EXIT_FAILURE);
  }
  struct dirent *ent;
  while ( (ent = readdir(repBDD)) != NULL){
    std::string file = ent->d_name;
    if(file.compare("concatenate.stips") == 0){
      remove(path2concatenate.c_str());
    }
  }
  closedir(repBDD);
  
  // Ensuite on concatène les fichiers
  for(int i = 0 ; i< nbActivities ; i++){
    string label = inttostring(am[i].label);
    string rep(path2bdd + "/" + label + "/stips");
    DIR * repertoire = opendir(rep.c_str());
    
    if (repertoire == NULL){
      std::cerr << "Impossible to open the stips directory!" << std::endl;
    }
    else{
      struct dirent * ent;
      while ( (ent = readdir(repertoire)) != NULL){
	std::string file = ent->d_name;
	if(file.compare(".") != 0 && file.compare("..") != 0){
	  string cmd("cat " + path2bdd + "/" + label + "/stips/" + file);
	  cmd = cmd + " >> " + path2bdd + "/" + "concatenate.stips";
	  system(cmd.c_str());
	}
      }
      closedir(repertoire);
    }
  }
  std::cout << "Done!" << std::endl;
  
  // Creating the file training.means
  std::cout << "Computing KMeans..." << std::endl;
  createTrainingMeans(path2bdd + "/" + "concatenate.stips",
		      dim,
		      maxPts,
		      k,
		      path2bdd + "/" + "training.means");
  
  std::string cmd = "cp " + path2bdd + "/" + "training.means " + "out";
  system(cmd.c_str());
  
  // Finally we have to compute BOWs
  for(int i = 0 ; i< nbActivities ; i++){
    string label = inttostring(am[i].label);
    string rep(path2bdd + "/" + label + "/stips");
    DIR * repertoire = opendir(rep.c_str());
    if (!repertoire){
      std::cerr << "Impossible to open the stips directory!" << std::endl;
      exit(EXIT_FAILURE);
    }
      struct dirent * ent;
      while ( (ent = readdir(repertoire)) != NULL){
	std::string file = ent->d_name;
	if(file.compare(".") != 0 && file.compare("..") != 0){
	  std::string path2STIPs(path2bdd + "/" + label + "/stips/" + file);
	  
	  KMdata dataPts(dim,maxPts);
	  int nPts = importSTIPs(path2STIPs, dim, maxPts, &dataPts);
	  dataPts.setNPts(nPts);
	  dataPts.buildKcTree();
	  
	  KMfilterCenters ctrs(k, dataPts);  
	  importCenters(path2bdd + "/" + "training.means", dim, k, &ctrs);
	  struct svm_problem svmProblem = computeBOW(am[i].label, dataPts, ctrs);
	  std::string path2BOW(path2bdd + "/" + label + "/bow/" + file + ".bow");
	  exportProblem(svmProblem, path2BOW);
	}
      }
      closedir(repertoire);
  }
  
  // créer la base de données svm
  
  // Création du fichier concatenate.bow
  // d'abord, on le supprime s'il existe  
  path2concatenate = path2bdd + "/" + "concatenate.bow";
  repBDD = opendir(path2bdd.c_str());
  if (repBDD == NULL){
    std::cerr << "Impossible to open the BDD directory"<< std::endl;
    exit(EXIT_FAILURE);
  }
  while ( (ent = readdir(repBDD)) != NULL){
    std::string file = ent->d_name;
    if(file.compare("concatenate.bow") == 0){
      remove(path2concatenate.c_str());
    }
  }
  closedir(repBDD);
      
  // Puis on le créé
  std::cout << "Creating the file concatenate.bow...";
  for(int i = 0 ; i< nbActivities ; i++){
    string label = inttostring(am[i].label);
    string rep(path2bdd + "/" + label + "/bow");
    DIR * repertoire = opendir(rep.c_str());
    
    if (repertoire == NULL){
      std::cerr << "Impossible to open the bow directory!" << std::endl;
    }
    else{
      struct dirent * ent;
      while ( (ent = readdir(repertoire)) != NULL){
	std::string file = ent->d_name;
	if(file.compare(".") != 0 && file.compare("..") != 0){
	  string cmd("cat " + path2bdd + "/" + label + "/bow/" + file);
	  cmd = cmd + " >> " + path2bdd + "/" + "concatenate.bow\n";
	  system(cmd.c_str());
	}
      }
      closedir(repertoire);
    }
  }
  std::cout << "Done!" << std::endl;
  
  
  // Créer le fichier svm model
  std::cout << "Generating the SVM model..." << std::endl;
  struct svm_model* svmModel = createSvmModel(path2bdd + "/concatenate.bow",k);
  
  std::cout << "Saving the SVM model..." << std::endl;
  std::string fileToSaveModel(path2bdd + "/svm.model");
  svm_save_model(fileToSaveModel.c_str(),svmModel);
  
  // Copying the file in the out folder
  cmd = "cp " + path2bdd + "/" + "svm.model " + "out";
  system(cmd.c_str());
  std::cout << "Done!" <<endl;
}

/**
 * \fn void addLabel(int label, std::string file, int k)
 * \brief Changes the label of the Bag Of Words
 *
 * \param[in] label The label.
 * \param[in] file The file containing the Bag Of Words.
 * \param[in] k The dimension of the Bag Of Words.
 */
void addLabel(int label, std::string file, int k){ 
  float* bowTab = new float[k];
  int tmp;
  
  cout << "file :" << file << endl;
  std::cout << "Importation du bow...";
  // importation du bow
  std::ifstream in(file.c_str(),ios::in);
  if(!in){
    std::cerr << "Can't open the .bow file to change its label!" << std::endl;
    exit(EXIT_FAILURE);
  }
  
  in >> tmp;
  // enregistrement de la fréquence de chaque means
  int center = 0;
  bool endOfLine = false;
  while(!endOfLine && center < k){
    std::string tmpFloat;
    if(!(in >> tmpFloat)){
      endOfLine = true;
    }
    if(!endOfLine){
      std::istringstream iss(tmpFloat);
      std::string token;
      getline(iss, token,':'); // centre i 
      getline(iss, token,':'); // valeur affectée au centre i
      bowTab[center] = atof(token.c_str());
    }
    center++;
  }
  in.close();
  std::cout << "OK!" << std::endl;
  
  // exportation avec changement du label
  ofstream out(file.c_str(), ios::out | ios::trunc);  // ouverture en écriture avec effacement du fichier ouvert
  if(!out){
    cerr << "Impossible d'ouvrir le fichier pour ajouter le label!" << endl;
    exit(EXIT_FAILURE);
  }
  out << label << " ";
  for(int i = 0 ; i < k ; i++){
    out << i+1 << ":" << bowTab[i] << " ";
  }
  out << endl;
  out.close();
  delete[] bowTab;
}

/**
 * \fn void addActivity(std::string activityName, std::string bddName)
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
  string activityBOWFolder(path2bdd + "/" + strID + "/bow");
  string activitySTIPSFolder(path2bdd + "/" + strID + "/stips");
  mkdir(activityFolder.c_str(),S_IRWXU|S_IRGRP|S_IXGRP); // rwx pour user
  mkdir(activityAVIFolder.c_str(),S_IRWXU|S_IRGRP|S_IXGRP); // rwx pour user
  mkdir(activityBOWFolder.c_str(),S_IRWXU|S_IRGRP|S_IXGRP); // rwx pour user
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
  string strID = inttostring(idActivity);
  // Recursive deletion of the activity file
  string activityFolder(path2bdd + "/" + strID);
  string activityAVIFolder(path2bdd + "/" + strID + "/avi");
  string activityBOWFolder(path2bdd + "/" + strID + "/bow");
  string activitySTIPSFolder(path2bdd + "/" + strID + "/stips");
  // avi
  emptyFolder(activityAVIFolder);
  rmdir(activityAVIFolder.c_str());
  
  // bow
  emptyFolder(activityBOWFolder);
  rmdir(activityBOWFolder.c_str());
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
 */
void addBdd(std::string bddName, int desc){
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
  std::string file = (path2bdd+"/"+"mapping.txt");
	// création du fichier desc.txt
	std::string filedesc = (path2bdd+"/"+"desc.txt");

  ofstream out(file.c_str(), ios::out);  // ouverture en écriture avec effacement du fichier ouvert
	ofstream outdesc(filedesc.c_str(), ios::out);
	outdesc << desc << endl;
  if(!out){
    std::cerr << "Impossible to create the file mapping.txt!" <<std::endl;
    exit(EXIT_FAILURE);
  }
  out.close();
	outdesc.close();
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
void refreshBdd(std::string bddName, int desc, int maxPts){
  std::string path2bdd("bdd/" + bddName);

	int dim = getDim(desc);
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
    if((file.compare("concatenate.stips") == 0) ||
       (file.compare("concatenate.bow") == 0) ||
       (file.compare("svm.model") == 0) ||
       (file.compare("training.means") == 0))
      remove(f.c_str());
  }
  closedir(repBDD);
  
  // Supression des fichier .bow et .stip de chaque activité
  activitiesMap *am;
  int nbActivities = mapActivities(path2bdd,&am);
  // supression des stips
  for(int i = 0 ; i< nbActivities ; i++){
    string label = inttostring(am[i].label);
    string rep(path2bdd + "/" + label + "/stips");
    string cmd("rm " + rep + "/*"); 
    system(cmd.c_str());
  }  
  // Deleting BOWs
  for(int i = 0 ; i< nbActivities ; i++){
    string label = inttostring(am[i].label);
    string rep(path2bdd + "/" + label + "/bow");
    string cmd("rm " + rep + "/*"); 
    system(cmd.c_str());
  }  
  // save the descriptor number
  saveDescInfo(bddName,desc);

  // Extracting STIPs for each videos
  for(int i = 0 ; i< nbActivities ; i++){
    std::string label = inttostring(am[i].label);
    std::string avipath(path2bdd + "/" + label + "/avi");
    std::string stipspath(path2bdd + "/" +  label + "/stips");
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
        string stipOutput(stipspath + "/" + label + idFile + ".stip");
				int nPts;
                cout << videoInput << std::endl;
		switch(desc){
			case 0: //HOG HOF
				nPts = extractHOGHOF(videoInput, dim, maxPts, &dataPts);
				break;
			case 1: //MBH
				nPts = extractMBH(videoInput, dim, maxPts, &dataPts);		
				break;
		}
        dataPts.setNPts(nPts);
        exportSTIPs(stipOutput, dim, dataPts);
	j++;	
      }
    }
    closedir(repertoire);
    // The extraction of the videos STIPs of the activity i terminated.
  }
  delete []am;
}

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
  
  if(FtpPut(meansFile.c_str(),rMeansFile.c_str(),FTPLIB_ASCII,nControl) != 1 ||
     FtpPut(svmFile.c_str(),rSvmFile.c_str(),FTPLIB_ASCII,nControl) != 1 ||
     FtpPut(mappingFile.c_str(),rMappingFile.c_str(),FTPLIB_ASCII,nControl) != 1
     ){
    perror("Impossible to write on the robot!\n");
    return exit(EXIT_FAILURE);
  }
  FtpQuit(nControl); 
}

/**
 * \fn int getDim(int desc)
 * \brief get the dimension from the descriptor number 
 *
 * \param[in] desc the descriptor number 
 */
int getDim(int desc){
	switch(desc){
		case 0:
			return 204;//HOG HOF
		case 1:
			return 192;//MBH
	}
	return -1;
}

/**
 * \fn void saveDescInfo(string bddName,int desc)
 * \bref save the descriptor information into the BDD
 * \param[in] bddName the BDD name
 * \param[in] desc the descriptor number
 */
void saveDescInfo(string bddName,int desc){
  std::string path2bdd("bdd/" + bddName + "/desc.txt");
	ofstream out(path2bdd.c_str(), ios::out);
	out<<desc<<endl;
	out.close();
}

/**
 * \fn int getDesc(string bddName)
 * \bref get the descriptor number from the BDD
 * \param[in] bddName the BDD name
 */
int getDesc(string bddName){
	std::string path2file("bdd/" + bddName + "/desc.txt");
	ifstream in(path2file.c_str());
	int desc;
	in >> desc;
	return desc;
}
