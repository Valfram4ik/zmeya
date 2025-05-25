#include "snake.h"

Snake::Snake(int startX, int startY) : dir(RIGHT) {
    body.push_back({ startX, startY });
}

void Snake::move() {
    auto newHead = body.front();
    switch (dir) {
    case UP:    newHead.second--; break;
    case DOWN:  newHead.second++; break;
    case LEFT:  newHead.first--; break;
    case RIGHT: newHead.first++; break;
    }
    body.insert(body.begin(), newHead);
    body.pop_back();
}

void Snake::changeDirection(Direction newDir) {
    if ((dir == UP && newDir != DOWN) ||
        (dir == DOWN && newDir != UP) ||
        (dir == LEFT && newDir != RIGHT) ||
        (dir == RIGHT && newDir != LEFT)) {
        dir = newDir;
    }
}

bool Snake::checkFoodCollision(int foodX, int foodY) const {
    return body.front().first == foodX && body.front().second == foodY;
}

void Snake::grow() {
    body.push_back(body.back());
}

const std::vector<std::pair<int, int>>& Snake::getBody() const {
    return body;
}

Direction Snake::getDirection() const {
    return dir;
}

bool Snake::checkSelfCollision() const {
    const auto& head = body.front();
    for (size_t i = 1; i < body.size(); ++i) {
        if (head == body[i]) return true;
    }
    return false;
}
