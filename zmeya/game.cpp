#include "game.h"
#include <conio.h>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <algorithm>


const int KEY_ESC = 27;
const int KEY_W = 119;
const int KEY_A = 97;
const int KEY_S = 115;
const int KEY_D = 100;
const int KEY_UP = 72;
const int KEY_LEFT = 75;
const int KEY_RIGHT = 77;
const int KEY_DOWN = 80;

const int WIDTH = 80;
const int HEIGHT = 24;
const int BORDER_WIDTH = 1;
const int START_X = 40;  
const int START_Y = 12;

Game::Game() :
    snake(START_X, START_Y),
    running(true),
    score(0),
    gameStarted(false)
{
    console = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cursorInfo = { 1, FALSE };
    SetConsoleCursorInfo(console, &cursorInfo);
    srand(static_cast<unsigned>(time(nullptr)));

    generateFood();
    prevCursorPos = { static_cast<SHORT>(START_X), static_cast<SHORT>(START_Y) };

    DWORD mode;
    GetConsoleMode(console, &mode);
    SetConsoleMode(console, mode & ~ENABLE_QUICK_EDIT_MODE);

    system("cls");
    drawBorders();

    // Отрисовка змейки с явным приведением типов
    SetConsoleTextAttribute(console, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
    COORD centerPos = { static_cast<SHORT>(START_X), static_cast<SHORT>(START_Y) };
    SetConsoleCursorPosition(console, centerPos);
    std::cout << 'O';

    // Отрисовка еды с явным приведением типов
    COORD foodPos = { static_cast<SHORT>(foodX), static_cast<SHORT>(foodY) };
    SetConsoleCursorPosition(console, foodPos);
    SetConsoleTextAttribute(console, FOREGROUND_RED | FOREGROUND_INTENSITY);
    std::cout << '@';

    showStartScreen();
}

void Game::run() {
    while (running) {
        processInput();
        if (gameStarted) {
            update();
            render();
        }
        Sleep(100);  // Задержка для плавности
    }
    gameOver();
}

void Game::processInput() {
    if (_kbhit()) {
        int key = _getch();

        // Обработка стрелок
        if (key == 0xE0 || key == 0) {
            key = _getch();
        }

        if (!gameStarted) {
            // Старт игры при любом допустимом нажатии
            if (key == KEY_W || key == KEY_A || key == KEY_S || key == KEY_D ||
                key == KEY_UP || key == KEY_LEFT || key == KEY_RIGHT || key == KEY_DOWN) {
                gameStarted = true;
                system("cls");
                drawBorders();
            }
            return;
        }

        // Обработка управления
        switch (key) {
        case KEY_W:
        case KEY_UP:
            if (snake.getDirection() != DOWN) snake.changeDirection(UP);
            break;
        case KEY_S:
        case KEY_DOWN:
            if (snake.getDirection() != UP) snake.changeDirection(DOWN);
            break;
        case KEY_A:
        case KEY_LEFT:
            if (snake.getDirection() != RIGHT) snake.changeDirection(LEFT);
            break;
        case KEY_D:
        case KEY_RIGHT:
            if (snake.getDirection() != LEFT) snake.changeDirection(RIGHT);
            break;
        case KEY_ESC:
            running = false;
            break;
        }
    }
}

void Game::update() {
    if (!gameStarted) return;

    snake.move();

    const auto& head = snake.getBody().front();

    // Проверка столкновения с едой
    if (head.first == foodX && head.second == foodY) {
        snake.grow(); // Установим флаг роста
        score += 10;
        generateFood();
    }

    // Проверка столкновений с границами и собой
    if (head.first <= BORDER_WIDTH ||
        head.first >= WIDTH - BORDER_WIDTH - 1 ||
        head.second <= BORDER_WIDTH ||
        head.second >= HEIGHT - BORDER_WIDTH - 1 ||
        snake.checkSelfCollision())
    {
        running = false;
    }
}

void Game::render() {
    // Очистка предыдущей позиции головы
    SetConsoleCursorPosition(console, prevCursorPos);
    std::cout << ' ';

    // Отрисовка змейки
    SetConsoleTextAttribute(console, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
    const auto& body = snake.getBody();
    for (const auto& segment : body) {
        COORD pos = { static_cast<SHORT>(segment.first), static_cast<SHORT>(segment.second) };
        SetConsoleCursorPosition(console, pos);
        std::cout << 'O';
    }

    // Очистка хвоста только если змейка не растет
    if (!snake.isGrowing() && body.size() > 1) {
        COORD tailPos = { static_cast<SHORT>(body.back().first),
                        static_cast<SHORT>(body.back().second) };
        SetConsoleCursorPosition(console, tailPos);
        std::cout << ' ';
    }

    // Отрисовка еды
    SetConsoleTextAttribute(console, FOREGROUND_RED | FOREGROUND_INTENSITY);
    COORD foodPos = { static_cast<SHORT>(foodX), static_cast<SHORT>(foodY) };
    SetConsoleCursorPosition(console, foodPos);
    std::cout << '@';

    // Сохранение текущей позиции головы
    prevCursorPos = { static_cast<SHORT>(body.front().first),
                    static_cast<SHORT>(body.front().second) };

    // Отрисовка счета
    COORD scorePos = { 0, HEIGHT };
    SetConsoleCursorPosition(console, scorePos);
    SetConsoleTextAttribute(console, FOREGROUND_BLUE | FOREGROUND_INTENSITY);
    std::cout << "Score: " << score << "    ";
}

void Game::generateFood() {
    do {
        foodX = rand() % (WIDTH - 2 * BORDER_WIDTH) + BORDER_WIDTH;
        foodY = rand() % (HEIGHT - 2 * BORDER_WIDTH) + BORDER_WIDTH;
    } while (std::any_of(snake.getBody().begin(), snake.getBody().end(),
        [this](const auto& segment) {
            return segment.first == foodX && segment.second == foodY;
        }));
}

void Game::drawBorders() {
    SetConsoleTextAttribute(console, FOREGROUND_BLUE | FOREGROUND_INTENSITY);
    // Горизонтальные границы
    for (int x = 0; x < WIDTH; x++) {
        // Верхняя граница
        COORD pos = { static_cast<SHORT>(x), 0 };
        SetConsoleCursorPosition(console, pos);
        std::cout << '#';

        // Нижняя граница
        pos.Y = HEIGHT - 1;
        SetConsoleCursorPosition(console, pos);
        std::cout << '#';
    }

    // Вертикальные границы (исключая углы, которые уже нарисованы)
    for (int y = 1; y < HEIGHT - 1; y++) {
        // Левая граница
        COORD pos = { 0, static_cast<SHORT>(y) };
        SetConsoleCursorPosition(console, pos);
        std::cout << '#';

        // Правая граница
        pos.X = WIDTH - 1;
        SetConsoleCursorPosition(console, pos);
        std::cout << '#';
    }
}

void Game::gameOver() {
    system("cls");
    COORD pos = { WIDTH / 2 - 5, HEIGHT / 2 };
    SetConsoleCursorPosition(console, pos);
    SetConsoleTextAttribute(console, FOREGROUND_RED | FOREGROUND_INTENSITY);
    std::cout << "GAME OVER!";

    pos.Y += 2;
    SetConsoleCursorPosition(console, pos);
    SetConsoleTextAttribute(console, FOREGROUND_BLUE | FOREGROUND_INTENSITY);
    std::cout << "Final Score: " << score;

    pos.Y += 2;
    SetConsoleCursorPosition(console, pos);
    std::cout << "Press any key to exit...";
    _getch();
}

void Game::showStartScreen() {
    COORD pos = { WIDTH / 2 - 15, HEIGHT / 2 + 3 };
    SetConsoleCursorPosition(console, pos);
    SetConsoleTextAttribute(console, FOREGROUND_RED | FOREGROUND_INTENSITY);
    std::cout << "PRESS WASD OR ARROWS TO START";

    pos.Y += 2;
    SetConsoleCursorPosition(console, pos);
    SetConsoleTextAttribute(console, FOREGROUND_BLUE | FOREGROUND_INTENSITY);
    std::cout << "CONTROLS: W/↑ A/← S/↓ D/→";

    pos.Y += 1;
    SetConsoleCursorPosition(console, pos);
    std::cout << "ESC - EXIT";
}

bool Game::isPositionValid(int x, int y) const {
    return x >= BORDER_WIDTH &&
        x < WIDTH - BORDER_WIDTH &&
        y >= BORDER_WIDTH &&
        y < HEIGHT - BORDER_WIDTH;
}