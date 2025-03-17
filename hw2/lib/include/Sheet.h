#ifndef SHEET_H
#define SHEET_H

#include <string>
#include <iostream>
#include <memory>

#include "Cell.h"
#include "AnsiTerminal.h"
#include "Cursor.h"
#include "MyVector.h"
#include "FormulaParser.h"

using std::string;
using std::shared_ptr;

class Sheet{
    public:

    Sheet();
    ~Sheet() {};
    void printSheet();
    void printTable();
    void printGrid();
    void printCursor();
    
    void inputProcess(char key);
    void setCell(); 

    void handleFileOperations();

    inline string getMode() const { return mode; }
    inline void setMode(string mode) { this->mode = mode; }

    AnsiTerminal& getTerminal() { return terminal; }
    MyVector<MyVector<shared_ptr<Cell>>>& getData() { return data; }

    void updateFormulas();

    private:
    
    MyVector<MyVector<shared_ptr<Cell>>> data;  
    AnsiTerminal terminal;
    Cursor cursor;

    string type_space;
    string temp;
    string mode;

    bool changeDone = false;

    void clearTypeSpace();
    string getColumnLabel(int col) const;


    

};

#endif