#include <iostream>
#include <string>
#include <iomanip>
#include <fstream>
#include <unistd.h>
#include <memory>

#include "AnsiTerminal.h"
#include "FileManager.h"
#include "Sheet.h"
#include "Cell.h"
#include "FormulaParser.h"
#include "Constants.h"
#include "MyVector.h"


Sheet::Sheet()
    : cursor(), 
      terminal(), 
      type_space(""), 
      temp(""), 
      mode(""), 
      changeDone(false), 
      data(Constants::TOTAL_ROWS, MyVector<std::shared_ptr<Cell>>(Constants::TOTAL_COLS, std::make_shared<ValueCell>())) 
{}


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
    using namespace Constants;
    
    int startCol = cursor.getColOffset();
    
    for(int row = 0 ; row <= VIEW_ROWS ; row++){
        switch (row){
            case 0:
            case 1:
                for(int col = 0 ; col < (VIEW_COLS * CELL_WIDTH) + (ROW_LABEL_WIDTH + 1) ; col++ ) 
                    terminal.printInvertedAt(row + 1, col, " ");
                break;
            case 2: 
            case 3: 
                break;
            case 4: 
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
                    terminal.printInvertedAt(row , 0, std::to_string(row - 4 + cursor.getRowOffset()));
                }
        }
    }
}

void Sheet::printCursor(){ 
    using namespace Constants;

    string cellValue = data[cursor.getRowIdx()][cursor.getColIdx()]->getValue();
    string cellType = data[cursor.getRowIdx()][cursor.getColIdx()]->getType();

    if(cellValue.length() < CELL_WIDTH)
        cellValue = cellValue + string(CELL_WIDTH - cellValue.size(), ' ') ;
    else 
        cellValue = cellValue.substr(0, CELL_WIDTH);

    terminal.printInvertedAt(cursor.getRow() + START_ROW + 1, (cursor.getCol() * CELL_WIDTH) + START_COL + 4 , cellValue); 
    string cell_name = getColumnLabel(cursor.getColIdx()) + std::to_string(cursor.getRowIdx() + 1);
    
    // Show cell name and type in row 1
    terminal.printInvertedAt(1, 1, cell_name + cellType);
    
    // Show cell content in row 2
    terminal.printInvertedAt(2, 0, cellValue);
    
    terminal.printAt(3, 0, type_space);
    if (!type_space.empty()) 
        terminal.printInvertedAt(cursor.getRow() + START_ROW + 1, (cursor.getCol() * CELL_WIDTH) + ROW_LABEL_WIDTH + 1, temp);
    else
        terminal.printAt(3, 0, string(CELL_WIDTH * VIEW_COLS + ROW_LABEL_WIDTH, ' '));
}

void Sheet::inputProcess(char key) {
    if (key == '\x93') {
        handleFileOperations();
        return;
    }
    
    if (key == '\x0D'){   // Enter key
        if (type_space.empty()) {
            data[cursor.getRowIdx()][cursor.getColIdx()] = std::make_shared<ValueCell>();
            clearTypeSpace();
        } else {
            clearTypeSpace();
            setCell();
        }
    }   
    else if (key == '\b' || key == 127 || key == 8){ 
        if(type_space.length() != 0)  type_space.pop_back();
        string clear_line(Constants::CELL_WIDTH * Constants::VIEW_COLS, ' ');
        terminal.printAt(3, 0, clear_line);
        terminal.printAt(3, 0, type_space); 
        if(temp.length() != 0) temp.pop_back(); 
    }
    else if (key == '\x18' || key == '\x19' || key == '\x1B' || key == '\x1A'){  
        if(changeDone) setCell();
        temp = "";
        type_space = "";
        cursor.moveCursor(key);
    }
    else{
        type_space += key;
        temp = (type_space.size() > Constants::CELL_WIDTH) ? type_space.substr(0, Constants::CELL_WIDTH) : type_space;
        changeDone = true;
    }

}

void Sheet::setCell() {
    if (type_space.empty()) {
        return;
    }

    if (type_space[0] == '=') {
        std::unique_ptr<FormulaCell> temp_formula_cell = std::make_unique<FormulaCell>(); 
        double result = FormulaParser::evaluateFormula(type_space, data);
        temp_formula_cell->setFormula(type_space);
        temp_formula_cell->setValue(std::to_string(result));
        data[cursor.getRowIdx()][cursor.getColIdx()] = std::move(temp_formula_cell);
    } else {
        std::unique_ptr<ValueCell> temp_value_cell = std::make_unique<StringValueCell>();
        
        bool hasNonSpace = false;
        bool isNumber = true;
        bool hasDecimal = false;
        
        for (char c : type_space) {
            if (!std::isspace(c)) {
                hasNonSpace = true;
                if (!std::isdigit(c) && c != '.') {
                    isNumber = false;
                    break;
                }
                if (c == '.') {
                    hasDecimal = true;
                }
            }
        }

        if (hasNonSpace && isNumber) {
            if (hasDecimal) {
                temp_value_cell = std::make_unique<DoubleValueCell>();
            } else {
                temp_value_cell = std::make_unique<IntValueCell>();
            }
        }

        try {
            temp_value_cell->setValue(type_space);
            data[cursor.getRowIdx()][cursor.getColIdx()] = std::move(temp_value_cell);
        } catch (const std::exception& e) {
            // If number conversion fails, fallback to StringValueCell
            temp_value_cell = std::make_unique<StringValueCell>();
            temp_value_cell->setValue(type_space);
            data[cursor.getRowIdx()][cursor.getColIdx()] = std::move(temp_value_cell);
        }
    }
    
    clearTypeSpace();
    changeDone = false;
}

void Sheet::updateFormulas() {
    for(size_t row = 0; row < Constants::TOTAL_ROWS; ++row) {
        for(size_t col = 0; col < Constants::TOTAL_COLS; ++col) {
            auto& cell = data[row][col];
            if(cell && cell->getType() == "(F)") {
                FormulaCell* fCell = dynamic_cast<FormulaCell*>(cell.get());
                if(fCell) {
                    try {
                        double result = FormulaParser::evaluateFormula(fCell->getFormula(), data);
                        fCell->setValue(std::to_string(result));
                    } catch(const std::exception&) {
                        // If evaluation fails, leave the original formula visible
                        fCell->setValue(fCell->getFormula());
                    }
                }
            }
        }
    }
}

void Sheet::printGrid() {
    updateFormulas(); 
    using namespace Constants;
    for(int row = 0; row < VIEW_ROWS; row++) {
        for(int col = 0; col < VIEW_COLS; col++) {
            string cellValue;
            auto& cell = data[row + cursor.getRowOffset()][col + cursor.getColOffset()];
            
            if(cell)
                cellValue = cell->getValue();
            
            // Format cell value
            if(cellValue.length() < CELL_WIDTH) {
                cellValue = cellValue + string(CELL_WIDTH - cellValue.size(), ' ');
            } else {
                cellValue = cellValue.substr(0, CELL_WIDTH);
            }

            terminal.printAt(row + START_ROW + 1, 
                           (col * CELL_WIDTH) + ROW_LABEL_WIDTH + 1, 
                           cellValue);
        }
    }
}

void Sheet::clearTypeSpace(){
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
    while ((c = terminal.getKeystroke()) != '\x0D' ) {
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

    filename += ".csv";
    bool success;
    if (op == 'S'|| op == 's') {
        success = FileManager::saveToFile(filename, data);
        terminal.printInvertedAt(2, 0, success ? "File saved successfully!   " : "Error saving file!         ");
    } else {
        success = FileManager::loadFromFile(filename, data);
        terminal.printInvertedAt(2, 0, success ? "File loaded successfully!   " + string(20,' '): "Error loading file!         ");
    }
    
    usleep(1000000); // wait for user to read it 
    terminal.printAt(2, 0, string(40, ' '));
}