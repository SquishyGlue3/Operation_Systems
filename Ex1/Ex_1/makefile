GCC = g++
CFLAGS = -Wall -g
all : prog1 prog2 prog3
prog1 : Core_usage_1.cpp
	$(GCC) $(CFLAGS) -o prog1 Core_usage_1.cpp
prog2 : Core_usage_2.cpp
	$(GCC) $(CFLAGS) -o prog2 Core_usage_2.cpp
prog3 : Core_usage_3.cpp
	$(GCC) $(CFLAGS) -o prog3 Core_usage_3.cpp
PHONY : clean
clean :
	rm -f prog1 prog2 prog3