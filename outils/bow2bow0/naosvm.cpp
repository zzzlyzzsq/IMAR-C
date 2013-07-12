/** 
 * \file naosvm.cpp
 * \brief Set of functions permiting to import/ predict a svm problem, import/create a svm model
 * \author Fabien ROUALDES (institut Mines-Télécom)
 * \date 09/07/2013 
*/
#include "naosvm.h" 

/**
 * \fn struct svm_problem importProblem(char* file)
 * \brief SVM Importation function. It read a file in the following format:
 * label 1:value 2:value 3:value (each lines).
 *
 * \param[in] file File containing the svm problem.
 * \param[in] k The number of clusters.
 * \return The svm problem in a structure.
 */
struct svm_problem importProblem(std::string file, int k){
  int l = nrOfLines(file);
  
  std::cout << "Mallocing svmProblem..." << std::endl;
  struct svm_problem svmProblem;
  svmProblem.l = l;
  svmProblem.y = (double*) malloc(svmProblem.l * sizeof(double));
  svmProblem.x = (struct svm_node **) malloc(svmProblem.l * sizeof(struct svm_node *));
  
  float* bowTab = new float[k];
  int label;
  
  std::cout << "Opening problem..." << std::endl;
  std::ifstream in(file.c_str(),ios::in);
  if(!in){
    cerr << "Can't open the file to test !" << endl;
    exit(EXIT_FAILURE);
  }
  int idActivity = 0;
  std::string line; // We read the file line by line
  while(idActivity < l && std::getline(in, line)){
    std::istringstream lss(line);
    // bowTab is reseting to 0
    for(int i = 0 ; i < k ; i++){
      bowTab[i] = .0;
    }
    lss >> label;
    svmProblem.y[idActivity] = label;
    int center = 0;
    bool endOfLine = false;
    while(!endOfLine && center < k){
      std::string tmpFloat;
      if(!(lss>>tmpFloat)){
	endOfLine = true;
      }
      if(!endOfLine){
	std::istringstream iss(tmpFloat);
	std::string token;
	getline(iss,token,':');  // centre i
	int index = atoi(token.c_str());
	// if the index is greater than the precedent center
	// it is normal else there is an error
	if(index > (center + 1) - 1){
	  while(center + 1 != index){
	    bowTab[center] = .0;
	    center++;
	  }
	  getline(iss,token,':');  // value i
	  bowTab[center] = atof(token.c_str());
	  center++;
	}
	else 
	  endOfLine = true;
      }
    }
    // Si les derniers centres sont égaux à 0 et qu'ils ne sont pas précisés
    while(center < k){
      bowTab[center] = .0;
      center++;
    }
    
    int notZero = 0;
    center = 0;
    while(center < k){
      if(bowTab[center] != 0){
	notZero++;
      }
      center++;
    }
    int i = 0;
    svmProblem.x[idActivity] = (svm_node *) malloc((notZero + 1) * sizeof(svm_node));
    center = 0;
    while(center < k){
      if(bowTab[center] != .0){
	svmProblem.x[idActivity][i].index = center + 1;
	svmProblem.x[idActivity][i].value = bowTab[center];
	i++;
      }
      center++;
    }
    svmProblem.x[idActivity][(notZero-1)+1].index = -1;
    // c'est la fin du tableau pas besoin d'ajouer une valeur
    idActivity++;
  }
  in.close();
  delete[] bowTab;
  return svmProblem;
}
/**
 * \fn void exportProblem(char* file)
 * \brief SVM Exporting function. It writes in a file in the following format:
 * label 1:value 2:value 3:value (each lines).
 *
 * \param[in] file File which will containing the svm problem.
 * \param[in] k The number of clusters.
 * \return The svm problem in a structure.
 */
void exportProblem(struct svm_problem svmProblem, std::string file){
  int l = svmProblem.l;  
  ofstream bowFile(file.c_str(), ios::out | ios::trunc);  // ouverture en écriture avec effacement du fichier ouvert
  if(!bowFile){
   std::cerr << "Cannot open the file!" << std::endl;
   exit(EXIT_FAILURE);
  }
  int idActivity = 0;
  while(idActivity < l){
    bowFile << svmProblem.y[idActivity];
    int i = 0;
    while(svmProblem.x[idActivity][i].index != -1){
	bowFile << " " << svmProblem.x[idActivity][i].index << ":" << svmProblem.x[idActivity][i].value;
	i++;
    }
    bowFile << std::endl;
    idActivity++;
  }
}
void exportProblemZero(struct svm_problem svmProblem, std::string file, int k){
  int l = svmProblem.l;  
  ofstream bowFile(file.c_str(), ios::out | ios::trunc);  // ouverture en écriture avec effacement du fichier ouvert
  if(!bowFile){
   std::cerr << "Cannot open the file!" << std::endl;
   exit(EXIT_FAILURE);
  }
  int idActivity = 0;
  while(idActivity < l){
    bowFile << svmProblem.y[idActivity];
    int i = 0;
    int center =  0;
    while(svmProblem.x[idActivity][i].index != -1){
      int index = svmProblem.x[idActivity][i].index;
      while(center+1<index){
	bowFile << " " << 0.0;
	center++;
      }
      bowFile << " " << svmProblem.x[idActivity][i].value;
      i++;
      center++;
    }
    while(center<k){
      bowFile << " " << 0.0;
      center++;
    }
    bowFile << std::endl;
    idActivity++;
  }
}

/**
 * \fn struct svm_problem computeBOW(const KMdata& dataPts, const KMfilterCenters& ctrs)
 * \brief Converts the KMdata into a Bag Of Words histogram in the SVM format:
 * label 1:value 2:value 3:value (each lines).
 *
 * \param[in] dataPts The KMdata.
 * \param[in] ctrs The centers.
 * \return The svm problem in a structure.
 */
struct svm_problem computeBOW(int label, const KMdata& dataPts, KMfilterCenters& ctrs){
  int k = ctrs.getK();
  int nPts = dataPts.getNPts();

  // 1. Getting assignments 
  KMctrIdxArray closeCtr = new KMctrIdx[dataPts.getNPts()]; // dataPts = 1 label
  double* sqDist = new double[dataPts.getNPts()];
  ctrs.getAssignments(closeCtr, sqDist); 
  
  // 2. Initializing histogram
  float* bowHistogram = NULL;
  bowHistogram = new float[k];
  for(int centre = 0; centre<k; centre++)
    bowHistogram[centre]=0;
  // 3. Filling histogram
  for(int point = 0; point < nPts ; point++){
    bowHistogram[closeCtr[point]]++;
  }
  // 4. Dividing by the number of points (YOU CAN DELETE THIS PART IF YOU WANT)
  for(int centre = 0 ; centre < k ; centre++){
    bowHistogram[centre] /= nPts;
  }
  delete closeCtr;
  delete[] sqDist;
  
  // 5. Exporting the BOW in the structure svmProblem
  struct svm_problem svmProblem;
  int l = 1;
  svmProblem.l = l;
  svmProblem.y = (double*) malloc(svmProblem.l * sizeof(double));
  svmProblem.x = (struct svm_node **) malloc(svmProblem.l * sizeof(struct svm_node *));
  
  int idActivity = 0;
  while(idActivity < l){
    svmProblem.y[idActivity] = label;
    int notZero = 0;
    int center = 0;
    while(center < k){
      if(bowHistogram[center] != 0){
	notZero++;
      }
      center++;
    }
    int i = 0;
    svmProblem.x[idActivity] = (svm_node *) malloc((notZero + 1) * sizeof(svm_node));
    center = 0;
    while(center < k){
      if(bowHistogram[center] != .0){
	svmProblem.x[idActivity][i].index = center + 1;
	svmProblem.x[idActivity][i].value = bowHistogram[center];
	i++;
      }
      center++;
    }
    svmProblem.x[idActivity][(notZero-1)+1].index = -1;
    // It is the end of the table we do not need to add a value
    idActivity++;
  }
  delete bowHistogram;

  return svmProblem; 
}
/**
 * \fn void printProblem(struct svm_problem svmProblem)
 * \brief It permits to print the SVM problem in the standard output.
 *
 * \param[in] svmProblem It is the structure containing the SVM problem.
 */
void printProblem(struct svm_problem svmProblem){
  int nbActivities = svmProblem.l;
  double* labels = svmProblem.y;
  struct svm_node** centers =svmProblem.x;
  
  cout << "l = " << nbActivities << endl;
  cout << "y -> ";
  for(int idActivity = 0 ; idActivity<nbActivities ; idActivity++){
    cout << labels[idActivity] << " ";
  }
  cout << endl;
  cout << "x -> ";
  
  for(int idActivity = 0 ; idActivity<nbActivities ; idActivity++){
    if(idActivity == 0)
      cout << "[ ] -> ";
    else
      cout << "     " << "[ ] -> ";
    int idCenter = 0;
    int index;
    while((index = centers[idActivity][idCenter].index) != -1){
      double value = centers[idActivity][idCenter].value;
      cout <<  "(" << index << "," << value << ")" << " ";
      idCenter++;
    }
    cout << "(" << index << ",?)" << endl;
  }
  
}

/**
 * \fn int nrOfLines(std::string filename)
 * \brief A function returning the number of lines (which correspond to the number of activities)
 *
 * \param[in] fileName The file we want to count the number of lines.
 * \return The number of lines of the file.
 */
int nrOfLines(std::string filename) {
  std::ifstream fichier(filename.c_str());
  if(!fichier){
    std::cout << "Ne peut ouvrir " << filename << std::endl;
    exit(EXIT_FAILURE);
  }
  
  std::string s;
  unsigned int count = 0;
  while(std::getline(fichier,s)) ++count;
  fichier.close();
  
  return count;
}

/**
 * \fn void printProbability(struct svm_model* pModel, struct svm_node* nodes)
 * \brief Print for each labels the probability of the activity (stored in the SVM node structure).
 *
 * \param[in] pModel A pointer to the SVM model.
 * \param[in] nodes The activity stored in SVM nodes.
 */
void printProbability(struct svm_model* pModel, struct svm_node* nodes){
  if(svm_check_probability_model(pModel) != 1){
    cerr << "The model does not contain required information to do probability estimates." << endl;
    exit(EXIT_FAILURE);
  }
  double *prob_estimates;
  int nrClass = pModel->nr_class;
  prob_estimates = (double *) malloc(pModel->nr_class * sizeof(double));
  double label;
  label = svm_predict_probability(pModel, nodes, prob_estimates); 
  cout << "Class\t";
  for(int i = 0 ; i<nrClass ; i++){
    int index = pModel->label[i];
    cout << index << "\t"; 
  }
  cout << endl;
  cout << label << "\t" ;
  for(int i = 0 ; i<nrClass ; i++){
    cout << prob_estimates[i] << "\t"; 
  }
  cout << endl;
}

/* PROBLEM WITH THIS FUNCTION
struct svm_node* importNodes(char* file){
  struct svm_node *svmNode = NULL;
  
  float bowTab[NB_MEANS];
  int label;
  
  std::ifstream in(file,ios::in);
  if(!in){
    cerr << "Can't open the file to test !" << endl;
    exit(EXIT_FAILURE);
  }
  // On remet à 0 le tableau bowTab
  for(int i = 0 ; i < NB_MEANS ; i++){
    bowTab[i] = .0;
  }
  in >> label;
  
  int center = 0;
  bool endOfLine = false;
  while(!endOfLine && center < NB_MEANS){
    std::string tmpFloat;
    if(!(in>>tmpFloat)){
      endOfLine = true;
    }
    if(!endOfLine){
      std::istringstream iss(tmpFloat);
      std::string token;
      getline(iss,token,':');  // centre i
      while(atoi(token.c_str()) != center + 1){
	bowTab[center] = .0;
	center++;
      }
      getline(iss,token,':');  // value i
      bowTab[center] = atof(token.c_str());
      center++;
    }
  }
  // Si les derniers centres sont égaux à 0 et qu'ils ne sont pas précisés
  while(center < NB_MEANS){
    bowTab[center] = .0;
    center++;
  }
  
  int notZero = 0;
  center = 0;
  while(center < NB_MEANS){
    if(bowTab[center] != 0){
      notZero++;
    }
    center++;
  }
  
  int i = 0;
  svmNode = (svm_node *) malloc((notZero + 1) * sizeof(svm_node));
  center = 0;
  while(center < NB_MEANS){
    if(bowTab[center] != .0){
	svmNode[i].index = center + 1;
	svmNode[i].value = bowTab[center];
	i++;
    }
    center++;
  }
  svmNode[(notZero-1)+1].index = -1;
  in.close();
  return svmNode;
}*/
/*
void printNodes(struct svm_node* nodes){
  cout << "x -> ";
  cout << "[ ] -> ";
  int idCenter = 0;
  int index;
  while((index = nodes[idCenter].index) != -1){
    double value = nodes[idCenter].value;
    cout <<  "(" << index << "," << value << ")" << " ";
    idCenter++;
  }
  cout << "(" << index << ",?)" << endl;
}*/

