# Configuration #
# Compilator
CC		= gcc

# Executables to build 
EXEC		= svmTrain

# Sources files
# Source code directory 
SRCDIR		= ../../src
# Include directories
INCLUDEDIRS 	= ../../include ../../include/kmlocal

# Compilation and link flags
CFLAGS 		= $(patsubst %,-I%,$(subst :, ,$(INCLUDEDIRS)))
LDFLAGS 	= -lsvm -lkmeans 

.PHONY: clean cleanall

all: $(EXEC)
$(EXEC): main.o naokmeans.o naosvm.o 
	$(CC) -Wall -o $@  $^ -L../../lib $(LDFLAGS)
main.o: main.cpp 
	$(CC) -Wall -o $@ -c $< $(CFLAGS)
naokmeans.o: $(SRCDIR)/naokmeans.cpp
	$(CC) -Wall -o $@ -c $< $(CFLAGS)
naosvm.o: $(SRCDIR)/naosvm.cpp
	$(CC) -Wall -o $@ -c $< $(CFLAGS)
clean:
	rm -f *~
cleanall: clean
	rm -f *.o $(EXEC)
