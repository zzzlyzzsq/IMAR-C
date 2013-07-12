/**
  * @author Fabien ROUALDES (Institut Mines-Télécom)
  * @date 12/07/2013
  * @file main.cpp
  * @brief Raise an event on the robot ("./raiseEvent <robot IP> <event name>").
  */
#include <iostream>

#include <alcommon/alproxy.h>
#include <alproxies/almemoryproxy.h>

int main(int argc, char* argv[]){
    if(argc != 3){
        std::cerr << "Usage: ./raiseEvent <robot IP> <event name>" << std::endl;
        std::cerr << "Exemple: ./raiseEvent 157.159.124.158 startRecognition" << std::endl;
        return EXIT_FAILURE;
    }
    std::string robotIP = argv[1];
    std::string eventName = argv[2];

    AL::ALMemoryProxy memProxy = AL::ALMemoryProxy(robotIP);
    memProxy.declareEvent(eventName);
    memProxy.raiseEvent(eventName,1.0f);

    return EXIT_SUCCESS;
}
