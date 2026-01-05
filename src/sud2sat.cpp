#include "sudoku.h"
#include <iostream>

int main() {
  using namespace sudoku;

  SudokuGrid puzzle = read_puzzle(std::cin);
  write_minimal_cnf(std::cout, puzzle);

  return 0;
}
