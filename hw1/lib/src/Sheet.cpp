#include <iostream>
#include <string>
#include <vector>
#include <iomanip>
#include <fstream>
#include <unistd.h>

#include "AnsiTerminal.h"
#include "FileManager.h"
#include "Sheet.h"
#include "Cell.h"
#include "FormulaParser.h"

using std::string;
using std::vector;
using std::to_string;


Sheet::Sheet(){ // allocate memory for the data vector
    data.resize(TOTAL_ROWS);
    for(int i = 0; i < TOTAL_ROWS; i++){
        data[i].resize(TOTAL_COLS);
    }
}

Sheet::~Sheet(){
    // Destructor
    
}

void Sheet::printSheet() {
    printTable();
    printGrid();
    printCursor();
}

string Sheet::getColumnLabel(int col) const { // to find the column letter
    string name;
    if (col >= 26) {
        name += 'A' + (col / 26 - 1);
        name += 'A' + (col % 26);
    } else {
        name = string(1, 'A' + col);
    }
    return name;
}

void Sheet::printTable(){   
    int startCol = cursor.getColOffset();
    
    for(int row = 0 ; row <= VIEW_ROWS ; row++){
        switch (row){
            case 0:
            case 1: // DONE
                for(int col = 0 ; col < (VIEW_COLS * CELL_WIDTH) + (ROW_LABEL_WIDTH + 1) ; col++ ) 
                    terminal.printInvertedAt(row + 1, col, " ");
                break;
            case 2: //DONE
            case 3: //DONE
                // bilerek boş bırakıldı
                break;
            case 4: //DONE
                for(int col = 0 ; col < (VIEW_COLS * CELL_WIDTH) + (ROW_LABEL_WIDTH + 1) ; col++ ) 
                    terminal.printInvertedAt(row, col, " ");
                
                // Print column letters with offset
                for(int col = ROW_LABEL_WIDTH + (CELL_WIDTH/2) + 1, colIdx = startCol; 
                    col < ( VIEW_COLS + 1 ) * CELL_WIDTH - (ROW_LABEL_WIDTH + 1); 
                    col += CELL_WIDTH, colIdx++){
                    terminal.printInvertedAt(START_ROW, col, getColumnLabel(colIdx));
                }
                break;
            default:
                // Print row numbers with offset
                for(row = 5 ; row <= VIEW_ROWS ; row++){
                    terminal.printInvertedAt(row , 0, "   ");
                    terminal.printInvertedAt(row , 0, to_string(row - 4 + cursor.getRowOffset()));
                }
        }
    }
}

void Sheet::printCursor(){
    string cellValue = data[cursor.getRowIdx()][cursor.getColIdx()].getValue();
    string cellType = data[cursor.getRowIdx()][cursor.getColIdx()].getType();

    if(cellValue.length() < CELL_WIDTH)
        cellValue = cellValue + string(CELL_WIDTH - cellValue.size(), ' ') ;
    else 
        cellValue = cellValue.substr(0, CELL_WIDTH);

    terminal.printInvertedAt(cursor.getRow() + START_ROW + 1, (cursor.getCol() * CELL_WIDTH) + START_COL + 4 , cellValue); 
    string cell_name = getColumnLabel(cursor.getColIdx()) + to_string(cursor.getRowIdx() + 1);
    
    // Show cell name and type in row 1
    terminal.printInvertedAt(1, 1, cell_name + "  (" + cellType + ")");
    
    // Show cell content in row 2
    terminal.printInvertedAt(2, 0, cellValue);
    
    terminal.printAt(3, 0, type_space);
    if (!type_space.empty()) 
        terminal.printInvertedAt(cursor.getRow() + START_ROW + 1, (cursor.getCol() * CELL_WIDTH) + ROW_LABEL_WIDTH + 1, temp);
    else
        terminal.printAt(3, 0, string(CELL_WIDTH * VIEW_COLS + ROW_LABEL_WIDTH, ' '));
}

void Sheet::inputProcess(char key) {
    if (key == '\\') {
        handleFileOperations();
        return;
    }
    
    if ((key <= 'Z' && key >= 'A') || (key <= 'z' && key >= 'A')) mode = "L";
    else if (key <= '9' && key >= '0') mode = "V";


    if (key == 'J'){
        // cursorun olduğu yeri kaydet
        setCell();
    }   
    else if (key == '\b' || key == 127 || key == 8){ //silme işlemrlerini yap
        if(type_space.length() != 0)  type_space.pop_back();
        string clear_line(CELL_WIDTH * VIEW_COLS, ' ');
        terminal.printAt(3, 0, clear_line);
        terminal.printAt(3, 0, type_space); // type_space'i ekrana yazdır
        if(temp.length() != 0) temp.pop_back(); // temp'i de güncelle
    }
    else if (key == 'U' || key == 'D' || key == 'L' || key == 'R'){ // Fixed the condition here
        // hareket etmeden önce geçici veriyi hücreye kaydet 
        if(changeDone) setCell();
        temp = "";
        mode = "";
        type_space = "";
        cursor.moveCursor(key);
    }
    else{
        // ekleme işlemi
        type_space += key;
        temp = (type_space.size() > CELL_WIDTH) ? type_space.substr(0, CELL_WIDTH) : type_space;
        changeDone = true;
    }

}

void Sheet::setCell() {
    string val_type = "V"; // default value type is value
    
    if (!type_space.empty() && type_space[0] == '=') { // check if the first character is '='
        // handle formula
        val_type = "F";
        double result = FormulaParser::evaluateFormula(type_space, data);
        data[cursor.getRowIdx()][cursor.getColIdx()].setType(val_type);
        data[cursor.getRowIdx()][cursor.getColIdx()].setValue(std::to_string(result));
    } else {
        // handle normal input
        for(auto i : type_space) {
            if((i <= 'Z' && i >= 'A') || (i <= 'z' && i >= 'a')) val_type = "L";
            else if(i == '+' || i == '-' || i == '*' || i == '/') val_type = "F";
        }
        data[cursor.getRowIdx()][cursor.getColIdx()].setValue(type_space); // set the value
        data[cursor.getRowIdx()][cursor.getColIdx()].setType(val_type); // set
    }
    
    clearTypeSpace(); // clear the type space
    changeDone = false; // change is done
}

void Sheet::printGrid(){ // print the grid
    for(int row = 0 ; row < VIEW_ROWS ; row++){
        for(int col = 0 ; col < VIEW_COLS ; col++){
            // Get cell value using offsets
            string cellValue = data[row + cursor.getRowOffset()][col + cursor.getColOffset()].getValue();
            
            if(cellValue.length() < CELL_WIDTH) // modify the cell value according to the cell width
                cellValue = cellValue + string(CELL_WIDTH - cellValue.size(), ' ');
            else 
                cellValue = cellValue.substr(0, CELL_WIDTH);

            terminal.printAt(row + START_ROW + 1, (col * CELL_WIDTH) + ROW_LABEL_WIDTH + 1, cellValue);
        }
    }
}

void Sheet::clearTypeSpace(){ // clear the type space
    type_space = "";
    temp = "";
}

void Sheet::handleFileOperations() {  
    terminal.printInvertedAt(2, 0, "File Operation (S)ave/(L)oad: ");
    char op = terminal.getKeystroke();
    
    if (op != 'S' && op != 'L' && op != 's' && op != 'l') { 
        terminal.printInvertedAt(2, 0, string(40, ' '));
        return;
    }
    terminal.printInvertedAt(2, 0, string(40, ' '));

    terminal.printInvertedAt(2, 0, "Enter filename: ");
    string filename;
    char c;
    while ((c = terminal.getKeystroke()) != 'J' ) {
        if (c == 27) {
            terminal.printInvertedAt(2, 0, string(40, ' '));
            return;
        }
        if (c == '\b' || c == 127) { // check whether the character is for deleting
            if (!filename.empty()) {
                filename.pop_back();
                terminal.printInvertedAt(2, 20 + filename.length(), " ");
                terminal.printInvertedAt(2, 20, filename);
            }
            continue;
        }
        filename += c;
        terminal.printInvertedAt(2, 20, filename);
    }

    bool success;
    if (op == 'S'|| op == 's') {
        success = FileManager::saveToFile(filename, data);
        terminal.printInvertedAt(2, 0, success ? "File saved successfully!   " : "Error saving file!         ");
    } else {
        success = FileManager::loadFromFile(filename, data);
        terminal.printInvertedAt(2, 0, success ? "File loaded successfully!   " : "Error loading file!         ");
    }

    usleep(1000000); // wait for user to read it 
    terminal.printAt(2, 0, string(40, ' '));
}