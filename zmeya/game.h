#pragma once
#include "snake.h"
#include <windows.h>

class Game {
public:
    Game();
    void run();

private:
    void processInput();
    void update();
    void render();
    void generateFood();
    void drawBorders();
    void gameOver();
    void showStartScreen();
    bool isPositionValid(int x, int y) const;

    Snake snake;
    int foodX, foodY;
    bool running;
    bool gameStarted;
    int score;
    HANDLE console;
    COORD prevCursorPos;
    const int WIDTH = 80;
    const int HEIGHT = 24;
    const int BORDER_WIDTH = 1;
};