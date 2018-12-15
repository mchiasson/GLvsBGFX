#ifndef RENDERER_H
#define RENDERER_H

#include <vector>

#include "Texture.h"
#include "Vertex.h"

class Renderer
{
public:
    Renderer();
    ~Renderer();
    void renderFrame(const std::vector<Vertex> &vertices);

    Texture atlas;
};

#endif // GAME_H
