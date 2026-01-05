#include "sudoku.h"

#include <cctype>
#include <iostream>
#include <stdexcept>

namespace sudoku {

// ======== Puzzle I/O ================================================

SudokuGrid read_puzzle(std::istream &in) {
  SudokuGrid grid;
  std::string chars;
  chars.reserve(81);

  while (chars.size() < 81) {
    int c = in.get();
    if (c == EOF)
      break;
    if (std::isspace(c))
      continue; // ignore whitespace

    char ch = static_cast<char>(c);

    // Acceptable characters:
    //  '1'..'9' = digits
    //  '0', '.', '*', '?' = empty
    if ((ch >= '1' && ch <= '9') || ch == '0' || ch == '.' || ch == '*' ||
        ch == '?') {
      chars.push_back(ch);
    } else {
      throw std::runtime_error("Invalid character in puzzle input");
    }
  }

  if (chars.size() != 81) {
    throw std::runtime_error("Puzzle contains fewer than 81 cells");
  }

  // Fill grid
  int idx = 0;
  for (int row = 0; row < 9; row++) {
    for (int col = 0; col < 9; col++) {
      char ch = chars[idx++];
      if (ch >= '1' && ch <= '9') {
        grid.cells[row][col] = ch - '0';
      } else {
        grid.cells[row][col] = 0; // wildcard
      }
    }
  }
  return grid;
}

void write_grid(std::ostream &out, const SudokuGrid &grid) {
  for (int row = 0; row < 9; row++) {
    for (int col = 0; col < 9; col++) {
      int val = grid.cells[row][col];
      out << val;
      if (col == 2 || col == 5)
        out << ' ';
    }
    out << '\n';
  }
}

// ======== Variable encoding =========================================

int encode_var(int row, int col, int digit) {
  return 81 * (row - 1) + 9 * (col - 1) + (digit - 1) + 1;
}

void decode_var(int var, int &row, int &col, int &digit) {
  int v = var - 1;

  row = v / 81;
  col = (v / 9) % 9;
  digit = v % 9;

  row += 1;
  col += 1;
  digit += 1;
}

// ======== CNF generation ============================================

void write_only_minimal_cnf(std::ostream &out) {
  // At least one digit in each cell:
  for (int x = 1; x <= 9; x++) {
    for (int y = 1; y <= 9; y++) {
      for (int z = 1; z <= 9; z++) {
        out << encode_var(x, y, z) << " ";
      }
      out << "0\n";
    }
  }

  // At most one per row:
  for (int y = 1; y <= 9; y++) {
    for (int z = 1; z <= 9; z++) {
      for (int x = 1; x <= 8; x++) {
        for (int i = x + 1; i <= 9; i++) {
          out << -encode_var(x, y, z) << " " << -encode_var(i, y, z) << " 0\n";
        }
      }
    }
  }

  // At most one per column:
  for (int x = 1; x <= 9; x++) {
    for (int z = 1; z <= 9; z++) {
      for (int y = 1; y <= 8; y++) {
        for (int i = y + 1; i <= 9; i++) {
          out << -encode_var(x, y, z) << " " << -encode_var(x, i, z) << " 0\n";
        }
      }
    }
  }

  // At most one per 3×3 block:
  for (int z = 1; z <= 9; z++) {
    for (int bi = 0; bi < 3; bi++) {
      for (int bj = 0; bj < 3; bj++) {
        for (int i1 = 0; i1 < 9; i1++) {
          int r1 = 3 * bi + 1 + (i1 / 3);
          int c1 = 3 * bj + 1 + (i1 % 3);

          for (int i2 = i1 + 1; i2 < 9; i2++) {
            int r2 = 3 * bi + 1 + (i2 / 3);
            int c2 = 3 * bj + 1 + (i2 % 3);

            out << -encode_var(r1, c1, z) << " " << -encode_var(r2, c2, z)
                << " 0\n";
          }
        }
      }
    }
  }
}

void write_only_extended_cnf(std::ostream &out) {
  // At most one number in each entry:
  for (int x = 1; x <= 9; x++) {
    for (int y = 1; y <= 9; y++) {
      for (int z = 1; z <= 8; z++) {
        for (int i = z + 1; i <= 9; i++) {
          out << -encode_var(x, y, z) << " " << -encode_var(x, y, i) << " 0\n";
        }
      }
    }
  }

  // Each number appears at least once in each row:
  for (int y = 1; y <= 9; y++) {
    for (int z = 1; z <= 9; z++) {
      for (int x = 1; x <= 9; x++) {
        out << encode_var(x, y, z) << " ";
      }
      out << "0\n";
    }
  }

  // Each number appears at least once in each column:
  for (int x = 1; x <= 9; x++) {
    for (int z = 1; z <= 9; z++) {
      for (int y = 1; y <= 9; y++) {
        out << encode_var(x, y, z) << " ";
      }
      out << "0\n";
    }
  }

  // Each number appears at least once in each 3×3 sub-grid:
  for (int z = 1; z <= 9; z++) {
    for (int bi = 0; bi < 3; bi++) {
      for (int bj = 0; bj < 3; bj++) {
        for (int x = 1; x <= 3; x++) {
          for (int y = 1; y <= 3; y++) {
            int r = 3 * bi + x;
            int c = 3 * bj + y;
            out << encode_var(r, c, z) << " ";
          }
        }
        out << "0\n";
      }
    }
  }
}

void write_minimal_cnf(std::ostream &out, const SudokuGrid &puzzle) {
  constexpr int kVars = 729;  // 9×9×9
  constexpr int kBase = 8829; // from the paper
  int givens = 0;

  // Count givens
  for (int x = 0; x < 9; x++)
    for (int y = 0; y < 9; y++)
      if (puzzle.cells[x][y] != 0)
        givens++;

  int total_clauses = kBase + givens;

  // Emit DIMACS header
  out << "p cnf " << kVars << " " << total_clauses << "\n";

  write_only_minimal_cnf(out);

  // Unit clauses for given clues
  for (int x = 0; x < 9; x++) {
    for (int y = 0; y < 9; y++) {
      int d = puzzle.cells[x][y];
      if (d != 0) {
        out << encode_var(x + 1, y + 1, d) << " 0\n";
      }
    }
  }
}

void write_extended_cnf(std::ostream &out, const SudokuGrid &puzzle) {
  constexpr int kVars = 729;   // 9×9×9
  constexpr int kBase = 11988; // from the paper
  int givens = 0;

  // Count givens
  for (int x = 0; x < 9; x++)
    for (int y = 0; y < 9; y++)
      if (puzzle.cells[x][y] != 0)
        givens++;

  int total_clauses = kBase + givens;

  // Emit DIMACS header
  out << "p cnf " << kVars << " " << total_clauses << "\n";

  write_only_minimal_cnf(out);
  write_only_extended_cnf(out);

  // Unit clauses for given clues
  for (int x = 0; x < 9; x++) {
    for (int y = 0; y < 9; y++) {
      int d = puzzle.cells[x][y];
      if (d != 0) {
        out << encode_var(x + 1, y + 1, d) << " 0\n";
      }
    }
  }
}

// ======== minisat output parsing ====================================

MinisatResult read_minisat_output(std::istream &in) {
  MinisatResult res;

  std::string token;
  if (!(in >> token)) {
    throw std::runtime_error("Unexpected EOF while reading minisat output");
  }

  if (token == "UNSAT") {
    res.status = SatStatus::Unsat;
    return res;
  }

  if (token != "SAT") {
    throw std::runtime_error("Expected SAT or UNSAT in minisat output");
  }

  res.status = SatStatus::Sat;

  // Now read literals until we see 0
  int lit;
  bool sawZero = false;

  while (in >> lit) {
    if (lit == 0) {
      sawZero = true;
      break;
    }
    res.assignment.push_back(lit);
  }

  if (!sawZero) {
    throw std::runtime_error("Malformed minisat output: missing terminating 0");
  }

  return res;
}

SudokuGrid assignment_to_grid(const MinisatResult &result) {
  if (result.status != SatStatus::Sat) {
    throw std::runtime_error("assignment_to_grid: result is not SAT");
  }

  SudokuGrid grid; // default-initialized to zeros

  for (int lit : result.assignment) {
    if (lit <= 0) {
      continue; // ignore negative literals
    }

    int row, col, digit;
    decode_var(lit, row, col, digit);

    // row, col, digit are 1..9; grid is 0-indexed
    grid.cells[row - 1][col - 1] = digit;
  }

  return grid;
}

} // namespace sudoku
