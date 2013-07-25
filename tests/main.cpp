#include "naomngt.h"
int main(int argc, char* argv[]){
  /*if(fileExist(argv[1],argv[2]) == true){
    printf("File exists!\n");
    }*/
  
  /*
    drink vs walk (1.0)
    drink vs read (0.35)
    drink vs applaud (-0.25)
    walk vs read (-0.32)
    wals vs applaud (-0.65)
    read vs applaud (1.32)
  */
  int nr_activities = 4;
  int nr_couples = (nr_activities)*(nr_activities -1)/2;
  int* labels = (int*) malloc(nr_activities * sizeof(int));
  for(int i=0; i<nr_activities ;i++){
    labels[i] = i;
  }
  double* dec_values = (double*) malloc(nr_couples * sizeof(double));
  dec_values[0] = 1.0;
  dec_values[1] = 0.35;
  dec_values[2] = -0.25;
  dec_values[3] = -0.32;
  dec_values[4] = -0.65;
  dec_values[5] = 1.32;
					
  SvmProbability* svmP =
    svm_calculate_probability(labels,
			      dec_values,
			      nr_activities);
  
  std::cout << "label\t proba" << std::endl;
  for(int i=0 ; i<nr_activities ;i++){
    std::cout << svmP[i].label << "\t";
    std::cout << svmP[i].probability << std::endl;
  }
  free(svmP);
  
  
  return EXIT_FAILURE;
}

/*#include <check.h>

START_TEST (test_name)
{

}
END_TEST
*/
