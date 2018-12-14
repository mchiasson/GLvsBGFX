#ifndef GAME_H
#define GAME_H

#include <entityx/entityx.h>
#include <chrono>
#include <glm/vec2.hpp>
#include <memory>

class Game : public entityx::EntityX
{
public:
    Game(glm::vec2 size);
    void update();

    std::chrono::high_resolution_clock::time_point prev_t;
};

#endif // GAME_H
