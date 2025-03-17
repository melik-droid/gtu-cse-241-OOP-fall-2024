#ifndef CURSOR_H
#define CURSOR_H

#include <string>
#include <iostream>
#include <vector>

#include "Cell.h"
#include "AnsiTerminal.h"

using std::string;
using std::vector;
using std::cout;
using std::endl;

class Cursor{
    public:

    Cursor();
    int moveCursor(char key) ; 
    
    bool setCell(int row_idx, int col_idx);  

    // setter and getter functions
    inline int getRow() const { return row; } 
    inline int getCol() const { return col; }
    inline int getRowOffset() const { return row_offset; }
    inline int getColOffset() const { return col_offset; }
    inline int getRowIdx() const { return row + row_offset; }
    inline int getColIdx() const { return col + col_offset; }


    inline void setRow(int row) { this->row = row; }
    inline void setCol(int col) { this->col = col; }
    inline void setRowOffset(int row_offset) { this->row_offset = row_offset; }
    inline void setColOffset(int col_offset) { this->col_offset = col_offset; }
    
    
    private:
    
    int row;
    int col;

    int row_offset;
    int col_offset;


    char key = '\0';
    
};

#endif