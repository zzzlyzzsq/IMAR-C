# Configuration #
# Compilator
CC		= g++

# Executables to build 
EXEC		= actmngt

# Sources files

# Source code directory
SRCDIRS 	= ../src

# Include directories
INCLUDEDIRS 	= ../include ../include/kmlocal ../include/densetrack

# Compilation and link flags
CFLAGS 		= $(patsubst %,-I%,$(subst :, ,$(INCLUDEDIRS)))
LDFLAGS 	= -lsvm -lkmeans -ldensetrack

.PHONY: clean cleanall

all: $(EXEC)
$(EXEC): main.o naomngt.o naokmeans.o naosvm.o naodensetrack.o
	$(CC) -Wall -o $@  $^ -L../lib $(LDFLAGS)
main.o: main.cpp 
	$(CC) -Wall -o $@ -c $< $(CFLAGS)
naomngt.o: $(SRCDIRS)/naomngt.cpp 
	$(CC) -Wall -o $@ -c $< $(CFLAGS)
naokmeans.o: $(SRCDIRS)/naokmeans.cpp
	$(CC) -Wall -o $@ -c $< $(CFLAGS)
naosvm.o: $(SRCDIRS)/naosvm.cpp
	$(CC) -Wall -o $@ -c $< $(CFLAGS)
naodensetrack.o: $(SRCDIRS)/naodensetrack.cpp
	$(CC) -Wall -o $@ -c $< $(CFLAGS)
clean:
	rm -f *~
cleanall: clean
	rm -f *.o $(EXEC)
