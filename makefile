# Compiler
CXX = g++

# Compiler flags
CXXFLAGS = -Wall -std=c++17 -lstdc++fs

# Executable names
TARGETS = player GS

# Source files for each executable
PLAYER_SRC = player_package/player.cpp player_package/commands.cpp utils.cpp
GS_SRC = server/GS.cpp

# Object files for each executable
PLAYER_OBJ = player_package/player.o player_package/commands.o utils.o
GS_OBJ = server/GS.o utils.o

# Directories to create
directories = player_package/games player_package/scoreboards SCORES GAMES

# Default target to build all executables
all: create_dirs $(TARGETS)

# Rule to create required directories
create_dirs:
	@mkdir -p $(directories)

# Rule to build the 'player' executable
player: $(PLAYER_OBJ)
	$(CXX) $(PLAYER_OBJ) -o player

# Rule to build the 'GS' executable
GS: $(GS_OBJ)
	$(CXX) $(GS_OBJ) -o GS

# Rules to compile source files into object files
player.o: player_package/player.cpp
	$(CXX) $(CXXFLAGS) -c player_package/player.cpp -o player.o

commands.o: player_package/commands.cpp
	$(CXX) $(CXXFLAGS) -c player_package/commands.cpp -o commands.o

utils.o: utils.cpp
	$(CXX) $(CXXFLAGS) -c utils.cpp -o utils.o

GS.o: server/GS.cpp
	$(CXX) $(CXXFLAGS) -c server/GS.cpp -o GS.o

# Clean up generated files
clean:
	rm -f $(PLAYER_OBJ) $(GS_OBJ) $(TARGETS)

lclean:
	rm -f $(PLAYER_OBJ) $(GS_OBJ) $(TARGETS) *.txt

# Rule to empty directories
empty_dirs:
	@for dir in $(directories); do \
		if [ -d $$dir ]; then \
			rm -rf $$dir/*; \
		fi; \
	done

# Specify phony targets
.PHONY: all clean create_dirs empty_dirs
