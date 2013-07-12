/**
 * \file main.cpp
 * \brief Programme de conversion de vidéo (from coloured video to grayscaled video)
 * \author Hadaptic 2013
 * \date 31/05/2013
 *
 * Permet de convertir une vidéo couleur vers une vidéo en noir et blanc.
 * La sortie sera au format AVI.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include "highgui.h"
#include "cv.h"

int main(int argc, char* argv[]){
  cv::VideoCapture capture;
  capture.open(argv[1]);
  if(!capture.isOpened()){
    std::cerr << "Error while opening Capture!" << std::endl;
    return EXIT_FAILURE;
  }
  double fps = capture.get(CV_CAP_PROP_FPS);
  double tpf = 1000/fps;
  
  double fheight = capture.get(CV_CAP_PROP_FRAME_HEIGHT);
  double fwidth = capture.get(CV_CAP_PROP_FRAME_WIDTH);
  cv::Size fsize;
  fsize.height = fheight;
  fsize.width = fwidth;
  
  double frameCount = capture.get(CV_CAP_PROP_FRAME_COUNT);
  int fourcc = capture.get(CV_CAP_PROP_FOURCC);
  cv::VideoWriter writer(argv[2],
			 fourcc,
			 fps,
			 fsize); 
  if(!writer.isOpened()){
    std::cerr << "Error while opening Writer!" << std::endl;
    return EXIT_FAILURE;
  }
  
  cv::Mat imBgr, imGray;
  while(capture.get(CV_CAP_PROP_POS_FRAMES) < frameCount){
    capture >> imBgr;     
    // Conversion from color to gray, then from gray to color (for the .avi output)
    cvtColor(imBgr,imGray,CV_BGR2GRAY); 
    cvtColor(imGray, imGray, CV_GRAY2BGR);
    writer << imGray;
    //imshow("video",imGray);
    //cv::waitKey(tpf);
  }
  return EXIT_SUCCESS;
}
