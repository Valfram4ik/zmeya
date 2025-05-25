#pragma once
#include <vector>
#include <utility>

enum Direction { UP, DOWN, LEFT, RIGHT };

class Snake {
public:
    Snake(int startX, int startY);
    void move();
    void changeDirection(Direction newDir);
    bool checkFoodCollision(int foodX, int foodY) const;
    void grow();
    const std::vector<std::pair<int, int>>& getBody() const;
    Direction getDirection() const;
    bool checkSelfCollision() const;

private:
    std::vector<std::pair<int, int>> body;
    Direction dir;
};
