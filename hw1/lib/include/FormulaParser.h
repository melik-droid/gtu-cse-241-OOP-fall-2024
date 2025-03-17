#ifndef FORMULA_PARSER_H
#define FORMULA_PARSER_H

#include <string>
#include <vector>
#include "Cell.h"

using std::string;
using std::vector;

class FormulaParser {
public:
    static double evaluateFormula(const std::string& formula, const std::vector<std::vector<Cell>>& data);

private:
    static int getColumnIndex(const std::string& col);
    static bool getCellCoordinates(const std::string& cellRef, int& row, int& col);
    static double getCellValue(const std::string& cellRef, const std::vector<std::vector<Cell>>& data);
    static void trimString(std::string& str);
    static bool isValidNumber(const std::string& str, double& result);
    static bool isValidInteger(const std::string& str, int& result);

    static std::vector<std::string> tokenizeFormula(const std::string& formula);
    static bool isOperator(char c);

    const static int INT_MAX = 2147483647;
    const static int INT_MIN = -2147483648;
};

#endif
