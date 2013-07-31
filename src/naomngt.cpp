/**
 * \file naomngt.cpp
 * \brief Set of functions permiting to manage the activity recognition BDD of Bag Of Words.
 * \author ROUALDES Fabien (Télécom SudParis)
 * \author HUILONG He (Télécom SudParis)
 * \date 25/07/2013
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
  
  // tri insertion
  for(int i=1 ; i<nbActivities ; i++){
    activitiesMap tmp;
    tmp.activity = (*am)[i].activity;
    tmp.label = (*am)[i].label;
    
    int j = i;
    while(j>0 && (*am)[j-1].label > tmp.label){
      (*am)[j].label = (*am)[j-1].label;
      (*am)[j].activity = (*am)[j-1].activity;
      j--;
    }
    (*am)[j].label = tmp.label;
    (*am)[j].activity = tmp.activity;
  }
  
  return nbActivities;
} 
bool labelExist(int label, activitiesMap *am, int nbActivities){
  int i = 0;
  while(i<nbActivities && am[i].label != label){
    i++;
  }
  return am[i].label == label;
}
int searchMapIndex(int label, activitiesMap *am, int nbActivities){
  int i = 0;
  while(i<nbActivities && am[i].label != label){
    i++;
  }
  if(am[i].label != label){
    std::cerr << " " << am[i].label << " " << label << " File mapping.txt corrupted!" << std::endl;
    exit(EXIT_FAILURE);
  }
  return i;
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
void addVideos(std::string bddName, std::string activity, int nbVideos, std::string* videoPaths, int maxPts){
  std::string path2bdd("bdd/" + bddName);
  int desc = getDesc(path2bdd);
  int dim = getDim(desc);
  
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
    if(nPts != 0){
      dataPts.setNPts(nPts);
      exportSTIPs(stipOutput, dim,dataPts);
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

/**
 * \fn void trainBdd(std::string bddName, int dim, int maxPts, int k)
 * \brief Trains the specified BDD.
 *
 * \param[in] bddName The name of the BDD.
 * \param[in] dim The dimension of the STIPs.
 * \param[in] maxPts The maximum number of points we want to compute.
 * \param[in] k The number of cluster (means).
 */
void trainBdd(std::string bddName, int maxPts, int k){
  std::string path2bdd("bdd/" + bddName);
  std::string meansFile(path2bdd + "/" + "training.means");
  int desc = getDesc(path2bdd);
  int dim = getDim(desc);
  
  // ouverture du fichier d'équivalence label <-> activités
  activitiesMap *am;
  int nbActivities = mapActivities(path2bdd,&am);
  
  // Création du fichier concatenate.stip
  concatenate_features_points(nbActivities, am, path2bdd);
  
  
  // Creating the file training.means
  std::cout << "Computing KMeans..." << std::endl;
  /*createTrainingMeans(path2bdd + "/" + "concatenate.stips",
		      dim,
		      maxPts,
		      k,
		      meansFile);
  */
  int subK = k/nbActivities;
  std::cout << "k=" << k << " & subK=" << subK << std::endl;
  k = create_specifics_training_means(path2bdd,
				      dim,
				      maxPts,
				      subK,
				      nbActivities,
				      am,
				      meansFile);
  
  std::string cmd = "cp " + path2bdd + "/" + "training.means " + "out";
  system(cmd.c_str());
  
  std::cout << "Computing BOWs..." << std::endl;
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
	std::cout << file << std::endl;
	std::string path2STIPs(path2bdd + "/" + label + "/stips/" + file);
	
	KMdata dataPts(dim,maxPts);
	int nPts = importSTIPs(path2STIPs, dim, maxPts, &dataPts);
	if(nPts != 0){
	  dataPts.setNPts(nPts);
	  dataPts.buildKcTree();
	  
	  KMfilterCenters ctrs(k, dataPts);  
	  importCenters(path2bdd + "/" + "training.means", dim, k, &ctrs);
	  
	  struct svm_problem svmProblem = computeBOW(am[i].label, dataPts, ctrs);
	  std::string path2BOW(path2bdd + "/" + label + "/bow/" + file + ".bow");
	  exportProblem(svmProblem, path2BOW);
	}
      }
    }
    closedir(repertoire);
  }
  std::cout << "Done!" << std::endl;
  // créer la base de données svm
  std::cout<< "bow" << std::endl;
  
  // Création du fichier concatenate.bow
  concatenate_bag_of_words(nbActivities, am, path2bdd);
    
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
  
  ofstream out(file.c_str(), ios::out);  // ouverture en écriture avec effacement du fichier ouvert
  if(!out){
    std::cerr << "Impossible to create the file mapping.txt!" <<std::endl;
    exit(EXIT_FAILURE);
  }
  out.close();

  // création du fichier desc.txt
  saveDescInfo(bddName,desc);
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
 * \fn void predictActivity(std::string bddName, int dim, int maxPts)
 * \brief Predict the activity done in a video with an existant trained BDD.
 *
 * \param[in] The path to the video to predict.
 * \param[in] bddName The name of the BDD containing videos.
 * \param[in] dim The STIPs dimension.
 * \param[in] maxPts The maximum number of STIPs we can extract.
 * \return The name of the activity.
 */
void predictActivity(std::string videoPath,
		     std::string bddName,
		     int maxPts,
		     int k){
  std::string path2bdd("bdd/" + bddName);   
  int desc = getDesc(path2bdd);
  int dim = getDim(desc);
  
  KMdata dataPts(dim,maxPts);
  
  int nPts = 0;
  
  switch(desc){
  case 0: //HOG HOF
    nPts = extractHOGHOF(videoPath, dim, maxPts, &dataPts);
    break;
  case 1: //MBH
    nPts = extractMBH(videoPath, dim, maxPts, &dataPts);		
    break;
  }
  
  dataPts.setNPts(nPts);
  dataPts.buildKcTree();
  
  KMfilterCenters ctrs(k, dataPts);  
  importCenters(path2bdd + "/" + "training.means", dim, k, &ctrs);
    
  activitiesMap *am;
  mapActivities(path2bdd,&am);
  
  struct svm_problem svmProblem = computeBOW(0, dataPts, ctrs);

  std::string path2model (path2bdd + "/" + "svm.model");
  struct svm_model* pSVMModel = svm_load_model(path2model.c_str());
  
  int nr_class = svm_get_nr_class(pSVMModel);
  int nr_couples = nr_class*(nr_class-1)/2;
  
  double* dec_values = (double*) malloc(nr_couples * sizeof(double));
  svm_predict_values(pSVMModel,
		     svmProblem.x[0],
		     dec_values);
  
  int *labels = (int*) malloc(nr_class * sizeof(int));
  svm_get_labels(pSVMModel,labels);
  
  std::cout << "computing proba ..." << std::endl;
  SvmProbability* svmP = svm_calculate_probability(labels, dec_values, nr_class);
  
  free(labels);
  free(dec_values);
  
  std::cout << "label\t proba" << std::endl;
  for(int i=0 ; i<nr_class ;i++){
    std::cout << svmP[i].label << "\t";
    std::cout << svmP[i].probability << std::endl;
  }
  free(svmP);
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
  
  if(FtpPut(meansFile.c_str(),rMeansFile.c_str(),FTPLIB_ASCII,nControl) != 1 ||
     FtpPut(svmFile.c_str(),rSvmFile.c_str(),FTPLIB_ASCII,nControl) != 1 ||
     FtpPut(mappingFile.c_str(),rMappingFile.c_str(),FTPLIB_ASCII,nControl) != 1 ||
     FtpPut(descFile.c_str(),rDescFile.c_str(),FTPLIB_ASCII,nControl) != 1
     ){
    perror("Impossible to write on the robot!\n");
    return exit(EXIT_FAILURE);
  }
  FtpQuit(nControl); 
}
#endif // TRANSFER_TO_ROBOT_NAO
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
  out << desc << std::endl;
}

/**
 * \fn int getDesc(string bddName)
 * \bref get the descriptor number from the BDD
 * \param[in] folder The folder containing the descriptor file.
 */
int getDesc(string folder){
  std::string path2file(folder + "/desc.txt");
  ifstream in(path2file.c_str());
  int desc;
  in >> desc;
  return desc;
}
 
void concatenate_features_points(int nbActivities, activitiesMap *am, std::string path2bdd){
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
}
int create_specifics_training_means(std::string path2bdd,
				    int dim,
				    int maxPts,
				    int subK,
				    int nr_class,
				    activitiesMap* am,
				    //std::vector <std::string> rejects,
				    std::string meansFile){
  // The total number of centers
  int k = nr_class*subK;
  
  double ***vDataPts = (double ***) malloc(nr_class*sizeof(double**));
  int *nrFP = (int *) malloc(nr_class*sizeof(int));
  if(!vDataPts){
    std::cerr << "Memory allocation error: vDataPts" << std::endl;
    exit(EXIT_FAILURE);
  }
  for(int i=0 ; i<nr_class ; i++){
    std::string rep(path2bdd + "/" + inttostring(am[i].label) + "/stips"); // FP = feature points
    DIR * repertoire = opendir(rep.c_str());
    if (!repertoire){
      std::cerr << "Impossible to open the stips directory!" << std::endl;
      exit(EXIT_FAILURE);
    }
    struct dirent * ent;
    nrFP[i] = 0;
    while ((ent = readdir(repertoire)) != NULL){
      std::string file = ent->d_name;
      if(file.compare(".") != 0 && file.compare("..") != 0){
	std::cout << file << std::endl;
	std::string path2FP(rep + "/" + file);
	KMdata kmData(dim,maxPts);
	int nPts = 0;
	if((nPts = importSTIPs(path2FP,dim,maxPts,&kmData)) != 0){
	  nrFP[i] += nPts;
	  if(nrFP[i]-nPts == 0)
	    vDataPts[i] = (double**) malloc(nPts*sizeof(double*));
	  else
	    vDataPts[i] = (double**) realloc(vDataPts[i],nrFP[i]*sizeof(double*));
	  if(!vDataPts[i]){
	    std::cerr << "Memory allocation error while importing features points" << std::endl;
	    exit(EXIT_FAILURE);
	  }
	  int index =0;
	  for(int n=(nrFP[i] - nPts) ; n<nrFP[i] ; n++){
	    vDataPts[i][n] = (double*) malloc(dim*sizeof(double));
	    if(!vDataPts[i][n]){
	      std::cerr << "Memory allocation error" << std::endl;
	      exit(EXIT_FAILURE);
	    }
	    for(int d=0 ; d<dim ; d++){
	      vDataPts[i][n][d] = kmData[index][d];
	    }
	    index++;
	  }
	} // else we read the next file because no points were detected
      } 
    }
    if(nrFP[i] == 0){
      std::cerr << "A class does not contain features points!" << std::endl;
      exit(EXIT_FAILURE);
    }
  } // all the feature points are saved in vDataPts[activity][vectors(dim)]
  
  // Total number of feature points
  int ttFP = 0;
  for(int i=0 ; i<nr_class ; i++){
    ttFP += nrFP[i];
  }
  
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
  int ic = 3;
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
  
  // Concatenate all the KMdata
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
  free(nrFP);
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
  
  exportCenters(meansFile, dim, k, ctrs);
  
  return k;
}

int getMinNumVideo(int nbActivities, activitiesMap *am, std::string path2bdd){
  int minNumVideo = 0x7fffffff;
  for(int i = 0 ; i < nbActivities ; i++){
    string label = inttostring(am[i].label);
    string rep(path2bdd + "/" + label + "/avi");
    DIR * repertoire = opendir(rep.c_str());
    if (repertoire == NULL){
      std::cerr << "Impossible to open the videos directory!" << std::endl;
    }
    else{
      struct dirent * ent;
      int numOfFile = 0;
      while( (ent = readdir(repertoire)) != NULL) numOfFile++;
      if ( numOfFile < minNumVideo ) minNumVideo = numOfFile;
    }
  }
  using std::cout;
  using std::endl;
  minNumVideo -= 2;
  cout << "The minimal number of videos: " << minNumVideo << endl; 
  return minNumVideo;
}

void concatenate_bag_of_words(int nbActivities, activitiesMap *am, std::string path2bdd){
  // d'abord, on le supprime s'il existe  
  std::string path2concatenate = path2bdd + "/" + "concatenate.bow";
  DIR* repBDD = opendir(path2bdd.c_str());
  if (repBDD == NULL){
    std::cerr << "Impossible to open the BDD directory"<< std::endl;
    exit(EXIT_FAILURE);
  }
  dirent *ent = NULL;
  while ( (ent = readdir(repBDD)) != NULL){
    std::string file = ent->d_name;
    if(file.compare("concatenate.bow") == 0){
      remove(path2concatenate.c_str());
    }
  }
  closedir(repBDD);
  
  int minNrVideo = getMinNumVideo(nbActivities, am,path2bdd);
  
  // Puis on le créé
  std::cout << "Creating the file concatenate.bow..."<<std::endl;
  for(int i = 0 ; i< nbActivities ; i++){
    string label = inttostring(am[i].label);
    string rep(path2bdd + "/" + label + "/bow");
    DIR * repertoire = opendir(rep.c_str());
    
    if (repertoire == NULL){
      std::cerr << "Impossible to open the bow directory!" << std::endl;
    }
    else{
      struct dirent * ent;
      int count = 0;
      while ( (ent = readdir(repertoire)) != NULL){
	std::string file = ent->d_name;
	if(file.compare(".") != 0 && file.compare("..") != 0 && count < minNrVideo){
	  string cmd("cat " + path2bdd + "/" + label + "/bow/" + file);
	  cmd = cmd + " >> " + path2bdd + "/" + "concatenate.bow\n";
	  system(cmd.c_str());
	  count++;
	}
      }
      closedir(repertoire);
    }
  }
  std::cout << "Done!" << std::endl;
}
