#pragma once

#include <array>
#include <iosfwd>
#include <string>
#include <vector>

namespace sudoku {
// 0 = empty, 1..9 = filled.
struct SudokuGrid {
  std::array<std::array<int, 9>, 9> cells{};
};

// ==== I/O for puzzles and solutions =================================

// Parse a puzzle from an 81-character encoding with arbitrary whitespace.
// Allowed non-whitespace chars:
//   '1'..'9' for digits, '0', '.', '*', '?' for empty.
SudokuGrid read_puzzle(std::istream &in);

// Print a filled Sudoku grid as 9 lines of 9 digits.
void write_grid(std::ostream &out, const SudokuGrid &grid);

// ==== Variable encoding (paper’s (i,j,k) -> integer mapping) =========
//
// For 1 <= i,j,k <= 9, encode as
//   v = 81*(i-1) + 9*(j-1) + (k-1) + 1
// returning v in {1,...,729}.
int encode_var(int row, int col, int digit);

// Inverse of encode_var.
// Precondition: var in {1,...,729}.
// Post: row,col,digit all in {1,...,9}.
void decode_var(int var, int &row, int &col, int &digit);

// ==== CNF generation (minimal vs extended encodings) ================
//
// Both functions produce DIMACS CNF on `out`:
//   p cnf <#vars> <#clauses>
//   <clauses>
//
// Variables: always 729 (9^3), as in the paper.
// Clause counts (without unit clauses for givens) from the paper:
//   - Minimal:  8,829 base clauses.
//   - Extended: 11,988 base clauses.
// Then add one unit clause per given cell.
//
void write_minimal_cnf(std::ostream &out, const SudokuGrid &puzzle);

// Same as above, but for the extended encoding (minimal + extra
// at-most-one and at-least-one constraints as in the paper).
void write_extended_cnf(std::ostream &out, const SudokuGrid &puzzle);

// ==== minisat output parsing and decoding ===========================

enum class SatStatus { Sat, Unsat, Unknown };

// Raw minisat result:
// - status: SAT / UNSAT / UNKNOWN
// - assignment: if SAT, the line of ints ending in 0.
struct MinisatResult {
  SatStatus status = SatStatus::Unknown;
  std::vector<int> assignment;
};

// Read minisat output from `in`.
// Format:
//   SAT\n
//   <l1 l2 ... ln 0>\n
// or:
//   UNSAT\n
MinisatResult read_minisat_output(std::istream &in);

// Convert a SAT assignment back into a filled Sudoku grid.
// Use decode_var on each positive literal that corresponds to a
// (row,col,digit) variable and fill the grid accordingly.
SudokuGrid assignment_to_grid(const MinisatResult &result);

} // namespace sudoku
