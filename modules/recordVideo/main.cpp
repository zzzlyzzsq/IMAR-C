#include <alproxies/alvideorecorderproxy.h>
int main(int argc, char* argv[]){
    if(argc != 3){
        std::cerr << "Usage: recordVideo <Robot IP> <Video>" << std::endl;
        return EXIT_FAILURE;
    }
    int duration = 2;
    AL::ALVideoRecorderProxy videoRecorderProxy(argv[1],9559);
    videoRecorderProxy.setResolution(0); // kQQVGA ( 160 * 120 ). 0
    videoRecorderProxy.setFrameRate(10);
    videoRecorderProxy.startRecording("/home/nao/recordings/cameras",argv[2]);
    sleep(duration);
    videoRecorderProxy.stopRecording();
    return EXIT_SUCCESS;
}
