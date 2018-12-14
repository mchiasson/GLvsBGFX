#include "Game.h"
#include <SDL2/SDL.h>

#include "events/FrameEvents.h"
#include "systems/CollisionSystem.h"
#include "systems/InputSystem.h"
#include "systems/MovementSystem.h"
#include "systems/RenderSystem.h"

#include <chrono>
#include <thread>

#include <bgfx/bgfx.h>

Game::Game(glm::vec2 size)
{
    systems.add<CollisionSystem>(size);
    systems.add<InputSystem>();
    systems.add<MovementSystem>();
    systems.add<RenderSystem>();
    systems.configure();
}

void Game::update()
{
    auto t = std::chrono::high_resolution_clock::now();

    events.emit<UpdateBeginEvent>(t);
    systems.update_all(std::chrono::duration<float>(t - prev_t).count());
    events.emit<UpdateEndEvent>(std::chrono::high_resolution_clock::now());

    prev_t = t;
}

