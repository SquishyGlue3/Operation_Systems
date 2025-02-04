# Compiler
CXX = g++
CC = gcc

# Compiler flags
COMMON_FLAGS = -g -Wall -lpthread
CXXFLAGS = $(COMMON_FLAGS)
CFLAGS = $(COMMON_FLAGS)

# Profiling and Coverage Flags
PROFILE_FLAGS = -pg
COVERAGE_FLAGS = -fprofile-arcs -ftest-coverage

# Targets
TARGETS = singleton race guard_example graph

# All target
all: $(TARGETS)
#--------------------------------------------------------------
# Singleton target
singleton: singleton.o
	$(CXX) -o singleton singleton.o $(CXXFLAGS)

singleton.o: singleton.cpp
	$(CXX) -c singleton.cpp $(CXXFLAGS)

#--------------------------------------------------------------

# Race condition target
race: race.o
	$(CC) -o race race.o $(CFLAGS)

race.o: race.c
	$(CC) -c race.c $(CFLAGS)

#--------------------------------------------------------------

# Hello target
hello: hello.o
	$(CXX) -o hello hello.o $(CXXFLAGS)

#--------------------------------------------------------------

# Guard example target
guard_example: guard.o
	$(CXX) -o guard_example guard.o $(CXXFLAGS)

guard.o: guard.cpp
	$(CXX) -c guard.cpp $(CXXFLAGS)

#--------------------------------------------------------------

# Graph target
graph: graph3.o
	$(CXX) -o graph graph3.o $(CXXFLAGS)

graph3.o: graph3.cpp
	$(CXX) -c graph3.cpp $(CXXFLAGS)

#Targil 3
run_graph: graph
	@./graph -v 3 -e 2 -s 15

#--------------------------------------------------------------

# Clean target
clean:
	rm -f *.o $(TARGETS) gmon.out *.gcno *.gcda *.gcov hello clean_coverage clean_prof clean_valgrind

#--------------------------------------------------------------

# Profiling (gprof)
prof: CXXFLAGS += $(PROFILE_FLAGS)
prof: CFLAGS += $(PROFILE_FLAGS)

#Targil 4
prof: clean $(TARGETS)
	./graph -v 3 -e 2 -s 15
	gprof graph gmon.out > gprof_report.txt

clean_prof:
	rm -f gmon.out gprof_report.txt

#--------------------------------------------------------------

# Code Coverage (gcov)
coverage: CXXFLAGS += $(COVERAGE_FLAGS)
coverage: CFLAGS += $(COVERAGE_FLAGS)

#Targil 4
coverage: clean $(TARGETS)
	./singleton
	./race
	./guard_example
	./graph -v 3 -e 2 -s 15
	gcov *.cpp *.c

clean_coverage:
	rm -f *.gcno *.gcda *.gcov

#--------------------------------------------------------------

# Valgrind targets
valgrind: CXXFLAGS = $(COMMON_FLAGS)  # Override profiling flags

#Targil 4
valgrind: graph
	valgrind ./graph -v 3 -e 2 -s 15

#Targil 4 - Valgrind CallGraph target
valgrind_callgraph: graph
	valgrind --tool=callgrind --callgrind-out-file=callgrind.out ./graph -v 3 -e 2 -s 15
	callgrind_annotate callgrind.out > callgrind_report.txt

#Targil 5 - valgrind hello
valgrind_hello: hello
	valgrind ./hello

#Targil 6 - Valgrind with outside debugger
valgrind_gdb: graph
	valgrind --vgdb=yes --vgdb-error=0 ./graph -v 3 -e 2 -s 15

#Targil 6 - Run the debugger
gdb:
	gdb ./graph

#Targil 7 - Valgrind Race condition
valgrind_race: race # we can see that the error
	valgrind --tool=helgrind ./race 



clean_valgrind:
	rm -f massif.out valgrind_callgraph.txt callgrind.out callgrind_report.txt

#--------------------------------------------------------------

# Phony targets
.PHONY: all clean prof clean_prof coverage clean_coverage valgrind valgrind_massif clean_valgrind
