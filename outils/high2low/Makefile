CC 		= g++ 
EXEC		= high2low
CFLAGS 		= `pkg-config --cflags opencv`
LDFLAGS 	= `pkg-config --libs opencv`

all: $(EXEC)
$(EXEC): 
	$(CC) -Wall -o $@ main.cpp $(CFLAGS) $(LDFLAGS)
clean: 
	rm -rf *~
cleanall: clean
	rm -rf $(EXEC) *.o
