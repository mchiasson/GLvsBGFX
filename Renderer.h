#ifndef RENDERER_H
#define RENDERER_H

#include <vector>

#include "Texture.h"
#include "Vertex.h"
#include <SDL2/SDL_video.h>

class Renderer
{
public:
    Renderer();
    ~Renderer();
    void renderFrame(const std::vector<Vertex> &vertices);

    Texture atlas;

    SDL_Window *window = nullptr;
    SDL_GLContext context = nullptr;
};

#endif // GAME_H
