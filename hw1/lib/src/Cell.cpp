#include <iostream>
#include <string>
#include <vector>

#include "Cell.h"
#include "AnsiTerminal.h"

using std::string;

Cell::Cell(){ // default constructor
    this->value = "";
    this->value_type = "";
    this->formula = "";
}

Cell::Cell(string label, string value, string formula){ // constructor
    this->value = label;
    this->value_type = value;
    this->formula = formula;
}





