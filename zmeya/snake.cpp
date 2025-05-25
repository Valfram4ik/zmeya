#include "snake.h"

// Конструктор змейки
Snake::Snake(int startX, int startY) : dir(RIGHT) { // Начальное направление - вправо
    body.push_back({ startX, startY }); // Добавляем голову в начальную позицию
}

// Движение змейки
void Snake::move() {
    // Создаем новую голову на основе текущего направления
    auto newHead = body.front();
    switch (dir) {
    case UP:    newHead.second--; break; // Y уменьшается (движение вверх)
    case DOWN:  newHead.second++; break; // Y увеличивается (движение вниз)
    case LEFT:  newHead.first--; break;  // X уменьшается (движение влево)
    case RIGHT: newHead.first++; break; // X увеличивается (движение вправо)
    }

    body.insert(body.begin(), newHead); // Вставляем новую голову в начало
    body.pop_back(); // Удаляем последний элемент (хвост)
}

// Смена направления с защитой от разворота на 180°
void Snake::changeDirection(Direction newDir) {
    // Проверяем, что новое направление не противоположно текущему
    if ((dir == UP && newDir != DOWN) ||
        (dir == DOWN && newDir != UP) ||
        (dir == LEFT && newDir != RIGHT) ||
        (dir == RIGHT && newDir != LEFT)) {
        dir = newDir; // Обновляем направление
    }
}

// Проверка, находится ли голова на позиции еды
bool Snake::checkFoodCollision(int foodX, int foodY) const {
    return body.front().first == foodX && body.front().second == foodY;
}

// Увеличение длины змейки
void Snake::grow() {
    body.push_back(body.back()); // Добавляем копию хвоста
}

// Геттер для тела змейки
const std::vector<std::pair<int, int>>& Snake::getBody() const {
    return body;
}

// Геттер для направления
Direction Snake::getDirection() const {
    return dir;
}

// Проверка самопересечения
bool Snake::checkSelfCollision() const {
    const auto& head = body.front(); // Получаем голову
    // Проверяем все элементы тела, начиная с 1 (чтобы не сравнивать голову саму с собой)
    for (size_t i = 1; i < body.size(); ++i) {
        if (head == body[i]) return true;
    }
    return false;
}
