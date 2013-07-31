/** 
 * \file naosvm.cpp
 * \brief Set of functions permiting to import/ predict a svm problem, import/create a svm model
 * \author ROUALDES Fabien (Télécom SudParis)
 * \author HE Huilong (Télécom SudParis)
 * \date 17/07/2013 
*/
#include "naosvm.h" 

/**
 * \fn struct svm_problem importProblem(std::string file, int k)
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
 * \fn exportProblem(struct svm_problem svmProblem, std::string file)
 * \brief SVM Exporting function. It writes in a file in the following format:
 * label 1:value 2:value 3:value (each lines).
 *
 * \param[in] svmProblem The SVM problem to export.
 * \param[out] file The file which will contain the Bag Of Words.
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

/**
 * \fn exportProblemZero(struct svm_problem svmProblem, std::string file, int k)
 * \brief SVM Exporting function. It writes in a file in the following format:
 * label 1:value 2:value 3:value (each lines). It is different of exportProblem
 * because it writes the null values.
 *
 * \param[in] svmProblem The SVM problem to export.
 * \param[out] file The file which will contain the Bag Of Words.
 * \param[in] The dimension of the STIPs.
 */
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
	bowFile << " " << center + 1 << ":" << 0.0;
	center++;
      }
      bowFile << " " << index << ":" << svmProblem.x[idActivity][i].value;
      i++;
      center++;
    }
    while(center<k){
      bowFile << " " << center + 1 << ":" << 0.0;
      center++;
    }
    bowFile << std::endl;
    idActivity++;
  }
}

/**
 * \fn struct svm_problem computeBOW(int label, const KMdata& dataPts, KMfilterCenters& ctrs)
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
int nrOfLines(std::string filename){
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

/**
 * \fn struct svm_model* createSvmModel(std::string bowFile, int k)
 * \brief Create the SVM model of the activities present in a file.
 *
 * \param[in] bowFile The name of the file containing the BOWs.
 * \param[in] k The number of clusters (dimension of a BOW).
 * \return The SVM model.
 */
struct svm_model* createSvmModel(std::string bowFile, int k){
  // SVM PARAMETER
  struct svm_parameter svmParameter;
  svmParameter.svm_type = C_SVC;
  svmParameter.kernel_type = RBF;
  //  svm.degree
  svmParameter.gamma = 1.0/k;
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
  svmParameter.probability = 0; /* do probability estimates */
  
  //  cross_validation = 0;
  
  // SVM PROBLEM
  cout << "Importing the problem..." << std::endl;
  struct svm_problem svmProblem = importProblem(bowFile,k);
  struct svm_model* svmModel = svm_train(&svmProblem,&svmParameter);
  free(svmProblem.x);
  free(svmProblem.y);
  
  return svmModel;
}
int print_scores(const struct svm_model *model,
		 const struct svm_node *x){
  int nr_class = svm_get_nr_class(model);
  int nr_couples = nr_class*(nr_class-1)/2;
  
  double* dec_values = (double*) malloc(nr_couples * sizeof(double));
  int label = (int) svm_predict_values(model,
				       x,
				       dec_values);
  

  int *labels = (int*) malloc(nr_class * sizeof(int));
  svm_get_labels(model,labels);
  
  int* votes = (int*) malloc(nr_class * sizeof(int));
  // Initialization to zero
  for(int i=0 ; i<nr_class ; i++){
    votes[i] = 0;
  }
  int p=0;
  for(int i=0 ; i<nr_class ; i++){
    for(int j=i+1;j<nr_class;j++){
      cout << labels[i] << " vs. " << labels[j] << " = " << dec_values[p] << endl;
      p++;
    }
  }
  
  free(dec_values);
  free(votes);
  free(labels);
  return label;
}

/**
 * \fn double entropy(double x, double lambda)
 * \brief Compute the entropy of the probability.
 *
 * \param[in] x The value of the probability.
 * \param[in] lambda the scale parameter.
 * \return The entropy of the probability.
 *
 * The entropy function is H(x) = -p1(x).log(p1(x)) - p2(x).log(p2(x))
 * with p1(x) = exp(lambda.x) / (exp(lambda.x) + exp(-lambda.x))
 * and p2(x) = 1 - p1(x).
 * 
 * We implement this function because we try to balance the scores 
 * obtained by svm_predict_values in dec_values. For the moment
 * it is not used because we do not know what are exactly these dec_values.
 */
double entropy(double x, double lambda){
  return - log(cosh(lambda*x)) + (lambda*x)*tanh(lambda*x);
}

SvmProbability* svm_calculate_probability(int* labels,
					  double* dec_values,
					  int nr_class){
  int nr_couples = (nr_class)*(nr_class-1)/2;
  double* current_dec_values = (double*) malloc(nr_couples*sizeof(double));
  for(int i=0 ; i<nr_couples ;i++){
    current_dec_values[i] = dec_values[i];
  }
  int* current_labels = (int*) malloc(nr_class*sizeof(int));
  for(int i=0 ; i<nr_class ;i++){
    current_labels[i] = labels[i];
  }
  
  SvmProbability* svmProbabilities = (SvmProbability*) malloc(nr_class*sizeof(SvmProbability));
  SvmProbability svmProbability;  
  
  bool labelVoted;
  int* labelsVoted = (int*) malloc(nr_class*sizeof(int));
  
  int i = 0;
  double probabilitiesSum = 0;
  int index;
  // Make a vote (nr_class-1) times
  while(i<nr_class-1){
    // The vote
    svmProbability = svm_vote(current_labels,current_dec_values,nr_class-i);
    svmProbability.probability = svmProbability.probability*(1-probabilitiesSum);
    svmProbabilities[i] = svmProbability;
    std::cout << "Vote " << i+1 <<":"<< std::endl;
    std::cout << "Label: " << svmProbability.label << std::endl;
    std::cout << "Probability: " << svmProbability.probability << std::endl;
    
    probabilitiesSum += svmProbability.probability;
    labelsVoted[i] = svmProbability.label;
    i++;

    free(current_labels);
    current_labels = NULL;
    free(current_dec_values);
    current_dec_values = NULL;    
    
    int nonVoted = 0;
    // Filling new current_labels
    current_labels = (int*) malloc((nr_class-i) * sizeof(int));
    for(int l=0 ; l<nr_class ; l++){
      labelVoted = false;
      for(int m=0 ; m<i; m++){
	if(labels[l] == labelsVoted[m])
	  labelVoted = true;
      }
      if(!labelVoted){
	current_labels[nonVoted] = labels[l];
	nonVoted++;
      }
    }
    
    // Filling new current_dec_values
    nr_couples = (nr_class - i)*(nr_class - i - 1)/2;
    current_dec_values = (double*) malloc(nr_couples*sizeof(double));

    index = 0; // for browsing dec_values
    int p = 0; // for browsing current_dec_values
    for(int k=0 ; k<nr_class ; k++){
      labelVoted = false;
      for(int m=0 ; m<i ; m++){ // i+1 = number of votes
	if(labels[k] == labelsVoted[m])
	  labelVoted = true;
      }
      if(!labelVoted){
	for(int l=k+1;l<nr_class;l++){
	  labelVoted = false;
	  for(int m=0 ; m<i ; m++){
	    if(labels[l] == labelsVoted[m])
	      labelVoted = true;
	  }
	  if(!labelVoted){
	    current_dec_values[p] = dec_values[index];
	    p++;
	  }
	  index++;
	}
      }
      else{
	// It is the number of iteration of the sub-loop
	index += nr_class-k-1;
      }
    }
  }
  
  int indice=0;
  for(int i=0 ; i < nr_class ; i++){
    labelVoted = false;
    for(int j=0 ; j<nr_class - 1;j++){
      if(labels[i] == labelsVoted[j]){
	labelVoted = true;
      }
    }
    if(!labelVoted){
      indice = i;
    }
  }
  free(labelsVoted);
  svmProbability.label = labels[indice];
  svmProbability.probability = (1-probabilitiesSum);
  svmProbabilities[nr_class-1] = svmProbability;
  return svmProbabilities;
}
SvmProbability svm_vote(int* labels,
			double* dec_values,
			int nr_class){
  SvmProbability svmProbability;
  int* votes = (int*) malloc(nr_class * sizeof(int));
  // Initialization to zero
  for(int i=0 ; i<nr_class ; i++){
    votes[i] = 0;
  }
  
  // Voting
  int p=0;
  for(int i=0 ; i<nr_class ; i++){
    for(int j=i+1;j<nr_class;j++){
      if(dec_values[p] > 0)
	++votes[i];
      else
	++votes[j];
      p++;
    }
  }
  
  // Election
  int indice = 0;
  int max = votes[0];
  for(int i=1 ; i<nr_class ; i++){
    if(votes[i] > max){
      max = votes[i];
      indice = i;
    }
  }
  free(votes); 

  // Probability of the vote
  svmProbability.label = labels[indice];
  svmProbability.probability = max*1.0/(nr_class-1);

  return svmProbability;
}

/**
 * the definitions of class MatrixC
 */
MatrixC::MatrixC(double* labs_with_repeat,int num_of_labs){
  const int MAX_NUM_LABS = 100;
  labels = new double[MAX_NUM_LABS];
  this->num_labels = 0;
  for(int i=0; i<num_of_labs; i++){
    bool exist = false; 
    for(int j=0; j<num_labels; j++){
      if(this->labels[j] == labs_with_repeat[i]){
        exist = true;
        break;
      }
    }
    if(!exist){
      this->num_labels++;
      labels[this->num_labels - 1] = labs_with_repeat[i];
    }
  }
  
  this->m = new int*[this->num_labels];
  for(int i=0; i<this->num_labels; i++){
    this->m[i] = new int[this->num_labels];
  }
  this->m_fre = new double*[this->num_labels];
  for(int i=0; i<this->num_labels; i++){
    this->m_fre[i] = new double[this->num_labels];
  }
  for(int i=0; i<this->num_labels; i++){
    for(int j=0; j<this->num_labels; j++){
      this->m[i][j] = 0;
      this->m_fre[i][j] = .0;
    }
  }
}

MatrixC::~MatrixC(){
  delete [] this->labels;
  for(int i=0; i<this->num_labels; i++){
    delete [] this->m[i];
    delete [] this->m_fre[i];
  }
  delete [] this->m;
  delete [] this->m_fre;
}

int MatrixC::getIndex(double lab){
  for(int i=0; i<this->num_labels; i++){
    if(this->labels[i] == lab){
      return i;
    }
  }
  return -1;
}

void MatrixC::addTransfer(double lab_in,double lab_out){
  int index_in = this->getIndex(lab_in);
  int index_out = this->getIndex(lab_out);
  this->m[index_in][index_out]++;
}

void MatrixC::calculFrequence(){
  int num = this->num_labels;
  for(int i=0; i<num; i++){
    int total = 0;
    for(int j=0; j<num; j++){
      total += this->m[i][j];
    }
    for(int j=0; j<num; j++){
      this->m_fre[i][j] = (double)this->m[i][j]/(double)total;
    }
  }
}

double** MatrixC::getMatrix(){
  return this->m_fre;
}

void MatrixC::output(std::string file){
  using namespace std;
  ofstream fout(file.c_str());
  int num = this->num_labels;
  fout<<setw(6)<<' ';
  for(int i=0;i<num;i++){
    fout<<setw(6)<<setprecision(0)<<this->labels[i];
  }
  fout<<endl;
  for(int i=0;i<num;i++){
    fout<<setw(6)<<setprecision(0)<<this->labels[i];
    for(int j=0;j<num;j++){
      fout<<setw(6)<<setprecision(2)<<this->m_fre[i][j];
    }
    fout<<endl;
  }
}
