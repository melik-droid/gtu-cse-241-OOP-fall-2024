#include <iostream>
#include <string>
#include <vector>
#include <iomanip>

#include "AnsiTerminal.h"
#include "Sheet.h"
#include "Cell.h"
#include "Cursor.h"

using std::string;
using std::vector;
using std::cout;
using std::endl;


Cursor::Cursor() : row(0), col(0), row_offset(0), col_offset(0) // Constructor
{} 



   

int Cursor::moveCursor(char key) {  // move the cursor
    switch(key) {
        case 'U':
            if(row > 0) row--;
            else if(row_offset > 0 ) row_offset--;
            break;
        case 'D':
            if(row < Sheet::VIEW_ROWS - Sheet::START_ROW - 1) row++;
            else if(row_offset < Sheet::TOTAL_ROWS - Sheet::VIEW_ROWS) row_offset++;
            break;
        case 'L':
            if(col > 0) col--;
            else if(col_offset > 0) col_offset--;
            break;
        case 'R':
            if(col < Sheet::VIEW_COLS - 1) col++;
            else if(col_offset < Sheet::TOTAL_COLS - Sheet::VIEW_COLS) col_offset++;
            break;
        default:
            break;
    }
    return 0;
}





bool Cursor::setCell(int row_idx, int col_idx) {
return true;
}