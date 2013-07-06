CC=g++ -O3
all: libkmeans
libkmeans: KM_ANN.o KMeans.o KMterm.o KMrand.o KCutil.o KCtree.o KMdata.o KMcenters.o KMfilterCenters.o KMlocal.o
	$(CC) -shared -o libkmeans.so KM_ANN.o KMeans.o KMterm.o KMrand.o KCutil.o KCtree.o KMdata.o KMcenters.o KMfilterCenters.o KMlocal.o
KM_ANN.o: KM_ANN.cpp KM_ANN.h
	$(CC) -Wall -c -fPIC -Wall -DASSERT KM_ANN.cpp
KMeans.o: KMeans.h
	$(CC) -Wall -c -fPIC -Wall -DASSERT KMeans.cpp
KMterm.o: KMterm.cpp KMterm.h
	$(CC) -c -fPIC -Wall -DASSERT KMterm.cpp
KMrand.o: KMrand.cpp KMrand.h
	$(CC) -c -fPIC -Wall -DASSERT KMrand.cpp
KCutil.o: KCutil.cpp KCutil.h
	$(CC) -c -fPIC -Wall -DASSERT KCutil.cpp
KCtree.o: KCtree.cpp KCtree.h
	$(CC) -c -fPIC -O3 -Wall -DASSERT KCtree.cpp
KMdata.o: KMdata.cpp KMdata.h
	$(CC) -c -fPIC -Wall -DASSERT KMdata.cpp
KMcenters.o: KMcenters.cpp KMcenters.h
	$(CC) -c -fPIC -Wall -DASSERT KMcenters.cpp
KMfilterCenters.o: KMfilterCenters.cpp KMfilterCenters.h
	$(CC) -c -fPIC -Wall -DASSERT KMfilterCenters.cpp
KMlocal.o: KMlocal.cpp KMlocal.h
	$(CC) -c -fPIC -Wall -DASSERT KMlocal.cpp
clean:
	rm -f *~
cleanall: clean
	rm -f *.o
