GCC = g++
FLAGS = -std=c++14 -Wall -Wextra -Wvla -pthread -g -pg

all: Kosaraju

Kosaraju: Kosaraju.cpp
	$(GCC) $(FLAGS) Kosaraju.cpp -o Kosaraju
run : Kosaraju
	@echo "Run with 5,5 input"
	@echo "[0,1], [1,2], [2,0], [2,3], [3,4]"
	@echo "-----------------------------"
	@./Kosaraju
gprofDeque: Kosaraju
	gprof Kosaraju gmon.out > analysisDeque.txt
	less analysisDeque.txt
gprofList: Kosaraju
	gprof Kosaraju gmon.out > analysisList.txt
	less analysisList.txt
tidy:
	clang-tidy Kosaraju.cpp -- $(FLAGS)
clean:
	rm -f Kosaraju *.o *.txt gmon.out

