#include <string>
#include <vector>
#include <iostream>

#include "AnsiTerminal.h"
#include "Sheet.h"
#include "Cursor.h"
#include "Cell.h"
    

int main() {
    Sheet sheet;
    int flag = 1; // initialize the flag with true
    sheet.getTerminal().clearScreen(); // Clear the screen at the beginning
    char key;

    while (flag) { // main loop
        sheet.printSheet();
        key = sheet.getTerminal().getSpecialKey();

        if (key == 'q') flag = 0;
        else sheet.inputProcess(key);
    }
    
    sheet.getTerminal().clearScreen(); // Clear the screen on exit
    return 0;
}
