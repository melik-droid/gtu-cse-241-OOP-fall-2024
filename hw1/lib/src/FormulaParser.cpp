#include "FormulaParser.h"
#include <sstream>
#include <algorithm>


using std::string;
using std::vector;

bool FormulaParser::isOperator(char c) { 
    return c == '+' || c == '-' || c == '*' || c == '/';
}

vector<string> FormulaParser::tokenizeFormula(const string& formula) { // assisted from AI at this place 
    vector<string> tokens;
    string currentToken;
    
    for (int i = 0; i < formula.length(); ++i) { 
        if (isOperator(formula[i])) {
            if (!currentToken.empty()) {
                tokens.push_back(currentToken);
                currentToken.clear();
            }
            tokens.push_back(string(1, formula[i]));
        } else {
            currentToken += formula[i];
        }
    }
    
    if (!currentToken.empty()) { 
        tokens.push_back(currentToken);
    }
    
    return tokens;
}

double FormulaParser::evaluateFormula(const string& formula, const vector<vector<Cell>>& data) {
    if (formula.empty() || formula[0] != '=') return 0.0;
    
    string workingFormula = formula.substr(1); 
    trimString(workingFormula);
    
    vector<string> tokens = tokenizeFormula(workingFormula); 
    double result = 0.0; //ending value
    char operation = '+'; // default operator 
    
    for (const string& token : tokens) {
        if (token.length() == 1 && isOperator(token[0])) {
            operation = token[0];
            continue;
        }
        
        double value = getCellValue(token, data);
        
        switch (operation) {
            case '+': result += value; break;
            case '-': result -= value; break;
            case '*': result *= value; break;
            case '/': 
                if (value != 0) result /= value;
                else return 0.0;
                break;
        }
    }
    return result;
}

bool FormulaParser::isValidNumber(const string& str, double& result) { // check is the number is value, partially taken from AI and modified
    if (str.empty()) return false;
    
    char* endPtr;
    result = strtod(str.c_str(), &endPtr);
    
    return (*endPtr == '\0');
}

bool FormulaParser::isValidInteger(const string& str, int& result) { // check whether the integer is valid 
    if (str.empty()) return false;
    
    char* endPtr;
    long value = strtol(str.c_str(), &endPtr, 10);
    
    if (*endPtr != '\0' || value > INT_MAX || value < INT_MIN) {
        return false;
    }
    
    result = static_cast<int>(value);
    return true;
}

int FormulaParser::getColumnIndex(const string& col) {  // to find column index
    int result = 0;
    for (char c : col) {
        if (isalpha(c)) {
            result = result * 26 + (toupper(c) - 'A');
        }
    }
    return result;
}

bool FormulaParser::getCellCoordinates(const string& cellRef, int& row, int& col) { // to find coordinates of cell
    string colStr;
    string rowStr;
    
    for (char c : cellRef) {
        if (isalpha(c)) colStr += c;
        else if (isdigit(c)) rowStr += c;
    }
    
    if (colStr.empty() || rowStr.empty()) return false;
    
    int rowNum;
    if (!isValidInteger(rowStr, rowNum)) return false;
    
    row = rowNum - 1;
    col = getColumnIndex(colStr);
    return true;
}

double FormulaParser::getCellValue(const string& cellRef, const vector<vector<Cell>>& data) { // to get value of cell, taken from AI
    int row, col;
    if (!getCellCoordinates(cellRef, row, col)) return 0.0;
    
    if (row >= 0 && row < data.size() && col >= 0 && col < data[0].size()) {
        const Cell& cell = data[row][col];
        const string& value = cell.getValue();
        
        if (value.empty()) return 0.0; 
        
        string trimmed = value;
        trimString(trimmed);
        
        double number;
        if (isValidNumber(trimmed, number)) {
            return number;
        }
    }
    return 0.0;
}

void FormulaParser::trimString(string& str) { // to trim the string
    str.erase(0, str.find_first_not_of(" \t\n\r\f\v"));
    str.erase(str.find_last_not_of(" \t\n\r\f\v") + 1);
}
