# libraries 
CC=g++ -O3
LDLIBS = $(addprefix -l, $(LIBS) $(LIBS_$(notdir $*)))
LIBS := \
	boost_program_options boost_regex boost_system boost_filesystem \
	opencv_core opencv_highgui opencv_video opencv_imgproc \
	avformat avdevice avutil avcodec swscale

all: libdensetrack.so
libdensetrack.so: IplImagePyramid.o  IplImageWrapper.o
	$(CC) -Wall -shared -o $@ $^  `pkg-config --libs opencv` $(LDLIBS)
IplImagePyramid.o: IplImagePyramid.cpp IplImagePyramid.h
	$(CC) -Wall -fPIC -o $@ -c $< -I../geometry -I../numeric `pkg-config --cflags opencv`
IplImageWrapper.o: IplImageWrapper.cpp IplImageWrapper.h 
	$(CC) -Wall -fPIC -o $@ -c $< -I../geometry -I../numeric `pkg-config --cflags opencv`
clean:
	rm -f *~ *.o
mrproper: clean
	rm -f libdensetrack.so
