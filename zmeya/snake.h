#pragma once
#include <vector>
#include <utility> // Для работы с std::pair

// Перечисление возможных направлений движения
enum Direction { UP, DOWN, LEFT, RIGHT };

// Класс, реализующий логику змейки
class Snake {
public:
    // Конструктор: создает змейку в указанных координатах
    Snake(int startX, int startY);

    // Метод для движения змейки
    void move();

    // Изменение направления движения
    void changeDirection(Direction newDir);

    // Проверка столкновения головы с едой
    bool checkFoodCollision(int foodX, int foodY) const;

    // Увеличение длины змейки
    void grow();

    // Получение тела змейки (константная версия)
    const std::vector<std::pair<int, int>>& getBody() const;

    // Получение текущего направления
    Direction getDirection() const;

    // Проверка самопересечения
    bool checkSelfCollision() const;

private:
    std::vector<std::pair<int, int>> body; // Вектор координат сегментов тела
    Direction dir; // Текущее направление движения
};
