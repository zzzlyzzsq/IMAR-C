all: densetracktofile
main.o: main.cpp
	g++ -c main.cpp -I../../include -I../../include/kmlocal
IplImagePyramid.o: IplImagePyramid.cpp
	g++ -c IplImagePyramid.cpp -I../../include
IplImageWrapper.o: IplImageWrapper.cpp
	g++ -c IplImageWrapper.cpp -I../../include
densetracktofile: main.o IplImagePyramid.o IplImageWrapper.o
	g++ -o densetracktofile main.o IplImagePyramid.o IplImageWrapper.o `pkg-config --cflags --libs opencv`
clean:
	rm *.o densetracktofile