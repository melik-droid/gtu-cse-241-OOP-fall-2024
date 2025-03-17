#ifndef CELL_H
#define CELL_H

#include <string>
#include <iostream>
#include <stdexcept>
#include <memory>

#include "AnsiTerminal.h"


using std::string;


class Cell {
protected:
    string value;
public:
    Cell() : value("") {};
    virtual ~Cell() = default; 
    virtual void setValue(const string& value) = 0;
    virtual std::string getValue() const = 0;
    virtual void clear() = 0;
    virtual string getType() const = 0;
};

class ValueCell : public Cell {
public:
    ValueCell() : Cell() {}
    virtual ~ValueCell() = default;
    virtual void setValue(const string& newValue) override { value = newValue; }
    virtual string getValue() const override { return value; }
    virtual string getType() const override { return ""; }
    void clear() override{ value.clear(); }
};

class FormulaCell : public Cell {
protected:
    std::string formula;
public:
    FormulaCell(const std::string& expr = "") : Cell(), formula(expr) {}
    void setFormula(const std::string& expr) { formula = expr; }
    std::string getFormula() const { return formula; }
    void setValue(const std::string& newValue) override { value = newValue; }
    string getValue() const override { return value.empty() ? formula : value; }
    std::string getType() const override final { return "(F)"; }
    void clear() override {
        formula.clear();
        value.clear();
    }
};

class IntValueCell : public ValueCell {
private: 
    int intValue;
public:
    IntValueCell() : ValueCell(), intValue(0) {}

    string getValue() const override { return std::to_string(intValue); }
    string getType() const override { return "(V)"; }

    void setValue(const string& newValue) override {
        try {
            intValue = std::stoi(newValue);
            value = newValue; 
        } catch ( const std::exception& e){
            throw std::invalid_argument("Invalid integer value");
        }
    }


    void clear() override {
        intValue = 0;
        value.clear();
    }

};

class DoubleValueCell : public ValueCell {
private:
    double doubleValue;
public:
    DoubleValueCell() : ValueCell(), doubleValue(0.0) {}

    string getValue() const override { return std::to_string(doubleValue); }
    string getType() const override { return "(V)"; }
    
    void setValue(const string& newValue) override {
        try {
            doubleValue = std::stod(newValue);
            value = newValue;
        } catch (const std::exception& e){
            throw std::invalid_argument("Invalid double value");
        }
    }

    void clear() override {
        doubleValue = 0.0;
        value.clear();
    }

};


class StringValueCell : public ValueCell {
public:
    StringValueCell() : ValueCell() {}

    string getValue() const override { return value; }
    string getType() const override { return "(L)"; }

    void setValue(const string& newValue) override { value = newValue; }

    void clear() override {
        value.clear();
    }
};

#endif