#include "FileManager.h"
#include <fstream>
#include <string>


using std::string;
using std::vector;

bool FileManager::saveToFile(const string& filename, const vector<vector<Cell>>& data) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }

    file << "row,column,type,value\n";  // print the header

    for (int row = 0; row < data.size(); row++) {
        for (int col = 0; col < data[row].size(); col++) {
            const Cell& cell = data[row][col];
            if (!cell.getValue().empty()) {
                string value = cell.getValue();
                string escaped_value;
                bool needs_quotes = false;

                for (char c : value) {
                    if (c == ',' || c == '"' || c == '\n') {
                        needs_quotes = true;
                    }
                    if (c == '"') {
                        escaped_value += "\"\"";
                    } else {
                        escaped_value += c;
                    }
                }

                if (needs_quotes) {
                    value = "\"" + escaped_value + "\"";
                } else {
                    value = escaped_value;
                }

                file << row << "," << col << ","
                     << cell.getType() << ","
                     << value << "\n";
            }
        }
    }

    file.close();
    return true;
}

bool FileManager::loadFromFile(const string& filename, vector<vector<Cell>>& data) { // Taken form AI 
    std::ifstream file(filename);
    if (!file.is_open()) {
        return false;
    }

    // Clear existing data
    for (auto& row : data) {
        for (auto& cell : row) {
            cell = Cell();
        }
    }

    // Skip header
    string header;
    std::getline(file, header);

    string line;
    while (std::getline(file, line)) {
        int start = 0;
        int pos = 0;
        vector<string> fields;
        bool in_quotes = false;
        string current_field;

        for (int i = 0; i < line.length(); i++) {
            if (line[i] == '"') {
                in_quotes = !in_quotes;
            } else if (line[i] == ',' && !in_quotes) {
                fields.push_back(current_field);
                current_field.clear();
            } else {
                current_field += line[i];
            }
        }
        fields.push_back(current_field);

        if (fields.size() != 4) continue;

        try {
            int row = std::stoi(fields[0]);
            int col = std::stoi(fields[1]);
            string type = fields[2];
            string value = fields[3];

            if (value.length() >= 2 && value.front() == '"' && value.back() == '"') {
                value = value.substr(1, value.length() - 2);
            }

            int pos = 0;
            while ((pos = value.find("\"\"", pos)) != string::npos) {
                value.replace(pos, 2, "\"");
                pos += 1;
            }

            if (row >= 0 && row < static_cast<int>(data.size()) && 
                col >= 0 && col < static_cast<int>(data[0].size())) {
                data[row][col].setValue(value);
                data[row][col].setType(type);
            }
        } catch (const std::exception&) {
            continue;
        }
    }

    file.close();
    return true;
}
