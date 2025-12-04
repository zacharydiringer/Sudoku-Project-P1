// sudoku_loader.cpp
// Full program with robust parsers for two file formats:
// 1) sudoku.txt   -> many puzzles, read sequential characters, ends with 'Z'
// 2) 3sudokuBoards.txt -> many puzzles (one per 81 chars, mixed formatting), ends with 'Z'
//
// Author: regenerated for Zachary Diringer

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cctype>
#include <cstdlib>

using namespace std;

// --------------------
// Board class (minimal, preserves original semantics)
// --------------------
const int SquareSize = 3;
const int BoardSize = SquareSize * SquareSize;
const int Blank = -1;

class board {
public:
    board(int);
    void clear();
    void initializeFromString(const string& s); // new initializer
    void print() const;
    void printConflicts() const; // placeholder - original implementation not required here
    // keep simple setters if you want solver logic added later
private:
    int value[BoardSize + 1][BoardSize + 1];
};

board::board(int sqSize) {
    clear();
}

void board::clear() {
    for (int i = 1; i <= BoardSize; ++i)
        for (int j = 1; j <= BoardSize; ++j)
            value[i][j] = Blank;
}

void board::initializeFromString(const string& s) {
    // s must be length 81 (9x9). We assume caller validated it.
    clear();
    if ((int)s.size() < 81) return; // safety
    int k = 0;
    for (int i = 1; i <= BoardSize; ++i) {
        for (int j = 1; j <= BoardSize; ++j) {
            char ch = s[k++];
            if (ch == '.' || !isdigit(static_cast<unsigned char>(ch))) {
                value[i][j] = Blank;
            }
            else {
                value[i][j] = ch - '0';
            }
        }
    }
}

void board::print() const {
    cout << "\n+";
    for (int c = 0; c < BoardSize; ++c) cout << "---";
    cout << "+\n";
    for (int i = 1; i <= BoardSize; ++i) {
        cout << "|";
        for (int j = 1; j <= BoardSize; ++j) {
            if (value[i][j] == Blank)
                cout << " . ";
            else
                cout << " " << value[i][j] << " ";
        }
        cout << "|\n";
    }
    cout << "+";
    for (int c = 0; c < BoardSize; ++c) cout << "---";
    cout << "+\n";
}

void board::printConflicts() const {
    // Original program had conflict tracking. For parsing/testing purposes,
    // we print a placeholder. If you want full conflict logic, I can add it.
    cout << "(Conflict-check placeholder)\n";
}

// --------------------
// Choice 1 reader: read sequential characters, ignore whitespace, stop at 'Z'
// --------------------
bool readPuzzle81Stream(ifstream& fin, string& puzzle) {
    puzzle.clear();
    char ch;
    while (fin.get(ch)) {
        if (ch == 'Z') { // end marker
            return false;
        }
        if (isspace(static_cast<unsigned char>(ch))) continue;
        puzzle.push_back(ch);
        if (puzzle.size() == 81) return true;
    }
    return false;
}

// --------------------
// Choice 2 reader: robust whole-file parse into 81-char puzzles
// - reads entire file into memory
// - strips whitespace
// - stops at 'Z'
// - slices into 81-char pieces
// --------------------
vector<string> parseFileIntoPuzzles(const string& filename) {
    vector<string> puzzles;
    ifstream fin(filename.c_str(), ios::binary);
    if (!fin) {
        cerr << "Cannot open " << filename << "\n";
        return puzzles;
    }

    // Read whole file into a string
    string raw;
    fin.seekg(0, ios::end);
    size_t sz = static_cast<size_t>(fin.tellg());
    fin.seekg(0, ios::beg);
    raw.reserve(sz + 10);

    string line;
    while (getline(fin, line)) {
        raw += line;
        raw += '\n'; // preserve possible 'Z' that might be alone on a line
    }
    fin.close();

    // Build a clean buffer: remove whitespace, but detect 'Z'
    string buf;
    buf.reserve(raw.size());
    for (char c : raw) {
        if (c == 'Z') break;            // stop at Z (end marker)
        if (isspace(static_cast<unsigned char>(c))) continue; // skip whitespace
        buf.push_back(c);
    }

    // Slice into 81-character puzzles
    size_t pos = 0;
    while (pos + 81 <= buf.size()) {
        string piece = buf.substr(pos, 81);
        // Optional validation: ensure piece contains only digits or dots
        bool valid = true;
        for (char ch : piece) {
            if (!(ch == '.' || isdigit(static_cast<unsigned char>(ch)))) {
                valid = false;
                break;
            }
        }
        if (valid) puzzles.push_back(piece);
        // If invalid, you might want to log or skip — here we skip invalid chunks
        pos += 81;
    }

    return puzzles;
}

// --------------------
// Main
// --------------------
int main() {
    cout << "Choose file parsing mode:\n";
    cout << "1 = sudoku.txt (stream parse, stop at 'Z')\n";
    cout << "2 = 3sudokuBoards.txt (robust whole-file parse, stop at 'Z')\n";
    cout << "Choice: ";

    int choice = 0;
    if (!(cin >> choice)) {
        cerr << "Invalid input\n";
        return 1;
    }

    if (choice == 1) {
        // Mode 1: stream parser (keeps previous behavior)
        const string fname = "sudoku.txt";
        ifstream fin(fname.c_str());
        if (!fin) {
            cerr << "Cannot open " << fname << "\n";
            return 1;
        }

        string puzzle;
        int count = 0;
        while (readPuzzle81Stream(fin, puzzle)) {
            ++count;
            cout << "\n--- Puzzle #" << count << " ---\n";
            board b(SquareSize);
            b.initializeFromString(puzzle);
            b.print();
            b.printConflicts();
        }
        cout << "\nTotal puzzles read: " << count << "\n";
        fin.close();
    }
    else if (choice == 2) {
        // Mode 2: robust whole-file parse
        const string fname = "3sudokuBoards.txt";
        vector<string> puzzles = parseFileIntoPuzzles(fname);
        if (puzzles.empty()) {
            cerr << "No puzzles found (file empty, not present, or invalid format)\n";
            return 1;
        }

        for (size_t i = 0; i < puzzles.size(); ++i) {
            cout << "\n--- Puzzle #" << (i + 1) << " ---\n";
            board b(SquareSize);
            b.initializeFromString(puzzles[i]);
            b.print();
            b.printConflicts();
        }
        cout << "\nTotal puzzles read: " << puzzles.size() << "\n";
    }
    else {
        cerr << "Invalid choice\n";
        return 1;
    }

    return 0;
}
