/**
 * @file main.cpp
 * @brief Reduce the frame rate of a video.
 * @author Fabien ROUALDES (Institut Mines-Télécom)
 * @date 12/07/2013
 */
#include <stdio.h>
#include <stdlib.h>
#include "highgui.h"
#include "cv.h"

int main(int argc, char* argv[]){
  if(argc != 4){
    std::cerr << "Usage: ./fpsConvert <input> <output> <fps>" << std::endl;
    return EXIT_FAILURE;
  }
  cv::VideoCapture capture;
  capture.open(argv[1]);
  if(!capture.isOpened()){
    std::cerr << "Error while opening Capture!" << std::endl;
    return EXIT_FAILURE;
  }
  double fpsOut = atof(argv[3]);
  double fpsIn = capture.get(CV_CAP_PROP_FPS);
  if(fpsOut> fpsIn){
    std::cerr << "You cannot have a fps output superior of the original video!" << std::endl;
    return EXIT_FAILURE;
  }
  
  double fheight = capture.get(CV_CAP_PROP_FRAME_HEIGHT);
  double fwidth = capture.get(CV_CAP_PROP_FRAME_WIDTH);
  cv::Size fsize;
  fsize.height = fheight;
  fsize.width = fwidth;
  
  double frameCount = capture.get(CV_CAP_PROP_FRAME_COUNT);
  int fourcc = capture.get(CV_CAP_PROP_FOURCC);
  
  cv::VideoWriter writer(argv[2],
			 fourcc,
			 fpsOut,
			 fsize); 
  if(!writer.isOpened()){
    std::cerr << "Error while opening Writer!" << std::endl;
    return EXIT_FAILURE;
  }
  cv::Mat mat;
  int currFrame = 0;
  int framesOutput = 0;
  while((currFrame = capture.get(CV_CAP_PROP_POS_FRAMES)) < frameCount){
    capture >> mat;
    if((currFrame % (int) (fpsIn/fpsOut)) == 0){
      writer << mat;
      framesOutput++;
    }
  }
  std::cout << "INPUT: " << argv[1] << std::endl;
  std::cout << "* Number of frames: " << frameCount << std::endl;
  std::cout << "* Frames per second: " << fpsIn << std::endl;
  
  std::cout << "OUTPUT: " << argv[2] << std::endl;
  std::cout << "* Number of frames: " << framesOutput << std::endl;
  std::cout << "* Frames per second: " << fpsOut << std::endl;

  return EXIT_SUCCESS;
}
