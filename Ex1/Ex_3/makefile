GCC = g++
CFLAGS = -Wall
all : buildLibrary
buildLibrary :
	$(GCC) $(CFLAGS) -fPIC -c Ex_2/Poisson.cpp -o Poisson.o
	$(GCC) $(CFLAGS) -shared -o libPoisson.so Poisson.o
main : buildLibrary
	$(GCC) $(CFLAGS) Ex_3/main.cpp -o main -L. -lPoisson
clean :
	rm -f *.o *.so main
