CC=g++
all: libsvm.so
libsvm.so: svm.o
	$(CC) -Wall -shared svm.o -o libsvm.so
svm.o: svm.cpp svm.h
	$(CC) -Wall -fPIC -c $< -o $@
clean:
	rm -f *~
cleanall: clean
	rm -f *.o *.so
