CC=g++
all: main
INCLUDE_DIRS=-I../include \
-I../include/kmlocal \
-I../include/densetrack
main: main.o naokmeans.o naosvm.o naodensetrack.o
	$(CC) -Wall -o main $^ -L../lib -lkmeans -lsvm -ldensetrack
main.o:	main.c naokmeans.h naosvm.h naodensetrack.h
	$(CC) -Wall -o $@ -c $< $(INCLUDE_DIRS)
naosvm.o: naosvm.cpp naosvm.h
	$(CC) -Wall -o $@ -c $< $(INCLUDE_DIRS)
naokmeans.o: naokmeans.cpp naokmeans.h 
	$(CC) -Wall -o $@ -c $< $(INCLUDE_DIRS)
naodensetrack.o: naodensetrack.cpp naodensetrack.h 
	$(CC) -Wall -o $@ -c $< $(INCLUDE_DIRS)
clean:
	rm -f *~
cleanall: clean
	rm -f *.o main
