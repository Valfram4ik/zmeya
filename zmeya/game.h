#pragma once
#include "snake.h"
#include <windows.h> // Для работы с WinAPI

// Константы управления
const int KEY_ESC = 27;   // ASCII-код клавиши ESC
const int KEY_W = 119;    // W
const int KEY_A = 97;     // A
const int KEY_S = 115;    // S
const int KEY_D = 100;    // D
const int KEY_UP = 72;    // Код стрелки вверх
const int KEY_LEFT = 75;  // Код стрелки влево
const int KEY_RIGHT = 77; // Код стрелки вправо
const int KEY_DOWN = 80;  // Код стрелки вниз

// Константы игры
const int WIDTH = 80;        // Ширина игрового поля
const int HEIGHT = 24;       // Высота игрового поля
const int BORDER_WIDTH = 1;  // Толщина границ
const int START_X = 40;      // Стартовая позиция X змейки
const int START_Y = 12;      // Стартовая позиция Y змейки
const int FOOD_WIDTH = 2;    // Ширина еды (в символах)
const int FOOD_HEIGHT = 2;   // Высота еды (в символах)
const int GAME_SPEED = 150;  // Задержка между кадрами (мс)

// Главный класс игры
class Game {
public:
    Game();  // Конструктор
    void run(); // Запуск игрового цикла

private:
    // Приватные методы
    void processInput();    // Обработка ввода пользователя
    void update();          // Обновление игрового состояния
    void render();          // Отрисовка объектов
    void generateFood();    // Генерация новой позиции еды
    void drawBorders();     // Рисование границ поля
    void gameOver();        // Обработка завершения игры
    void showStartScreen(); // Отображение стартового экрана
    bool isPositionValid(int x, int y) const; // Проверка позиции

    // Поля класса
    Snake snake;        // Объект змейки
    int foodX, foodY;   // Координаты еды
    bool running;       // Флаг работы игры (true - игра активна)
    bool gameStarted;   // Флаг начала игры
    int score;          // Текущий счет
    HANDLE console;     // Хэндл консоли для работы с WinAPI
    COORD prevCursorPos;// Предыдущая позиция курсора для очистки
    std::vector<std::pair<int, int>> prevBody; // Предыдущее состояние змейки
};
