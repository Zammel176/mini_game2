#ifndef INPUTMANAGER_H
#define INPUTMANAGER_H

#include <termios.h>

class InputManager {
private:
    termios originalTerminalSettings;
public:
    InputManager();
    ~InputManager();
    char getInput() const;
};

#endif
