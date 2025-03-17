#ifndef SHEET_H
#define SHEET_H

#include <string>
#include <iostream>
#include <vector>

#include "Cell.h"
#include "AnsiTerminal.h"
#include "Cursor.h"

using std::string;
using std::vector;


class Sheet{
    public:

    Sheet();
    ~Sheet();
    void printSheet();
    void printTable();
    void printGrid();
    void printCursor();
    
    void inputProcess(char key);
    void setCell(); 

    void handleFileOperations();

    inline string getMode() const { return mode; }
    inline void setMode(string mode) { this->mode = mode; }

    const static int START_ROW = 4;
    const static int START_COL = 0;
    
    const static int TOTAL_ROWS = 254;   // Total rows in the grid
    const static int TOTAL_COLS = 63;    // Total columns in the grid
    const static int VIEW_ROWS = 21; // Number of rows visible in the viewport
    const static int VIEW_COLS = 8; // Number of columns visible in the viewport

    const static int CELL_WIDTH = 9;     // Width of each cell
    const static int ROW_LABEL_WIDTH = 3; // Width of the row label

    AnsiTerminal& getTerminal() { return terminal; }
    vector<vector<Cell>>& getData() { return data; }

    private:
    
    vector<vector<Cell>> data;
    AnsiTerminal terminal;
    Cursor cursor;

    string type_space;
    string temp;
    string mode;

    bool changeDone = false;

    void clearTypeSpace();
    string getColumnLabel(int col) const; // Add this new method


    

};

#endif