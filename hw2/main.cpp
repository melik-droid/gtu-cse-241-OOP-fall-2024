#include <string>
#include <iostream>

#include "AnsiTerminal.h"
#include "Sheet.h"
#include "Cursor.h"
#include "Cell.h"
    

int main() {
    Sheet sheet;
    int flag = 1; 
    sheet.getTerminal().clearScreen();
    char key;

    while (flag) { 
        sheet.printSheet();
        key = sheet.getTerminal().getSpecialKey();

        if (key == '\x11') flag = 0;
        else sheet.inputProcess(key);
    }
    
    sheet.getTerminal().clearScreen(); 
    return 0;
}
