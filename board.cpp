// Board.cpp file for the sudoku project part A
// Rachel Barume, Kyle Murrah, Zachary Diringer
// Declaration and implementation of the board class

//import statements
#include <iostream>
#include <limits.h>
#include "d_matrix.h"
#include "d_except.h"
#include <list>
#include <fstream>

using namespace std;

//constants for our class

typedef int ValueType;  // type of value in each cell
const int Blank = -1;   //  cell is blank indicator
const int SquareSize = 3;
const int BoardSize = SquareSize * SquareSize;
const int MinValue = 1;
const int MaxValue = 9;

class board
{

public:

    board(int); //constructor
    void clear(); //clear the board
    void initialize(ifstream& fin); //initialize the board
    void print(); // print out the board
    bool isBlank(int, int); //check if cell is blank
    ValueType getCell(int, int); //return cell value

    // update and remove values
    void setCell(int, int, int);
    void clearCell(int, int);

    // print conflicts
    void printConflicts();

    // returns true if puzzle is solved
    bool solved();

private:
    matrix<ValueType> value; //create datatype for board

    //checking if value is in row, col, or square for conflicts
    matrix<bool> conflictsRow;
    matrix<bool> conflictsCol;
    matrix<bool> conflictsSqr;

    void clearConflicts();
    void addConflict(int i, int j, int val);
    void removeConflict(int i, int j, int val);
}; //end class

board::board(int sqSize)
    : value(BoardSize + 1, BoardSize + 1),
    conflictsRow(BoardSize + 1, MaxValue + 1, false),
    conflictsCol(BoardSize + 1, MaxValue + 1, false),
    conflictsSqr(BoardSize + 1, MaxValue + 1, false)
{
    clear();
} // end board 

// Return the square number of cell i,j (counting from left to right,
// top to bottom. Note that i and j each go from 1 to BoardSize
int squareNumber(int i, int j)
{
    // Note that (int) i/SquareSize and (int) j/SquareSize are the x-y
    // coordinates of the square that i,j is in.
    return SquareSize * ((i - 1) / SquareSize) + (j - 1) / SquareSize + 1;
} //end squareNumber

void board::clear()
{

    for (int i = 1; i <= BoardSize; i++)
        for (int j = 1; j <= BoardSize; j++)
            value[i][j] = Blank;

    clearConflicts();
} //end clear

void board::clearConflicts() //clear conflicts function
{
    for (int i = 1; i <= BoardSize; i++)
    {
        for (int v = 1; v <= MaxValue; v++)
        {
            conflictsRow[i][v] = false;
            conflictsCol[i][v] = false;
            conflictsSqr[i][v] = false;
        } //end for
    } //end for
} //end clearConflicts

ValueType board::getCell(int i, int j) //return a cell value
{
    if (i >= 1 && i <= BoardSize && j >= 1 && j <= BoardSize)
        return value[i][j];
    else
        throw rangeError("bad value in getCell");
} // end getCell

bool board::isBlank(int i, int j) //check if cell is blank
{
    if (i < 1 || i > BoardSize || j < 1 || j > BoardSize)
        throw rangeError("bad value in setCell");
    return (getCell(i, j) == Blank);
} //end isBlank

void board::addConflict(int i, int j, int val) // add conflicts
{
    if (val < MinValue || val > MaxValue) return;

    conflictsRow[i][val] = true;
    conflictsCol[j][val] = true;
    int sq = squareNumber(i, j);
    conflictsSqr[sq][val] = true;
} //end addConflicts

void board::removeConflict(int i, int j, int val) //remove conflicts
{
    if (val < MinValue || val > MaxValue) return;

    conflictsRow[i][val] = false;
    conflictsCol[j][val] = false;
    int sq = squareNumber(i, j);
    conflictsSqr[sq][val] = false;
} //end removeConflicts

void board::setCell(int i, int j, int val) //reset a cell to a new value
{
    if (i < 1 || i > BoardSize || j < 1 || j > BoardSize)
        throw rangeError("bad value in setCell");

    ValueType old = value[i][j];
    if (old != Blank)
        removeConflict(i, j, old);

    value[i][j] = val;

    if (val != Blank)
        addConflict(i, j, val);
} // end setCell

void board::clearCell(int i, int j) //clear value in a cell
{
    if (i < 1 || i > BoardSize || j < 1 || j > BoardSize)
        throw rangeError("bad value in clearCell");

    ValueType old = value[i][j];
    if (old != Blank)
    {
        removeConflict(i, j, old);
        value[i][j] = Blank;
    } //end if
} //end clearCell

void board::initialize(ifstream& fin) //initializes board
{
    char ch;
    clear();

    for (int i = 1; i <= BoardSize; i++)
    {
        for (int j = 1; j <= BoardSize; j++)
        {
            fin >> ch;
            if (ch != '.')
                setCell(i, j, ch - '0');
        } //end for 
    } //end for 
} //end initialize

void board::print()
{

    for (int i = 1; i <= BoardSize; i++)
    {

        if ((i - 1) % SquareSize == 0)
        {

            cout << " -";

            for (int j = 1; j <= BoardSize; j++)
                cout << "---";

            cout << "-";
            cout << endl;
        } //end if

        for (int j = 1; j <= BoardSize; j++)
        {
            if ((j - 1) % SquareSize == 0)
                cout << "|";

            if (!isBlank(i, j))
                cout << " " << getCell(i, j) << " ";

            else
                cout << " ";

        } //end for

        cout << "|";
        cout << endl;

    } //end for

    cout << " -";

    for (int j = 1; j <= BoardSize; j++)
        cout << "---";

    cout << "-";
    cout << endl;
} //end print


void board::printConflicts() //print all of the conflicts
{
    cout << "\nRow Conflicts (digits 1-9):\n";
    for (int r = 1; r <= BoardSize; r++)
    {
        cout << "Row " << r << ": ";
        for (int v = 1; v <= MaxValue; v++)
        {
            cout << (conflictsRow[r][v] ? "T" : "F") << " ";
        }
        cout << "\n";
    } //end for

    cout << "\nColumn Conflicts (digits 1-9):\n";
    for (int c = 1; c <= BoardSize; c++)
    {
        cout << "Col " << c << ": ";
        for (int v = 1; v <= MaxValue; v++)
        {
            cout << (conflictsCol[c][v] ? "T" : "F") << " ";
        }
        cout << "\n";
    } //end for

    cout << "\nSquare Conflicts (digits 1-9):\n";
    for (int s = 1; s <= BoardSize; s++)
    {
        cout << "Sq " << s << ": ";
        for (int v = 1; v <= MaxValue; v++)
        {
            cout << (conflictsSqr[s][v] ? "T" : "F") << " ";
        }
        cout << "\n";
    } //end for

    cout << endl;
} //end printConflicts

bool board::solved() //check if the board is solve
{
    //if all of the cells are not blank and there are no conflicts
    //board will be solved 

    for (int i = 1; i <= BoardSize; i++)
        for (int j = 1; j <= BoardSize; j++)
            if (isBlank(i, j))
                return false;

    for (int r = 1; r <= BoardSize; r++)
        for (int v = 1; v <= MaxValue; v++)
            if (!conflictsRow[r][v]) return false;

    for (int c = 1; c <= BoardSize; c++)
        for (int v = 1; v <= MaxValue; v++)
            if (!conflictsCol[c][v]) return false;

    for (int s = 1; s <= BoardSize; s++)
        for (int v = 1; v <= MaxValue; v++)
            if (!conflictsSqr[s][v]) return false;

    return true;
} //end solved

int main()
{
    ifstream fin;
    string fileNames[] = { "sudoku1.txt", "sudoku2.txt", "sudoku3.txt" };

    for (int f = 0; f < 3; f++)
    {
        fin.open(fileNames[f].c_str());

        if (!fin)
        {
            cerr << "Cannot open " << fileNames[f] << endl;
            exit(1);
        }

        try
        {
            board b1(SquareSize);

            while (fin && fin.peek() != 'Z')
            {
                b1.initialize(fin);
                b1.print();
                b1.printConflicts();
            }
        } //end try
        catch (indexRangeError& ex)
        {
            cout << ex.what() << endl;
            exit(1);
        }

        fin.close();
        cout << endl;
    } //end for

    return 0;
} //end main

//end file
