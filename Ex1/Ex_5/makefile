GCC = g++
FLAGS = -pg -Wall
all : main report

main : main.o
	$(GCC) $(FLAGS) -o main main.o profiling.cpp

main.o : main.cpp
	$(GCC) $(FLAGS) -c main.cpp

report : main
	./main 1 100
	./main 1 1000
	./main 1 10000
	gprof main gmon.out > report.txt
	cat report.txt
PHONY : clean all
clean : 
	rm -f *.o main report gmon.out