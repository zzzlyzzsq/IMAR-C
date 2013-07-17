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
bool fileExist(std::string file, std::string folder){
  DIR * repertoire = opendir(folder.c_str());
  if (repertoire == NULL){
    std::cerr << "Impossible to open the directory!" << std::endl;
  }
  else{
    struct dirent * ent;
    while ( (ent = readdir(repertoire)) != NULL){
      std::string entityName = ent->d_name;
      if(entityName.compare(".") != 0 && entityName.compare("..") != 0){
	if(entityName.compare(file)){
	  return true;
	}
      }
    }
    closedir(repertoire);
  }
  return false;
}
void addVideos(std::string bddName, std::string activity, int nbVideos, std::string* videoPaths, int dim, int maxPts){
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
    int nPts = extractSTIPs(videoInput, dim, maxPts, &dataPts);
    dataPts.setNPts(nPts);
    exportSTIPs(stipOutput, dim,dataPts);
    j++;
  }
}
string inttostring(int int2str){
  // créer un flux de sortie
  std::ostringstream oss;
  // écrire un nombre dans le flux
  oss << int2str;
  // récupérer une chaîne de caractères
  std::string result = oss.str(); 
  return result;
}

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
	  std::string path2BOW(path2bdd + "/" + label + "/bow/" + file + ".bow");	  	exportProblem(svmProblem, path2BOW);
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
 * \fn void addLabel
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
 * \fn void addActivity
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
 * \fn void addBdd(std::string bddName)
 * \brief Creates a new BDD.
 *
 * \param[in] bddName The name of the BDD we want to create.
 */
void addBdd(std::string bddName){
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
  DIR * repertoire = opendir(folder.c_str());
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
 * \fn void refreshBdd(std::string bddName, int dim, in maxPts)
 * \brief Deletes all files excepted videos and extracts STIPs again.
 *
 * \param[in] bddName The name of the BDD containing videos.
 * \param[in] dim The STIPs dimension.
 * \param[in] maxPts The maximum number of STIPs we can extract
 */
void refreshBdd(std::string bddName, int dim, int maxPts){
  std::string path2bdd("bdd/" + bddName);

  // Supression des fichiers concatenate.stips, concatenate.bow, svm.model et training.means
  DIR * repBDD = opendir(path2bdd.c_str());
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
        int nPts = extractSTIPs(videoInput, dim, maxPts, &dataPts);
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
