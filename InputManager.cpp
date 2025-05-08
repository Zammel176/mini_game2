#include "InputManager.h"
#include <cstdio>  
#include <cctype>
#include <unistd.h>
#include <termios.h>
InputManager::InputManager() {
    tcgetattr(STDIN_FILENO, &originalTerminalSettings);
    termios newSettings = originalTerminalSettings;
    newSettings.c_lflag &= ~(ICANON | ECHO);
    newSettings.c_cc[VMIN] = 1;
    newSettings.c_cc[VTIME] = 0;
    tcsetattr(STDIN_FILENO, TCSANOW, &newSettings);
}

InputManager::~InputManager() {
    tcsetattr(STDIN_FILENO, TCSANOW, &originalTerminalSettings);
}

char InputManager::getInput() const {
    char ch = getchar();
    if (ch == 27) {
        getchar();
        ch = getchar();
        switch(ch) {
            case 'A': return 'U';
            case 'B': return 'D';
            case 'C': return 'R';
            case 'D': return 'L';
        }
    }
    return toupper(ch);
}
