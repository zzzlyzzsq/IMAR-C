#include "naomngt.h"

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
  
  /*
  
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
  
  std::cout << "Creating the file training.means...";
  KMdata dataPts(dim,maxPts);
  int nPts = importSTIPs(path2bdd + "/" + "concatenate.stips", dim, maxPts, &dataPts);
  dataPts.setNPts(nPts);
  dataPts.buildKcTree();
  KMfilterCenters ctrs(k, dataPts);  
  int ic = 3;
  kmIvanAlgorithm(ic, dim, dataPts, k, ctrs);
  exportCenters(path2bdd + "/" + "training.means", dim, k, ctrs);
  
  std::string cmd = "cp " + path2bdd + "/" + "training.means " + "out";
  system(cmd.c_str());
  
  struct svm_problem svmProblem;
  // Finally we have to compute BOWs
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
	  std::string path2STIPs(path2bdd + "/" + label + "/stips/" + file);
	  nPts = importSTIPs(path2STIPs, dim, maxPts, &dataPts);
	  dataPts.setNPts(nPts);
	  dataPts.buildKcTree();
	  svmProblem = computeBOW(am[i].label, dataPts, ctrs);
	  std::string path2BOW(path2bdd + "/" + label + "/bow/" + file + ".bow");	  
	  exportProblem(svmProblem, path2BOW);
	}
      }
      closedir(repertoire);
    }
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
    if(file.compare("concatenate.stips") == 0){
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
  */ 
  // Créer le fichier svm model
  std::cout << "Generating the SVM model..." << std::endl;
  // SVM PARAMETER
  struct svm_parameter svmParameter;
  svmParameter.svm_type = C_SVC;
  svmParameter.kernel_type = RBF;
  //  svm.degree
  svmParameter.gamma = 1/k;
  // double coef0;
  
  /* For training only : */
  svmParameter.cache_size = 100; // in MB
  svmParameter.eps = 1e-3; // stopping criteria
  svmParameter.C = 1;
  
  // change the penalty for some classes
  svmParameter.nr_weight = 0;
  svmParameter.weight_label = NULL;
  svmParameter.weight = NULL;
  
  //  double nu; // for NU_SVC, ONE_CLASS, and NU_SVR
  //  double p;	// for EPSILON_SVR 
  
  svmParameter.shrinking = 1;	/* use the shrinking heuristics */
  svmParameter.probability = 1; /* do probability estimates */
  
  //  cross_validation = 0;
  
  // SVM PROBLEM
  cout << "Importing the problem..." << std::endl;;
  struct svm_problem svmProblem = importProblem(path2bdd + "/concatenate.bow",k);
  
  // SVM MODEL
  cout << "Checking if parameters are within the feasible range of the problem..." << std::endl;
  const  char* errorMsg = svm_check_parameter(&svmProblem,&svmParameter);
  if(errorMsg != NULL){
    cerr << endl << "Error:" << endl;
    cerr << errorMsg << endl;
    exit(EXIT_FAILURE);
  }

  struct svm_model* svmModel = svm_train(&svmProblem,&svmParameter);
  cout << "# CHECK PROBABILIY" << endl;
  if(svm_check_probability_model(svmModel) == 1){
    cout << "The model contains required information to do probability estimates" << endl;
  }
  else
    cout << "The model does not contain required information to do probability estimates" << endl;
  std::string fileToSaveModel(path2bdd + "/svm.model");
  svm_save_model(fileToSaveModel.c_str(),svmModel);
  std::string cmd = "cp " + path2bdd + "/" + "svm.model " + "out";
  // cmd = "cp " + path2bdd + "/" + "svm.model " + "out";
  system(cmd.c_str());
  std::cout << "Done!" <<endl;
}

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
  // On vérifie que la nouvelle activité existe bien
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
    std::cout << "L'activité n'existe pas !" << std::endl;
    exit(EXIT_FAILURE);
  }
  string strID = inttostring(idActivity);
  // Suppression récursive du fichier activité
  string activityFolder(path2bdd + "/" + strID);
  string activityAVIFolder(path2bdd + "/" + strID + "/avi");
  string activityBOWFolder(path2bdd + "/" + strID + "/bow");
  string activitySTIPSFolder(path2bdd + "/" + strID + "/stips");
  // avi
  viderDossier(activityAVIFolder);
  rmdir(activityAVIFolder.c_str());
  
  // bow
  viderDossier(activityBOWFolder);
  rmdir(activityBOWFolder.c_str());
  // stips
  viderDossier(activitySTIPSFolder);
  rmdir(activitySTIPSFolder.c_str());
  // ACTIVITE
  rmdir(activityFolder.c_str());
  
  // Réécritude du fichier mapping.txt
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
  viderDossier(path2bdd);
  rmdir(path2bdd.c_str());
}
void viderDossier(std::string folder){
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
  // supression des bow
  for(int i = 0 ; i< nbActivities ; i++){
    string label = inttostring(am[i].label);
    string rep(path2bdd + "/" + label + "/bow");
    string cmd("rm " + rep + "/*"); 
    system(cmd.c_str());
  }  
  // Calcul des STIPs de chaque vidéos
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
