GCC = g++
CFLAGS = -Wall -lm
all : demo tests
tests : test1 test2 test3
test1 : demo
	./demo 0 3
test2 : demo
	./demo 2.5 3
test3 : demo
	./demo 2.5 5
demo : Poisson.o
	$(GCC) $(CFLAGS) -o demo Poisson.o
Poisson.o : Poisson.cpp
	$(GCC) $(CFLAGS) -c Poisson.cpp
clean :
	rm demo Poisson.o