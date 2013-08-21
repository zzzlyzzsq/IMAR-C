#include "imbdd.h"

void IMbdd::write_bdd_configuration(std::string pFolder,
				    std::string pFilename){  
  TiXmlDocument doc;  
  TiXmlDeclaration* decl = new TiXmlDeclaration( "1.0", "", "" );  
  doc.LinkEndChild( decl );  
  
  TiXmlElement* root = new TiXmlElement("BDD");  
  root->SetAttribute("name",(this->bddName).c_str());
  root->SetAttribute("folder",(this->folder).c_str());
  doc.LinkEndChild( root );  
  
  TiXmlComment* bdd_comment = new TiXmlComment();
  bdd_comment->SetValue("Configuration of the BDD");  
  root->LinkEndChild(bdd_comment);  
  
  // DenseTrack
  TiXmlElement* fp = new TiXmlElement("DenseTrack");  
  root->LinkEndChild(fp);  
  
  TiXmlComment* dt_comment = new TiXmlComment();
  dt_comment->SetValue("The feature points extractor");
  fp->LinkEndChild(dt_comment);
  
  TiXmlElement* scale = new TiXmlElement("Scale");
  scale->SetAttribute("scale_num",this->scale_num);
  fp->LinkEndChild(scale);
  
  TiXmlElement* descriptor = new TiXmlElement("Descriptor");
  descriptor->SetAttribute("type",(this->descriptor).c_str());
  descriptor->SetAttribute("dim",this->dim);
  // or mbh or hoghof_mbh
  fp->LinkEndChild(descriptor);
  
  // KMeans
  TiXmlElement * kmeans = new TiXmlElement("KMeans");  
  kmeans->SetAttribute("algorithm",(this->km_algorithm).c_str());
  root->LinkEndChild(kmeans);  
  
  TiXmlElement* centers = new TiXmlElement("Centers");  
  kmeans->LinkEndChild(centers);  
  
  TiXmlElement* k = new TiXmlElement("k");
  k->SetAttribute("nr",this->k);
  centers->LinkEndChild(k);
  
  TiXmlElement* kFile = new TiXmlElement("file");
  kFile->SetAttribute("path",(this->KMeansFile).c_str());  
  centers->LinkEndChild(kFile);

  // Normalization
  TiXmlElement* normalization = new TiXmlElement("Normalization");
  normalization->SetAttribute("type",(this->normalization).c_str());
  root->LinkEndChild(normalization);
  
  TiXmlElement* means = new TiXmlElement("Means");  
  means->SetAttribute("path",(this->meansFile).c_str());  
  normalization->LinkEndChild(means);  

  TiXmlElement* standardDeviation = new TiXmlElement("StandardDeviation");
  standardDeviation->SetAttribute("path",(this->standardDeviationFile).c_str());
  normalization->LinkEndChild(standardDeviation);

  // SVM
  TiXmlElement* svm = new TiXmlElement("SVM");  
  root->LinkEndChild(svm);  
  
  //dump_to_stdout( &doc );
  std::string savePath(pFolder + "/" + pFilename);
  doc.SaveFile(savePath.c_str());  
} 

void IMbdd::load_bdd_configuration(std::string pFolder, std::string pFilename){
  TiXmlDocument doc("imconfig");
  std::string loadPath(pFolder + "/" + pFilename);
  if(!doc.LoadFile(loadPath.c_str())){
    std::cerr << "Impossible to load the bdd!" << std::endl;
    return;
  }
  
  TiXmlHandle hDoc(&doc);
  TiXmlElement* pParent;
  TiXmlElement* pElem;
  TiXmlHandle hRoot(0);
  
  // BDD name
  pElem=hDoc.FirstChildElement().Element();
  // should always have a valid root but handle gracefully if it does
  if (!pElem) return;
  this->bddName = pElem->Attribute("name");
  this->folder = pElem->Attribute("folder");
  hRoot=TiXmlHandle(pElem);
    
  // DenseTrack
  pElem = hRoot.FirstChild("DenseTrack").FirstChild("Scale").Element();
  pElem->QueryIntAttribute("scale_num",&this->scale_num);
  pElem = hRoot.FirstChild("DenseTrack").FirstChild("Descriptor").Element();
  this->descriptor = pElem->Attribute("type");
  pElem->QueryIntAttribute("dim",&this->dim);
  
  // KMeans
  pElem = hRoot.FirstChildElement("KMeans").Element();
  this->km_algorithm = pElem->Attribute("algorithm");
  pElem = hRoot.FirstChild("KMeans").FirstChild().FirstChild().Element(); 
  pElem->QueryIntAttribute("nr", &k);  
  pElem = pElem->NextSiblingElement();
  this->KMeansFile = pElem->Attribute("path");
  
  // Normalization
  pElem = hRoot.FirstChild("Normalization").Element();
  this->normalization = pElem->Attribute("type");
  pElem = hRoot.FirstChild("Normalization").FirstChild("Means").Element(); 
  this->meansFile = pElem->Attribute("path");
  pElem = hRoot.FirstChild("Normalization").FirstChild("StandardDeviation").Element(); 
  this->standardDeviationFile = pElem->Attribute("path");
  
  // SVM
  /*pElem = hRoot.FirstChildElement("SVM").FirstChild("Normalization").Element();
    this->normalization = pElem->Attribute("type");*/
}
void IMbdd::show_bdd_configuration(){
  std::cout << "BDD: " << bddName << " (in "<< folder << ")" << std::endl;
  std::cout << "# DenseTrack" << std::endl;
  std::cout << "\t - Number of scales: " << scale_num << std::endl;
  std::cout << "\t - Descriptor: " << descriptor << std::endl;
  std::cout << "\t - Dimension: " << dim << std::endl;
  std::cout << "# KMeans" << std::endl;
  std::cout << "\t - MaxPts= " << maxPts << std::endl;
  std::cout << "\t - Algorithm: " << km_algorithm << std::endl;
  std::cout << "\t - Number of means: " << k << std::endl;
  std::cout << "\t - File to the means: " << KMeansFile << std::endl;
  std::cout << "# Normalization" << std::endl;
  std::cout << "\t - Normalization used: " << normalization << std::endl;
  std::cout << "# SVM" << std::endl;
}
void IMbdd::saveName(std::string bddName){this->bddName = bddName;};
void IMbdd::changeDenseTrackSettings(int scale_num,
				     std::string descriptor,
				     int dim){
  this->scale_num = scale_num;
  this->descriptor = descriptor;
  this->dim = dim;
}
void IMbdd::changeKMSettings(std::string algorithm,
			     int k,
			     std::string KMeansFile){
  this->km_algorithm = algorithm;
  this->k = k;
  this->KMeansFile = KMeansFile;
}
void IMbdd::changeNormalizationSettings(std::string normalization,
					std::string meansFile,
					std::string standardDeviationFile){
  this->normalization = normalization;
  this->meansFile = meansFile;
  this->standardDeviationFile = standardDeviationFile;
}
void IMbdd::changeSVMSettings(std::string normalization){
  this->normalization = normalization;
}
IMbdd::IMbdd(std::string bddName){
  this->bddName = bddName;
  
  // DenseTrack
  this->scale_num = -1;
  this->descriptor = "";
  this->dim = -1;
  
  // KMeans
  this->maxPts = 1000000;
  this->km_algorithm = "";
  this->k = -1;
  this->KMeansFile = "";
  
  // Normalization
  this->normalization = "";
  this->meansFile = "";
  this->standardDeviationFile = "";
  
  // SVM
  this->normalization = "";
}
