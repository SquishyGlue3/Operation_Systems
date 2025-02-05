# Compiler
CXX = g++
# Compiler flags
CXXFLAGS = -pthread -Wall -Wextra -O2
# Output executable name
TARGET = singleton_guard
# Source file
SRC = singleton_guard.cpp

# Default rule: Compile the program
all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRC)

# Run rule: Compile and run the program
run: $(TARGET)
	./$(TARGET)

# Clean rule: Remove the compiled binary
clean:
	rm -f $(TARGET)
