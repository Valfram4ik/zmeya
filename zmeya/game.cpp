#include "game.h"
#include <conio.h>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <algorithm>

Game::Game() : snake(START_X, START_Y), running(true), score(0), gameStarted(false) {
    console = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cursorInfo = { 1, FALSE };
    SetConsoleCursorInfo(console, &cursorInfo);
    srand(static_cast<unsigned>(time(nullptr)));

    generateFood();
    prevCursorPos = { static_cast<SHORT>(START_X), static_cast<SHORT>(START_Y) };

    system("cls");
    drawBorders();
    showStartScreen();

    SetConsoleTextAttribute(console, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
    SetConsoleCursorPosition(console, prevCursorPos);
    std::cout << 'O';

    prevBody = snake.getBody();
}

void Game::run() {
    while (running) {
        DWORD frameStart = GetTickCount();

        processInput();
        if (gameStarted) {
            update();
            render();
        }

        DWORD frameTime = GetTickCount() - frameStart;
        if (frameTime < GAME_SPEED) {
            Sleep(GAME_SPEED - frameTime);
        }
    }
    gameOver();
}

void Game::processInput() {
    if (_kbhit()) {
        int key = _getch();

        if (key == 0xE0) {
            key = _getch();
            switch (key) {
            case 72: key = KEY_UP; break;
            case 80: key = KEY_DOWN; break;
            case 75: key = KEY_LEFT; break;
            case 77: key = KEY_RIGHT; break;
            }
        }

        if (!gameStarted) {
            if (key == KEY_W || key == KEY_A || key == KEY_S || key == KEY_D ||
                key == KEY_UP || key == KEY_LEFT || key == KEY_RIGHT || key == KEY_DOWN) {
                gameStarted = true;
                system("cls");
                drawBorders();
            }
            return;
        }

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

    // Движение змейки
    snake.move();

    const auto& head = snake.getBody().front();

    // Проверка столкновения с едой (2x2 клетки)
    bool ateFood = false;
    for (int dy = 0; dy < FOOD_HEIGHT && !ateFood; dy++) {
        for (int dx = 0; dx < FOOD_WIDTH; dx++) {
            if (head.first == foodX + dx && head.second == foodY + dy) {
                ateFood = true;
                break;
            }
        }
    }

    if (ateFood) {
        // Увеличение змейки
        snake.grow();
        score += 10;

        // Очистка старой еды
        for (int dy = 0; dy < FOOD_HEIGHT; dy++) {
            for (int dx = 0; dx < FOOD_WIDTH; dx++) {
                COORD pos = {
                    static_cast<SHORT>(foodX + dx),
                    static_cast<SHORT>(foodY + dy)
                };
                SetConsoleCursorPosition(console, pos);
                std::cout << ' ';
            }
        }

        // Генерация новой еды
        generateFood();
    }

    // Проверка столкновения с границами
    if (head.first <= BORDER_WIDTH ||
        head.first >= WIDTH - BORDER_WIDTH - 1 ||
        head.second <= BORDER_WIDTH ||
        head.second >= HEIGHT - BORDER_WIDTH - 1) {
        running = false;
        return;
    }

    // Проверка самопересечения (только если змейка длиннее 4 сегментов)
    if (snake.getBody().size() > 4) {
        const auto& head = snake.getBody().front();
        // Начинаем проверку с 4-го сегмента для исключения ложных столкновений
        for (auto it = snake.getBody().begin() + 4; it != snake.getBody().end(); ++it) {
            if (head == *it) {
                running = false;
                return;
            }
        }
    }
}

void Game::render() {
    // Очистка предыдущих позиций змейки
    SetConsoleTextAttribute(console, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
    for (const auto& segment : prevBody) {
        COORD pos = {
            static_cast<SHORT>(segment.first),
            static_cast<SHORT>(segment.second)
        };
        SetConsoleCursorPosition(console, pos);
        std::cout << ' '; // Очищаем старую позицию
    }

    // Сохраняем текущее состояние для следующей очистки
    prevBody = snake.getBody();

    // Отрисовка новых позиций змейки
    for (const auto& segment : prevBody) {
        COORD pos = {
            static_cast<SHORT>(segment.first),
            static_cast<SHORT>(segment.second)
        };
        SetConsoleCursorPosition(console, pos);
        std::cout << 'O';
    }

    // Отрисовка еды (2x2)
    SetConsoleTextAttribute(console, FOREGROUND_RED | FOREGROUND_INTENSITY);
    for (int dy = 0; dy < FOOD_HEIGHT; dy++) {
        for (int dx = 0; dx < FOOD_WIDTH; dx++) {
            COORD pos = {
                static_cast<SHORT>(foodX + dx),
                static_cast<SHORT>(foodY + dy)
            };
            SetConsoleCursorPosition(console, pos);
            std::cout << '@';
        }
    }

    // Обновление счёта
    COORD scorePos = { 0, HEIGHT };
    SetConsoleCursorPosition(console, scorePos);
    SetConsoleTextAttribute(console, FOREGROUND_BLUE | FOREGROUND_INTENSITY);
    std::cout << "Score: " << score << "    ";
}

void Game::generateFood() {
    const auto& body = snake.getBody();
    bool collision;

    do {
        collision = false;
        foodX = rand() % (WIDTH - FOOD_WIDTH - 2 * BORDER_WIDTH) + BORDER_WIDTH;
        foodY = rand() % (HEIGHT - FOOD_HEIGHT - 2 * BORDER_WIDTH) + BORDER_WIDTH;

        for (const auto& segment : body) {
            for (int dy = 0; dy < FOOD_HEIGHT; dy++) {
                for (int dx = 0; dx < FOOD_WIDTH; dx++) {
                    if (segment.first == foodX + dx && segment.second == foodY + dy) {
                        collision = true;
                        break;
                    }
                }
                if (collision) break;
            }
            if (collision) break;
        }
    } while (collision);
}

void Game::drawBorders() {
    SetConsoleTextAttribute(console, FOREGROUND_BLUE | FOREGROUND_INTENSITY);
    for (int x = 0; x < WIDTH; x++) {
        COORD pos = { static_cast<SHORT>(x), 0 };
        SetConsoleCursorPosition(console, pos);
        std::cout << '#';

        pos.Y = HEIGHT - 1;
        SetConsoleCursorPosition(console, pos);
        std::cout << '#';
    }
    for (int y = 1; y < HEIGHT - 1; y++) {
        COORD pos = { 0, static_cast<SHORT>(y) };
        SetConsoleCursorPosition(console, pos);
        std::cout << '#';

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
    COORD pos = { WIDTH / 2 - 15, HEIGHT / 2 + 2 };
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
    return x > BORDER_WIDTH - 1 && x < WIDTH - BORDER_WIDTH &&
        y > BORDER_WIDTH - 1 && y < HEIGHT - BORDER_WIDTH;
}
