#include <cctype>
#include <stdexcept>
#include <cmath>
#include <sstream>
#include <limits>
#include <algorithm>
#include <numeric>

#include "MyVector.h"
#include "FormulaParser.h"


double FormulaParser::evaluateFormula(const std::string& formula, const MyVector<MyVector<std::shared_ptr<Cell>>>& data) {
    if (formula.empty() || (formula[0] != '=')) {
        throw std::invalid_argument("Formula must start with '='");
    }

    // Check for functions first
    std::istringstream iss(formula.substr(1));
    std::string token;
    double result = 0.0;
    char operation = '+';
    
    while(iss >> token) {
        double value = 0.0;
        
        if(token == "+" || token == "-" || token == "*" || token == "/") {
            operation = token[0];
            continue;
        }
        
        long openParen = token.find('(');
        if(openParen != std::string::npos) {
            std::string func = token.substr(0, openParen);
            std::string range = token.substr(openParen + 1);
            range = range.substr(0, range.length() - 1); // Remove closing parenthesis
            value = evaluateFunction(func, range, data);
        }
        // If not a function, treat as regular expression
        else {
            std::unordered_set<std::string> visited;
            value = parseExpression(token, data, visited);
        }
        
        switch(operation) {
            case '+': result += value; break;
            case '-': result -= value; break;
            case '*': result *= value; break;
            case '/': result = value != 0 ? result / value : 0; break;
        }
    }
    
    return result;
}

double FormulaParser::parseExpression(const std::string& expr, const MyVector<MyVector<std::shared_ptr<Cell>>>& data, std::unordered_set<std::string>& visited) {
    long pos = 0;
    double result = parseTerm(expr, pos, data, visited);
    while (pos < expr.length()) {
        if (expr[pos] == '+') {
            pos++;
            result += parseTerm(expr, pos, data, visited);
        }
        else if (expr[pos] == '-') {
            pos++;
            result -= parseTerm(expr, pos, data, visited);
        }
        else {
            break;
        }
    }
    return result;
}


double FormulaParser::parseTerm(const std::string& expr, long& pos, const MyVector<MyVector<std::shared_ptr<Cell>>>& data, std::unordered_set<std::string>& visited) {
    double result = parseFactor(expr, pos, data, visited);
    while (pos < expr.length()) {
        if (expr[pos] == '*') {
            pos++;
            result *= parseFactor(expr, pos, data, visited);
        }
        else if (expr[pos] == '/') {
            pos++;
            double divisor = parseFactor(expr, pos, data, visited);
            if (divisor == 0) {
                throw std::runtime_error("Division by zero");
            }
            result /= divisor;
        }
        else {
            break;
        }
    }
    return result;
}

double FormulaParser::parseFactor(const std::string& expr, long& pos, const MyVector<MyVector<std::shared_ptr<Cell>>>& data, std::unordered_set<std::string>& visited) {
    while (pos < expr.length() && isspace(expr[pos])) pos++;
    if (pos >= expr.length()) {
        throw std::runtime_error("Unexpected end of expression");
    }

    if (expr[pos] == '(') {
        pos++;
        double result = parseExpression(expr, data, visited);
        if (pos >= expr.length() || expr[pos] != ')') {
            throw std::runtime_error("Mismatched parentheses");
        }
        pos++;
        return result;
    }
    else if (isalpha(expr[pos])) {
        std::string cellRef;
        while (pos < expr.length() && isalpha(expr[pos])) {
            cellRef += toupper(expr[pos]);
            pos++;
        }
        std::string rowStr;
        while (pos < expr.length() && isdigit(expr[pos])) {
            rowStr += expr[pos];
            pos++;
        }
        if (rowStr.empty()) {
            throw std::runtime_error("Invalid cell reference");
        }
        cellRef += rowStr;
        return getCellValue(cellRef, data, visited);
    }
    else {
        std::string number;
        bool hasDot = false;
        while (pos < expr.length() && (isdigit(expr[pos]) || expr[pos] == '.')) {
            if (expr[pos] == '.') {
                if (hasDot) {
                    throw std::runtime_error("Invalid number format");
                }
                hasDot = true;
            }
            number += expr[pos++];
        }
        if (number.empty()) {
            throw std::runtime_error("Invalid factor");
        }
        return std::stod(number);
    }
}

double FormulaParser::getCellValue(const std::string& cellRef, const MyVector<MyVector<std::shared_ptr<Cell>>>& data, std::unordered_set<std::string>& visited) {
    if (visited.find(cellRef) != visited.end()) {
        throw std::runtime_error("Circular reference detected");
    }
    visited.insert(cellRef);

    long len = 0;
    while (len < cellRef.length() && isalpha(cellRef[len])) len++;
    std::string colStr = cellRef.substr(0, len);
    std::string rowStr = cellRef.substr(len);
    int col = columnNameToIndex(colStr);
    int row = std::stoi(rowStr) - 1;

    if (row < 0 || row >= data.size() || col < 0 || col >= data[0].size()) {
        throw std::out_of_range("Cell reference out of range");
    }

    Cell* cell = data[row][col].get();
    if (!cell) {
        throw std::runtime_error("Referenced cell is empty");
    }

    if (cell->getType() == "(F)") {
        FormulaCell* fCell = dynamic_cast<FormulaCell*>(cell);
        if (!fCell) {
            throw std::runtime_error("Invalid FormulaCell");
        }
        double value = evaluateFormula(fCell->getFormula(), data);
        visited.erase(cellRef);
        return value;
    }
    else {
        ValueCell* vCell = dynamic_cast<ValueCell*>(cell);
        if (!vCell) {
            throw std::runtime_error("Invalid ValueCell");
        }
        try {
            return std::stod(vCell->getValue());
        }
        catch (...) {
            throw std::runtime_error("Non-numeric cell value");
        }
    }
}

int FormulaParser::columnNameToIndex(const std::string& col) {
    int index = 0;
    for (char c : col) {
        if (!isalpha(c)) {
            throw std::invalid_argument("Invalid column name");
        }
        index = index * 26 + (toupper(c) - 'A' + 1);
    }
    return index - 1;
}

int FormulaParser::getColumnIndex(const std::string& col) {
    int result = 0;
    for(char c : col) {
        result = result * 26 + (c - 'A' + 1);
    }
    return result - 1;
}

std::pair<int, int> FormulaParser::parseCellRef(const std::string& ref) {
    std::string col;
    std::string row;
    
    for(char c : ref) {
        if(isalpha(c)) col += c;
        else row += c;
    }
    
    return {std::stoi(row) - 1, getColumnIndex(col)};
}

// Helper function to get range of cells
MyVector<double> FormulaParser::getCellRange(const std::string& range, const MyVector<MyVector<std::shared_ptr<Cell>>>& data) {
    MyVector<double> values;
    long dotPos = range.find("..");
    if(dotPos == std::string::npos) return values;
    
    std::string start = range.substr(0, dotPos);
    std::string end = range.substr(dotPos + 2);
    
    auto [startRow, startCol] = parseCellRef(start);
    auto [endRow, endCol] = parseCellRef(end);
    
    for(int row = startRow; row <= endRow; ++row) {
        for(int col = startCol; col <= endCol; ++col) {
            if(row < static_cast<int>(data.size()) && col < static_cast<int>(data[0].size())) {
                try {
                    values.push_back(std::stod(data[row][col]->getValue()));
                } catch(...) {
                    // Skip non-numeric values
                }
            }
        }
    }
    
    return values;
}

double FormulaParser::evaluateFunction(const std::string& func, const std::string& range, const MyVector<MyVector<std::shared_ptr<Cell>>>& data) {
    auto values = getCellRange(range, data);
    if(values.empty()) return 0.0;
    
    if(func == "SUM") {
        return std::accumulate(values.begin(), values.end(), 0.0);
    }
    else if(func == "AVER") {
        return std::accumulate(values.begin(), values.end(), 0.0) / values.size();
    }
    else if(func == "MAX") {
        return *std::max_element(values.begin(), values.end());
    }
    else if(func == "MIN") {
        return *std::min_element(values.begin(), values.end());
    }
    else if(func == "STDDEV") {
        double mean = std::accumulate(values.begin(), values.end(), 0.0) / values.size();
        double sq_sum = std::inner_product(values.begin(), values.end(), values.begin(), 0.0);
        return std::sqrt(sq_sum / values.size() - mean * mean);
    }
    return 0.0;
}