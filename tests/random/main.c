#include <time.h>
#include <stdlib.h>
#include <iostream>

int main(){
  srand(time(NULL)); // initialisation of rand
  int nrVideos = 100;
  int nrVideosByActivities = 5;
  int nrTests = 10;
  for(int t=0 ; t<nrTests ; t++){
    int randomVector[nrVideosByActivities];
    for(int s=0; s<nrVideosByActivities;s++){
      int r = (int) rand()%(nrVideos);
      int index = 0;
      while(index<s && randomVector[index] != r){
	index++;
      }
      if(s==0 || randomVector[index] != r)
	randomVector[s] = r;
      else{
	s--;
      }
    }
    for(int s=0 ; s<nrVideosByActivities ; s++){
      std::cout << randomVector[s] << " ";
    }
    std::cout << std::endl;
     }
}
