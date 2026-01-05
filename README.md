# Sudoku to SAT Reduction

Translate 9x9 Sudoku puzzles to SAT (DIMACS CNF) via a Karp reduction, and decode SAT solver assignments back into solved grids.
The project includes both minimal and extended encodings as described in [this paper](https://sat.inesc-id.pt/~ines/publications/aimath06.pdf).

## Build
```bash
make
```

To remove generated files:
```bash
make clean
```

## Tools
- `sud2sat`, `sat2sud`: minimal encoding.
- `sud2sat1`, `sat2sud1`: extended encoding.

All commands read from STDIN and write to STDOUT.

## Input Format
Sudoku puzzles are encoded as 81 characters (whitespace ignored):
- Digits `1`-`9` are fixed entries.
- Any of `0`, `.`, `*`, `?` indicates an empty cell.

## Example Workflow
```bash
$ cat puzzle.txt
...1.5...
14....67.
.8...24..
.63.7..1.
9.......3
.1..9.52.
..72...8.
.26....35
...4.9...
$ ./sud2sat <puzzle.txt >puzzle.cnf
$ minisat puzzle.cnf assign.txt >stat.txt
$ ./sat2sud <assign.txt >solution.txt
$ cat solution.txt
672 145 398
145 983 672
389 762 451
263 574 819
958 621 743
714 398 526
597 236 184
426 817 935
831 459 267
```

`sat2sud` expects MiniSAT-style output: a `SAT` line followed by a line of signed
variable assignments ending with `0`. If the solver returns `UNSAT`, there is no solution.

## Repository Layout
- `src/` — C++ source files.
- `Makefile` — build rules for all executables.
- `puzzle.txt`, `puzzles.txt`, `hardpuzzles.txt` — sample Sudoku inputs.
- `report.pdf` — performance report comparing encodings.
