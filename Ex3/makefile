GCC = g++
FLAGS = -std=c++14 -Wall -Wextra -Wvla -pthread -g -pg

all: Kosaraju

Kosaraju: Kosaraju.cpp
	$(GCC) $(FLAGS) Kosaraju.cpp -o Kosaraju
run : Kosaraju
	./Kosaraju
gprofDeque: Kosaraju
	gprof Kosaraju gmon.out > analysisDeque.txt
	less analysisDeque.txt
gprofList: Kosaraju
	gprof Kosaraju gmon.out > analysisList.txt
	less analysisList.txt
clean:
	rm -f Kosaraju *.o *.txt gmon.out

