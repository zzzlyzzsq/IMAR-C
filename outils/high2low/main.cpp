/**
 * @file main.cpp
 * @brief Decrease the quality of a video
 * @author Fabien ROUALDES
 * @date 19/08/2013
 */
#include <stdio.h>
#include <stdlib.h>
#include "highgui.h"
#include "cv.h"

int main(int argc, char* argv[]){
  std::string vInput("input.avi");
  std::string vOutput("output.avi");
  
  cv::VideoCapture capture(vInput);
  /*try{capture = (vInput);}
  catch(cv::Exception& e){
    const char* err_msg = e.what();
    std::cout << "exception caught: " << err_msg << std::endl;
    }*/
  double fpsOut = 10;
  double fpsIn = capture.get(CV_CAP_PROP_FPS);
  double ratio = fpsOut/fpsIn; 
  std::cout << "Ratio fpsIn/fpsOut=" << ratio << std::endl;
  if(ratio > 1){
    std::cerr << "You cannot have a fps output superior of the original video!" << std::endl;
    return EXIT_FAILURE;
  }
  
  cv::Size fsize(160,120);
  // 120*106 = QQVGA Quarter-QVGA
  // 320*240 = QVGA Quarter Video Graphics Array
  // 640*480 = VGA Video Graphics Array

  int frameCount = capture.get(CV_CAP_PROP_FRAME_COUNT);
  int fourcc = capture.get(CV_CAP_PROP_FOURCC);
  
  cv::VideoWriter writer(vOutput,
			 fourcc,
			 fpsOut,
			 fsize); 
  if(!writer.isOpened()){
    std::cerr << "Error while opening Writer!" << std::endl;
    return EXIT_FAILURE;
  }
  cv::Mat mat;
  cv::Mat mConverted(fsize.height,fsize.width, CV_8UC1);
  int maxFrames = ratio*frameCount;
  
  // Randomizing the frames
  int values[frameCount];
  for(int i=0 ; i<frameCount ; i++){
    values[i] = i;
  }
  std::random_shuffle(values, values + frameCount);
  std::sort(values, values + maxFrames);
  
  for(int i=0 ; i<maxFrames ; i++){
    capture.set(CV_CAP_PROP_POS_FRAMES, (double) values[i]);
    capture >> mat;
    resize(mat,mConverted,fsize);
    writer << mConverted;
  }
  
  std::cout << "INPUT: " << vInput << std::endl;
  std::cout << "* Number of frames: " << frameCount << std::endl;
  std::cout << "* Frames per second: " << fpsIn << std::endl;
  
  std::cout << "OUTPUT: " << vOutput << std::endl;
  std::cout << "* Number of frames: " << maxFrames << std::endl;
  std::cout << "* Frames per second: " << fpsOut << std::endl;
  
  return EXIT_SUCCESS;
}
