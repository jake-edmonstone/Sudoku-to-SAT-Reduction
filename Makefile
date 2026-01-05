CXX      := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -O2

SRC_DIR  := src

all: sud2sat sud2sat1 sat2sud sat2sud1

sud2sat: $(SRC_DIR)/sud2sat.o $(SRC_DIR)/sudoku.o
	$(CXX) $(CXXFLAGS) -o $@ $^

sud2sat1: $(SRC_DIR)/sud2sat1.o $(SRC_DIR)/sudoku.o
	$(CXX) $(CXXFLAGS) -o $@ $^

sat2sud: $(SRC_DIR)/sat2sud.o $(SRC_DIR)/sudoku.o
	$(CXX) $(CXXFLAGS) -o $@ $^

sat2sud1: $(SRC_DIR)/sat2sud1.o $(SRC_DIR)/sudoku.o
	$(CXX) $(CXXFLAGS) -o $@ $^

$(SRC_DIR)/%.o: $(SRC_DIR)/%.cpp $(SRC_DIR)/sudoku.h
	$(CXX) $(CXXFLAGS) -c -o $@ $<

clean:
	rm -f $(SRC_DIR)/*.o \
		sud2sat sud2sat1 sat2sud sat2sud1 \
		*.cnf \
		solution.txt \
		stat.txt \
		assign.txt
	
.PHONY: all clean
