#include <iostream>
#include <string>
#include <iomanip>

#include "AnsiTerminal.h"
#include "Sheet.h"
#include "Cell.h"
#include "Cursor.h"
#include "Constants.h"

using std::string;
using std::cout;
using std::endl;


Cursor::Cursor() : row(0), col(0), row_offset(0), col_offset(0) // Constructor
{} 



   

int Cursor::moveCursor(char key) {  // move the cursor
    using namespace Constants;
    switch(key) {
        case '\x18':
            if(row > 0) row--;
            else if(row_offset > 0 ) row_offset--;
            break;
        case '\x19':
            if(row < VIEW_ROWS - START_ROW - 1) row++;
            else if(row_offset < TOTAL_ROWS - VIEW_ROWS) row_offset++;
            break;
        case '\x1B':
            if(col > 0) col--;
            else if(col_offset > 0) col_offset--;
            break;
        case '\x1A':
            if(col < VIEW_COLS - 1) col++;
            else if(col_offset < TOTAL_COLS - VIEW_COLS) col_offset++;
            break;
        default:
            break;
    }
    return 0;
}
