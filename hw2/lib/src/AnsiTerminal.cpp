#include "AnsiTerminal.h"
#include <iostream>
#include <unistd.h>   // For read()
#include <termios.h>  // For terminal control

typedef enum{UP, DOWN, LEFT, RIGHT} Direction;

// Constructor: Configure terminal for non-canonical mode
AnsiTerminal::AnsiTerminal() {
    // Save the original terminal settings
    tcgetattr(STDIN_FILENO, &original_tio);
    struct termios new_tio = original_tio;

    // Disable canonical mode and echo for real-time input reading
    new_tio.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &new_tio);
}

// Destructor: Restore the terminal settings to original state
AnsiTerminal::~AnsiTerminal() {
    tcsetattr(STDIN_FILENO, TCSANOW, &original_tio);
}

// Method to print text at a specified position
void AnsiTerminal::printAt(int row, int col, const std::string &text) {
    std::cout << "\033[" << row << ";" << col << "H\033[32m" << text << "\033[0m" << std::flush;
}

// Method to print text with inverted background at a specified position
void AnsiTerminal::printInvertedAt(int row, int col, const std::string &text) {
    std::cout << "\033[" << row << ";" << col << "H\033[7;32m" << text << "\033[0m" << std::flush; // \033[7m enables reverse video mode, \033[0m resets to normal
    // \033[7m enables reverse video mode, \033[0m resets to normal
}

// Method to clear the terminal screen
void AnsiTerminal::clearScreen() {
    std::cout << "\033[2J\033[H" << std::flush; // Clear screen and move cursor to home
}

// Method to get a single keystroke from the terminal
char AnsiTerminal::getKeystroke() {
    char ch;
    read(STDIN_FILENO, &ch, 1);  // Read the first character

    // If the character falls within ASCII range for control characters
    // Ctrl+A to Ctrl+Z are 0x01 to 0x1A, mapping to 'a' - '@'
    if (ch >= 1 && ch <= 26) {
        // Map Ctrl+A to Ctrl+Z to a more readable form

        if(ch == 10) return '\x0D'; // Enter key

        return ch + 'A' - 1;
    }

    return ch;  // Return the character as-is if it's a regular key
}

// Method to handle arrow key sequences, Alt keys, and other special keys
char AnsiTerminal::getSpecialKey() {
    char ch = getKeystroke();

    if(ch == '\033'){
        char next_ch;  
        if(read(STDIN_FILENO, &next_ch, 1) == 0) return '\033';

        if(next_ch == '['){
            char arrow_key;
            if(read(STDIN_FILENO, &arrow_key, 1) == 1){
                switch (arrow_key)
                {
                case 'A': return '\x18'; break; // UP 
                case 'B': return '\x19'; break; // DOWN
                case 'C': return '\x1A'; break; // Right
                case 'D': return '\x1B'; break; // Left
                }
            }
        } else {
            if(next_ch == 's')  return '\x93'; // Hex code for Alt+S
            if(next_ch == 'q')  return '\x11'; // Hex code for Alt+Q
            return next_ch | 0x80 ;
        }
    }
    return ch;
}
