# libraries
ARCH=-m32
CC=g++ -O3 $(ARCH)

# CROSS-COMPILATION
ifeq ($(ARCH),-m32)
	CTC := 	/home/noox/Documentos/stages/ete_2013-nanoInnov/activity_recognition/robot/linux64-nao-atom-cross-toolchain-1.14.3
	LIBBOOST := boost_program_options-mt boost_regex-mt boost_system-mt boost_filesystem-mt
	LIBS_PATHS := $(CTC)/i686-aldebaran-linux-gnu $(CTC)/sysroot $(CTC)/sysroot/usr/lib
else
	LIBBOOST := boost_program_options boost_regex boost_system boost_filesystem
endif

LIBS := \
	$(LIBBOOST) \
	opencv_core opencv_highgui opencv_video opencv_imgproc \
	avformat avdevice avutil avcodec swscale

LDLIBS = $(addprefix -l, $(LIBS) $(LIBS_$(notdir $*)))
LDLIBS_PATHS = $(addprefix -L, $(LIBS_PATHS))

# BUILD
all: libdensetrack.so
libdensetrack.so: IplImagePyramid.o  IplImageWrapper.o
	$(CC) -Wall -shared -o $@ $^ $(LDLIBS_PATHS) $(LDLIBS)
IplImagePyramid.o: IplImagePyramid.cpp IplImagePyramid.h
	$(CC) -Wall -fPIC -o $@ -c $< 
IplImageWrapper.o: IplImageWrapper.cpp IplImageWrapper.h 
	$(CC) -Wall -fPIC -o $@ -c $< 
clean:
	rm -f *~ *.o
mrproper: clean
	rm -f libdensetrack.so
