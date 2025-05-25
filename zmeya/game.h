#pragma once
#include "snake.h"
#include <windows.h>
#include <vector>
// Константы управления
const int KEY_ESC = 27;
const int KEY_W = 119;
const int KEY_A = 97;
const int KEY_S = 115;
const int KEY_D = 100;
const int KEY_UP = 72;
const int KEY_LEFT = 75;
const int KEY_RIGHT = 77;
const int KEY_DOWN = 80;

// Константы игры
const int WIDTH = 80;
const int HEIGHT = 24;
const int BORDER_WIDTH = 1;
const int START_X = 40;
const int START_Y = 12;
const int FOOD_WIDTH = 2;
const int FOOD_HEIGHT = 2;
const int GAME_SPEED = 150;

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

    std::vector<std::pair<int, int>> prevBody;

    Snake snake;
    int foodX, foodY;
    bool running;
    bool gameStarted;
    int score;
    HANDLE console;
    COORD prevCursorPos;
};
