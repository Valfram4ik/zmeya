#include "game.h"
#include <conio.h>    // Для _kbhit() и _getch()
#include <iostream>   
#include <stdlib.h>    // Для rand()
#include <time.h>      // Для time()
#include <algorithm>  // Для std::any_of

// Конструктор игры
Game::Game() :
    snake(START_X, START_Y), // Инициализация змейки в центре
    running(true),           // Игра активна
    score(0),               // Начальный счет
    gameStarted(false)      // Игра еще не началась
{
    console = GetStdHandle(STD_OUTPUT_HANDLE); // Получаем хэндл консоли

    // Настраиваем курсор (делаем невидимым)
    CONSOLE_CURSOR_INFO cursorInfo = { 1, FALSE };
    SetConsoleCursorInfo(console, &cursorInfo);

    srand(static_cast<unsigned>(time(nullptr))); // Инициализация генератора случайных чисел

    generateFood(); // Генерируем первую еду

    prevCursorPos = { static_cast<SHORT>(START_X), static_cast<SHORT>(START_Y) }; // Начальная позиция

    system("cls"); // Очищаем консоль
    drawBorders(); // Рисуем границы
    showStartScreen(); // Показываем стартовый экран

    // Отрисовываем начальное положение змейки
    SetConsoleTextAttribute(console, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
    SetConsoleCursorPosition(console, prevCursorPos);
    std::cout << 'O';
}

// Главный игровой цикл
void Game::run() {
    while (running) { // Пока игра активна
        DWORD frameStart = GetTickCount(); // Засекаем время начала кадра

        processInput(); // Обрабатываем ввод
        if (gameStarted) { // Если игра начата
            update();   // Обновляем состояние
            render();   // Отрисовываем объекты
        }

        // Контроль FPS (задержка между кадрами)
        DWORD frameTime = GetTickCount() - frameStart;
        if (frameTime < GAME_SPEED) {
            Sleep(GAME_SPEED - frameTime); // Задержка для стабильного FPS
        }
    }
    gameOver(); // Вызываем экран завершения
}

// Обработка пользовательского ввода
void Game::processInput() {
    if (_kbhit()) { // Если нажата клавиша
        int key = _getch(); // Считываем код клавиши

        // Обработка стрелок (двойной код)
        if (key == 0xE0) {
            key = _getch(); // Получаем реальный код стрелки
            switch (key) {
            case 72: key = KEY_UP; break;
            case 80: key = KEY_DOWN; break;
            case 75: key = KEY_LEFT; break;
            case 77: key = KEY_RIGHT; break;
            }
        }

        if (!gameStarted) { // Если игра еще не начата
            // Старт при нажатии любой клавиши управления
            if (key == KEY_W || key == KEY_A || key == KEY_S || key == KEY_D ||
                key == KEY_UP || key == KEY_LEFT || key == KEY_RIGHT || key == KEY_DOWN) {
                gameStarted = true; // Начинаем игру
                system("cls"); // Очищаем консоль
                drawBorders(); // Перерисовываем границы
            }
            return;
        }

        // Обработка управления во время игры
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
            running = false; // Выход из игры
            break;
        }
    }
}

// Обновление игрового состояния
void Game::update() {
    if (!gameStarted) return; // Если игра не начата, выходим

    snake.move(); // Двигаем змейку

    const auto& head = snake.getBody().front(); // Получаем голову

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
        snake.grow(); // Увеличиваем змейку
        score += 10; // Добавляем очки

        // Очищаем старую еду
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

        generateFood(); // Генерируем новую еду
    }

    // Проверка столкновения с границами
    if (head.first <= BORDER_WIDTH ||
        head.first >= WIDTH - BORDER_WIDTH - 1 ||
        head.second <= BORDER_WIDTH ||
        head.second >= HEIGHT - BORDER_WIDTH - 1) {
        running = false; // Завершаем игру
        return;
    }

    // Проверка самопересечения (только если длина > 4)
    if (snake.getBody().size() > 4) {
        const auto& head = snake.getBody().front();
        for (auto it = snake.getBody().begin() + 4; it != snake.getBody().end(); ++it) {
            if (head == *it) {
                running = false;
                return;
            }
        }
    }
}

// Отрисовка игровых объектов
void Game::render() {
    // Очистка предыдущих позиций змейки
    SetConsoleTextAttribute(console, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
    for (const auto& segment : prevBody) {
        COORD pos = {
            static_cast<SHORT>(segment.first),
            static_cast<SHORT>(segment.second)
        };
        SetConsoleCursorPosition(console, pos);
        std::cout << ' ';
    }

    // Сохраняем текущее состояние змейки
    prevBody = snake.getBody();

    // Отрисовка новой змейки
    for (const auto& segment : prevBody) {
        COORD pos = {
            static_cast<SHORT>(segment.first),
            static_cast<SHORT>(segment.second)
        };
        SetConsoleCursorPosition(console, pos);
        std::cout << ' '; // Очищаем старую позицию
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

    // Обновление позиции головы для следующей очистки
    if (!prevBody.empty()) {
        prevCursorPos = {
            static_cast<SHORT>(prevBody.front().first),
            static_cast<SHORT>(prevBody.front().second)
        };
    }

    // Обновление счёта
    COORD scorePos = { 0, HEIGHT };
    SetConsoleCursorPosition(console, scorePos);
    SetConsoleTextAttribute(console, FOREGROUND_BLUE | FOREGROUND_INTENSITY);
    std::cout << "Score: " << score << "    ";
}

// Генерация новой позиции еды
void Game::generateFood() {
    const auto& body = snake.getBody();
    bool collision;

    do {
        collision = false;
        // Генерация в пределах поля (с учетом границ)
        foodX = rand() % (WIDTH - FOOD_WIDTH - 2 * BORDER_WIDTH) + BORDER_WIDTH;
        foodY = rand() % (HEIGHT - FOOD_HEIGHT - 2 * BORDER_WIDTH) + BORDER_WIDTH;

        // Проверка на пересечение с телом змейки
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
    } while (collision); // Повторяем, пока не найдем свободное место
}

// Рисование границ игрового поля
void Game::drawBorders() {
    SetConsoleTextAttribute(console, FOREGROUND_BLUE | FOREGROUND_INTENSITY);
    // Верхняя и нижняя границы
    for (int x = 0; x < WIDTH; x++) {
        COORD pos = { static_cast<SHORT>(x), 0 };
        SetConsoleCursorPosition(console, pos);
        std::cout << '#';

        pos.Y = HEIGHT - 1;
        SetConsoleCursorPosition(console, pos);
        std::cout << '#';
    }
    // Левая и правая границы
    for (int y = 1; y < HEIGHT - 1; y++) {
        COORD pos = { 0, static_cast<SHORT>(y) };
        SetConsoleCursorPosition(console, pos);
        std::cout << '#';

        pos.X = WIDTH - 1;
        SetConsoleCursorPosition(console, pos);
        std::cout << '#';
    }
}

// Экран завершения игры
void Game::gameOver() {
    system("cls"); // Очищаем консоль

    // Выводим "GAME OVER"
    COORD pos = { WIDTH / 2 - 5, HEIGHT / 2 };
    SetConsoleCursorPosition(console, pos);
    SetConsoleTextAttribute(console, FOREGROUND_RED | FOREGROUND_INTENSITY);
    std::cout << "GAME OVER!";

    // Выводим финальный счет
    pos.Y += 2;
    SetConsoleCursorPosition(console, pos);
    SetConsoleTextAttribute(console, FOREGROUND_BLUE | FOREGROUND_INTENSITY);
    std::cout << "Final Score: " << score;

    // Инструкция для выхода
    pos.Y += 2;
    SetConsoleCursorPosition(console, pos);
    std::cout << "Press any key to exit...";
    _getch(); // Ожидаем нажатия любой клавиши
}

// Стартовый экран
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

// Проверка валидности позиции (не используется в текущей версии)
bool Game::isPositionValid(int x, int y) const {
    return x > BORDER_WIDTH - 1 && x < WIDTH - BORDER_WIDTH &&
        y > BORDER_WIDTH - 1 && y < HEIGHT - BORDER_WIDTH;
}
