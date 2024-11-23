# The compiler to use
CXX = g++

# Compiler flags
CXXFLAGS = -Wall -std=c++11

# The name of the output executable
TARGET = player

# The source file to compile
SRC = player_package/player.cpp

# The object file to generate
OBJ = player.o

# The default rule to compile the target
all: $(TARGET)

# Linking the object file to create the executable
$(TARGET): $(OBJ)
	$(CXX) $(OBJ) -o $(TARGET)

# Rule to compile the source file into an object file
$(OBJ): $(SRC)
	$(CXX) $(CXXFLAGS) -c $(SRC)

# Clean up generated files
clean:
	rm -f $(OBJ) $(TARGET)

# Optional: to remove backup or temporary files
.PHONY: clean
