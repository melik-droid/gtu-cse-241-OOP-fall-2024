#ifndef FORMULAPARSER_H
#define FORMULAPARSER_H

#include <string>
#include <vector>
#include <memory>
#include <unordered_set>

#include "Cell.h"

class FormulaParser {
public:
    static double evaluateFormula(const std::string& formula, const MyVector<MyVector<std::shared_ptr<Cell>>>& data);

private:
    static double parseExpression(const std::string& expr, const MyVector<MyVector<std::shared_ptr<Cell>>>& data, std::unordered_set<std::string>& visited);
    static double parseTerm(const std::string& expr, long& pos, const MyVector<MyVector<std::shared_ptr<Cell>>>& data, std::unordered_set<std::string>& visited);
    static double parseFactor(const std::string& expr, long& pos, const MyVector<MyVector<std::shared_ptr<Cell>>>& data, std::unordered_set<std::string>& visited);
    static double getCellValue(const std::string& cellRef, const MyVector<MyVector<std::shared_ptr<Cell>>>& data, std::unordered_set<std::string>& visited);
    static int columnNameToIndex(const std::string& col);
    static int getColumnIndex(const std::string& col);
    static std::pair<int, int> parseCellRef(const std::string& ref);
    static MyVector<double> getCellRange(const std::string& range, const MyVector<MyVector<std::shared_ptr<Cell>>>& data);
    static double evaluateFunction(const std::string& func, const std::string& range, const MyVector<MyVector<std::shared_ptr<Cell>>>& data);
};

#endif