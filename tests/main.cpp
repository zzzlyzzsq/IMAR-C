#include "naomngt.h"
int main(int argc, char* argv[]){
  if(fileExist(argv[1],argv[2]) == true){
    printf("File exists!\n");
  }
  return EXIT_FAILURE;
}

/*#include <check.h>

START_TEST (test_name)
{

}
END_TEST
*/
