#include "Board.h"
#include "InputManager.h"
#include <unistd.h>  
#include <iostream>
using namespace std;
int main() {
    cout << "\033[?25l";
    Board board;
    InputManager inputManager;

    while (true) {
        board.render();
        char input = inputManager.getInput();

        switch(input) {
            case 'U': case 'D': case 'L': case 'R':
                board.tryMovePlayer(input);
                break;
            case 'W':
                board.placeWall();
                break;
            case 'M':
                board.placeGoldMine();
                break;
            case 'E':
                board.placeElixirCollector();
                break;
            case 'C':
                board.collectResources();
                break;
            case 'Q':
                cout << "\033[?25h";
                return 0;
        }

        board.update();
        usleep(100000);
    }
    cout << "\033[?25h";
    return 0;
}
