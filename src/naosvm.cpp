/** 
 * \file naosvm.cpp
 * \brief Set of functions permiting to import/ predict a svm problem, import/create a svm model
 * \author ROUALDES Fabien (Télécom SudParis)
 * \author HE Huilong (Télécom SudParis)
 * \date 17/07/2013 
*/
#include "naosvm.h" 
#include <math.h>

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
  delete[] closeCtr;
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
  delete[] bowHistogram;

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
 * \fn struct svm_model* create_svm_model(std::string bowFile, int k)
 * \brief Create the SVM model of the activities present in a file.
 *
 * \param[in] bowFile The name of the file containing the BOWs.
 * \param[in] k The number of clusters (dimension of a BOW).
 * \return The SVM model.
 */
struct svm_model* create_svm_model(int k, struct svm_problem svmProblem){
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
  
  return svm_train(&svmProblem,&svmParameter);
}
void bow_gaussian_normalization(int k,
				double* means,
				double* stand_devia,
				struct svm_problem &svmProblem
				){
  struct svm_node* node = NULL;
  for(int a=0 ; a<svmProblem.l ; a++){
    node = svmProblem.x[a];
    int i = 0;
    while(node[i].index != -1){
      int index = node[i].index - 1;
      int value = node[i].value;
      node[i].value = (value-means[index])/stand_devia[index];
      i ++;
    }
  }
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

/**
 * the definitions of class MatrixC
 */
MatrixC::MatrixC(int nr_class, int* labels){
  int n = this->num_labels = nr_class;
  this->labels = new double[n];
  for(int i=0; i<n; i++){
    this->labels[i] = labels[i];
  }
  this->m = new int*[n];
  for(int i=0; i<n; i++){
    this->m[i] = new int[n];
  }
  this->m_fre = new double*[n];
  for(int i=0; i<n; i++){
    this->m_fre[i] = new double[n];
  }
  for(int i=0; i<n; i++){
    for(int j=0; j<n; j++){
      this->m[i][j] = 0;
      this->m_fre[i][j] = .0;
    }
  }
  
  this->nrTest = 0;
  this->nrRecognition = 0;
}
MatrixC::MatrixC(const svm_model* model){
  int n = this->num_labels = model->nr_class;
  this->labels = new double[n];
  for(int i=0; i<n; i++){
    this->labels[i] = model->label[i];
  }
  this->m = new int*[n];
  for(int i=0; i<n; i++){
    this->m[i] = new int[n];
  }
  this->m_fre = new double*[n];
  for(int i=0; i<n; i++){
    this->m_fre[i] = new double[n];
  }
  for(int i=0; i<n; i++){
    for(int j=0; j<n; j++){
      this->m[i][j] = 0;
      this->m_fre[i][j] = .0;
    }
  }
  
  this->nrTest = 0;
  this->nrRecognition = 0;
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
  if(lab_in == lab_out) this->nrRecognition++;
  this->nrTest++;
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
  this->recognitionRate = (double)this->nrRecognition / (double)this->nrTest;  
}

double** MatrixC::getMatrix(){return this->m_fre;}

void MatrixC::output(){
  using namespace std;
  int num = this->num_labels;
  cout<<"===Confusion Matrix==="<<endl;
  cout<<setw(6)<<' ';
  for(int i=0;i<num;i++){
    cout<<setw(6)<<setiosflags(ios::fixed)<<setprecision(0)<<this->labels[i];
  }
  cout<<endl;
  for(int i=0;i<num;i++){
    cout<<setw(6)<<setiosflags(ios::fixed)<<setprecision(0)<<this->labels[i];
    for(int j=0;j<num;j++){
      cout<<setw(6)<<setprecision(2)<<this->m_fre[i][j];
    }
    cout<<endl;
  }
  cout<<"===END Confusion Matrix==="<<endl;
}

void MatrixC::exportMC(std::string folder, std::string file){
  std::string MCfile(folder + "/" + file);
  ofstream out(MCfile.c_str(), ios::out | ios::trunc);  // ouverture en écriture avec effacement du fichier ouvert
  int num = this->num_labels;
  for(int i=0;i<num;i++){
    for(int j=0;j<num;j++){
      out<< this->m_fre[num-i-1][j] << " ";
    }
    out<<endl;
  }
}

void bow_simple_normalization(struct svm_problem& svmProblem){
  int nrBows = svmProblem.l;
  int i;
  double sum;
  for(int l=0 ; l<nrBows ; l++){
    i=0;
    
    sum = 0;
    while((svmProblem.x[l])[i].index != -1){
      sum += (svmProblem.x[l])[i].value;
      i++;
    }
    
    i=0;
    while((svmProblem.x[l])[i].index != -1){
      (svmProblem.x[l])[i].value /= sum;
      i++;
    }
  }
}

void destroy_svm_problem(struct svm_problem svmProblem){
  free(svmProblem.y);
  //svmProblem.y = NULL;
  
  for(int a=0 ; a<svmProblem.l ; a++){
    free(svmProblem.x[a]);
    //svmProblem.x[a] = NULL;
  }
  free(svmProblem.x);
  //svmProblem.x = NULL;
}

void addBOW(const struct svm_problem& svmBow, struct svm_problem& svmProblem){
  if(svmProblem.y==NULL || svmProblem.x==NULL){
    svmProblem.y=(double*)malloc(sizeof(double));
    svmProblem.x=(struct svm_node**)malloc(sizeof(struct svm_node*));
    if(svmProblem.y==NULL || svmProblem.x==NULL){
      std::cerr << "Malloc error of svmProblem.x and svmProblem.y!" << std::endl;
      exit(EXIT_FAILURE);
    }
  }
  else{
    double *tmpy=(double*)realloc(svmProblem.y,(svmProblem.l+1)*sizeof(double));
    struct svm_node** tmpx=(struct svm_node**)realloc(svmProblem.x,(svmProblem.l+1)*sizeof(struct svm_node*));
    if(tmpy==NULL || tmpx==NULL){
      std::cerr << "Re-Malloc error of svmProblem.x and svmProblem.y!" << std::endl;
      exit(EXIT_FAILURE);
    }
    svmProblem.y = tmpy;
    svmProblem.x = tmpx;
  }
  svmProblem.y[svmProblem.l] = svmBow.y[0];
  int d=0;
  while(svmBow.x[0][d].index != -1){
    if(d==0&&(svmProblem.x[svmProblem.l]=(struct svm_node*) malloc(2*sizeof(struct svm_node))) == NULL){
      std::cerr << "Malloc error of svmProblem.x[svmProblem.l]!" << std::endl;
      exit(EXIT_FAILURE);
    }
    else if((svmProblem.x[svmProblem.l] = 
	     (struct svm_node*) realloc(svmProblem.x[svmProblem.l],
					(d+2)*sizeof(struct svm_node))) == NULL){
      std::cerr << "Re-Malloc error of svmProblem.x[svmProblem.l]!" << std::endl;
      exit(EXIT_FAILURE);
    }
    svmProblem.x[svmProblem.l][d].index = svmBow.x[0][d].index;
    svmProblem.x[svmProblem.l][d].value = svmBow.x[0][d].value;
    d++;
  }
  svmProblem.x[svmProblem.l][d].index = -1;
  svmProblem.l++;
}

void get_gaussian_parameters(int k,
			     struct svm_problem svmProblem,
			     double* means,
			     double* stand_devia){
  struct svm_node** nodes = svmProblem.x;
  //double* labels = svmProblem.y;
  int num_nodes = svmProblem.l;
  int pointers[num_nodes];
  for(int i=0; i<num_nodes; i++)
    pointers[i] = 0;
  for(int i=0; i<k; i++){
    double components[num_nodes]; 
    double total = 0;
    for(int j=0; j<num_nodes; j++){
      struct svm_node* node = nodes[j];
      components[j] = 0;
      int pointer = pointers[j];
      int index = node[pointer].index;
      if(i+1 == index){
        components[j] = node[pointer].value;
        pointers[j] ++;
      }
      total += components[j];
    }
    means[i] = total/num_nodes;
    double var = 0;
    for(int j=0; j<num_nodes; j++){
      var += (components[j]-means[i])*(components[j]-means[i]);
    }
    var /= num_nodes;
    stand_devia[i] = sqrt(var);  
  }
}
void save_gaussian_parameters(const IMbdd& bdd,
			      double* means,
			      double* stand_devia){
  // Save the gaussian parameters
  std::string meansPath(bdd.getFolder() + "/" + bdd.getMeansFile());
  std::string standPath(bdd.getFolder() + "/" + bdd.getStandardDeviationFile());
  std::ofstream outmean(meansPath.c_str());
  std::ofstream outstand(standPath.c_str());
  std::cout << meansPath << " " << standPath << std::endl;
  for(int i=0 ; i<bdd.getK() ; i++){
    outmean<<means[i]<<std::endl;
    outstand<<stand_devia[i]<<std::endl;
  }
}

void load_gaussian_parameters(const IMbdd& bdd, 
			      double* means,
			      double* stand_devia){
  std::string meansPath(bdd.getFolder() + "/" + bdd.getMeansFile());
  std::string standPath(bdd.getFolder() + "/" + bdd.getStandardDeviationFile());
  std::ifstream inmean(meansPath.c_str());
  std::ifstream instand(standPath.c_str());
  std::cout << meansPath << " " << standPath << std::endl;
  for(int i=0; i<bdd.getK() ; i++){
    inmean >> (means)[i];
    instand >> (stand_devia)[i];
  }
}

int get_svm_problem_labels(const struct svm_problem& svmProblem, int** labels){
  int nrVectors = svmProblem.l;
  
  int tmpLabels [nrVectors];
  int labelsIndex=0;
  
  for(int i=0 ; i<nrVectors ; i++){
    int currentLabel = svmProblem.y[i];
    bool labelExist = false;
    int l=0;
    while(l<labelsIndex && !labelExist){
      if (tmpLabels[l] == svmProblem.y[i]){
	labelExist = true;
      }
      l++;
    }
    if(!labelExist){
      tmpLabels[labelsIndex] = currentLabel;
      labelsIndex++;
    }
  }
  
  (*labels) = new int[labelsIndex];
  for(int i=0 ; i<labelsIndex ; i++){
    (*labels)[i] = tmpLabels[i];
  }
  
  return labelsIndex;
}

struct svm_problem equalizeSVMProblem(const struct svm_problem& svmProblem, struct svm_problem& svmRest){
  int k = getMaxIndex(svmProblem);
  
  int *labels = NULL;
  int nrLabels = get_svm_problem_labels(svmProblem,&labels);
  


  int min = getMinNumVideo(svmProblem);
  int nrVectors = nrLabels*min;

  struct svm_problem equalizedProblem; 
  equalizedProblem.l = nrVectors;
  equalizedProblem.y =  new double[nrVectors];
  equalizedProblem.x = new struct svm_node*[nrVectors];
  for(int i=0 ; i<nrVectors ; i++){
    equalizedProblem.x[i] = new struct svm_node[k+1];
    // It is better to allocate following the number of index != 0 in svmProblem
  }

  svmRest.l = svmProblem.l - nrVectors;
  svmRest.y = new double[svmRest.l];
  svmRest.x = new struct svm_node*[svmRest.l];
  for(int i=0 ; i<svmRest.l ; i++){
    svmRest.x[i] = new struct svm_node[k+1];
    // It is better to allocate following the number of index != 0 in svmProblem
  }
  
  int indexEP=0; // Equalized problem
  int indexRP=0; // Rest problem
  for(int i=0 ; i<nrLabels ; i++){
    int currentLabel = labels[i];
    int count=0;
    int v=0;
    while(v < svmProblem.l){
      if(svmProblem.y[v] == currentLabel){
	if(count < min){
	  int d=0;
	  equalizedProblem.y[indexEP] = currentLabel;
	  while(d < k && svmProblem.x[v][d].index != -1){
	    equalizedProblem.x[indexEP][d].index = svmProblem.x[v][d].index;
	    equalizedProblem.x[indexEP][d].value = svmProblem.x[v][d].value;
	    d++;
	  }
	  equalizedProblem.x[indexEP][d].index = -1;
	  count++;
	  indexEP++;
	}
	else{
	  int d=0;
	  svmRest.y[indexRP] = currentLabel;
	  while(d < k && svmProblem.x[v][d].index != -1){
	    svmRest.x[indexRP][d].index = svmProblem.x[v][d].index;
	    svmRest.x[indexRP][d].value = svmProblem.x[v][d].value;
	    d++;
	  }
	  svmRest.x[indexRP][d].index = -1;
	  indexRP++;
	}
      }
      v++;
    }
  }
  
  delete[] labels;
  return equalizedProblem;
}  
int getMinNumVideo(const struct svm_problem& svmProblem){
  int* labels = NULL;
  int nbActivities = get_svm_problem_labels(svmProblem, &labels);
  
  int minNumVideo = 0x7fffffff;
  for(int i = 0 ; i < nbActivities ; i++){
    int numOfFile = 0;
    int label = labels[i];
    for(int p=0 ; p<svmProblem.l ; p++){
      if(svmProblem.y[p] == label){
	numOfFile++;
      }
    }  
    if(numOfFile < minNumVideo) minNumVideo = numOfFile;
  }
  delete []labels;
  using std::cout;
  using std::endl;
  cout << "The minimal number of videos: " << minNumVideo << endl; 
  return minNumVideo;
}

int getMaxIndex(const struct svm_problem& svmProblem){
  int nrVectors = svmProblem.l;
  int maxIndex = 0;
  for(int i=0 ; i<nrVectors ; i++){
    int d=0;
    while(svmProblem.x[i][d].index != -1){
      if(svmProblem.x[i][d].index > maxIndex) maxIndex = svmProblem.x[i][d].index;
      d++;
    }
  }
  return maxIndex;
}

int svm_vote(int nr_class,
	     int votes[],
	     double dec_values[]){
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
  
  // Electing
  int indexMax=0;
  for(int i=1 ; i<nr_class ; i++){
    if(votes[i] > votes[indexMax])
      indexMax = i;
  }
  return indexMax;
}
// svm training funciton using the strategy one-vs-rest
svm_model **svm_train_ovr(const svm_problem *prob, const svm_parameter *param){
  int nr_class;
  vector<double> label;
  label = get_labels_from_prob(prob);
  nr_class = label.size();
  int l = prob->l;
  svm_model **model = new svm_model*[nr_class];
  if(nr_class == 1){
    std::cerr<<"Training data in only one class.Aborting!"<<std::endl;
    exit(EXIT_FAILURE);
  }
  double *temp_y = new double[l];
  for(int i=0;i<nr_class;i++){
    double label_class = label[i];
    for(int j=0;j<l;j++) temp_y[j] = prob->y[j];
    for(int j=0;j<l;j++){
      if(label_class != prob->y[j]){
        prob->y[j] = 0;
      }
    }
    model[i] = svm_train(prob,param);
    for(int j=0;j<l;j++) prob->y[j] =  temp_y[j];
  }
  delete [] temp_y;
  return model;
}

// svm predictor using the one-vs-rest strategy
double svm_predict_ovr(const svm_model** models, const svm_node* x,int nbr_class){
  double *decvs = new double[nbr_class];
  double *preds = new double[nbr_class];
  for(int i=0;i<nbr_class;i++){
    preds[i] = svm_predict_values(models[i],x,&decvs[i]); 
    if(decvs[i]<0 && preds[i]>0)
      decvs[i] = -decvs[i];
  }
  double maxvalue = decvs[0];
  double label = preds[0];
  for(int i=1;i<nbr_class;i++){
    if(decvs[i]>maxvalue){
      maxvalue = decvs[i];
      label = preds[i];
    }
  }
  if(label <= 0)
    std::cerr<<"predict ovr failed!"<<endl;
  delete [] decvs;
  delete [] preds;
  return label;
}

void get_svm_parameter(int k,struct svm_parameter &svmParameter){
  // SVM PARAMETER
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
}

std::vector<double> get_labels_from_prob(const svm_problem *prob){
  int l = prob->l;
  double *y = prob->y;
  vector<double> labels;
  for(int i=0; i<l; i++){
    bool exist = false;
    for(std::vector<double>::iterator itr = labels.begin(); itr != labels.end();itr++){
      if(y[i] == *itr){
        exist = true;
        break;
      }
    }
    if(!exist){
      labels.push_back(y[i]);
    }
  }
  return labels;
}
