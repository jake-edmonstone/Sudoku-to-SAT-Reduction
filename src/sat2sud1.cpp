#include "sudoku.h"
#include <iostream>

int main() {
  using namespace sudoku;

  MinisatResult res = read_minisat_output(std::cin);

  if (res.status != SatStatus::Sat) {
    std::cerr << "UNSAT or UNKNOWN instance; no Sudoku solution.\n";
    return 1;
  }

  SudokuGrid solved = assignment_to_grid(res);
  write_grid(std::cout, solved);

  return 0;
}
