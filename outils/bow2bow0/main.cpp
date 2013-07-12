#include "naosvm.h"

int main(int argc, char* argv[]){
  if(argc != 3){
    std::cerr << "Usage: ./bow2bow0 input output" << std::endl;
    return EXIT_FAILURE;
  }
  int k = 100;
  std::string bow(argv[1]);
  std::string bow0(argv[2]);
  std::cout << "Importing problem..." << std::endl;
  struct svm_problem svmProblem = importProblem(bow, k);
  printProblem(svmProblem);
  std::cout << "Exporting problem..." << std::endl;
  exportProblemZero(svmProblem, bow0, k);
  return EXIT_SUCCESS;
}
