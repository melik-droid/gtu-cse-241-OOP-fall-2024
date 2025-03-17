#ifndef CELL_H
#define CELL_H

#include <string>
#include <iostream>

#include "AnsiTerminal.h"


using std::string;


class Cell{
    public:

    Cell();
    Cell(string label, string value, string formula);

    inline void setValue(const string& value) { this->value = value; }
    inline string getValue() const { return value; }
    
    inline void setType(const string& value_type) { this->value_type = value_type; }
    inline string getType() const { return value_type; }


    
    private:

    string value_type;
    string value;
    string formula;

};







#endif