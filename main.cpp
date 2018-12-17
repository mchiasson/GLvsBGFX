#include <ctime>
#include <memory>
#include <climits>

#include "Renderer.h"

#include <SDL2/SDL.h>

#include <glm/gtc/matrix_transform.hpp>

#ifndef NUM_IMG_ROW
#define NUM_IMG_ROW 320
#endif

static std::unique_ptr<Renderer> renderer;
static bool running = true;

struct Transform
{
    glm::vec2 position = glm::vec2(0.0f);
    float angle = 0.0f;
};

int main(int argc, char* argv[])
{
    (void)argc;
    (void)argv;

    std::srand(static_cast<uint32_t>(std::time(nullptr)));

#ifndef NDEBUG
    SDL_LogSetPriority(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_VERBOSE);
#endif

    if(SDL_Init( SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER ) < 0)
    {
        throw std::runtime_error(SDL_GetError());
    }

    renderer = std::make_unique<Renderer>();

    // We normalize our texture coord between -32768 and +32767.
    // This way, we only need to upload 4 bytes of texcoord per images instead of 8 bytes.
    // These values are specifically hardcoded for 'atlas.png' and won't work with any other
    // images.

    glm::i16vec2 textures[18*4];

    int16_t s0 = 512+SHRT_MIN;
    int16_t t0 = 512+SHRT_MIN;
    int16_t t1 = t0 + 2048;
    for (size_t i = 0; i < 10; ++i)
    {
        int16_t s1 = s0 + 2048;

        textures[i*4+0].s = s0;
        textures[i*4+0].t = t0;
        textures[i*4+1].s = s0;
        textures[i*4+1].t = t1;
        textures[i*4+2].s = s1;
        textures[i*4+2].t = t1;
        textures[i*4+3].s = s1;
        textures[i*4+3].t = t0;

        s0 = s1 + 1024;
    }

    s0 = 512 + SHRT_MIN;
    t0 = t1 + 1024;
    t1 = t0 + 2048;
    for (size_t i = 10; i < 18; ++i)
    {
        int16_t s1 = s0 + 2048;
        int16_t t1 = t0 + 2048;

        textures[i*4+0].s = s0;
        textures[i*4+0].t = t0;
        textures[i*4+1].s = s0;
        textures[i*4+1].t = t1;
        textures[i*4+2].s = s1;
        textures[i*4+2].t = t1;
        textures[i*4+3].s = s1;
        textures[i*4+3].t = t0;

        s0 = s1 + 1024;
    }


    std::vector<Transform> transforms(NUM_IMG_ROW*NUM_IMG_ROW, Transform());

    std::vector<Vertex> vertexData;
    vertexData.resize(NUM_IMG_ROW*NUM_IMG_ROW*4);

    int width, height;
    SDL_GetWindowSize(renderer->window, &width, &height);

    const float xspace = float(width) / (float(NUM_IMG_ROW)+1.0f);
    const float ySpace = float(height) / (float(NUM_IMG_ROW)+1.0f);

    for (int y = 0; y < NUM_IMG_ROW; ++y)
    {
        for (int x = 0; x < NUM_IMG_ROW; ++x)
        {
            int i = (x+(y*NUM_IMG_ROW));
            transforms[i].position.x = xspace*(x+1);
            transforms[i].position.y = ySpace*(y+1);
            vertexData[i*4+0].a_texcoord = textures[(i % 18) * 4 + 0];
            vertexData[i*4+1].a_texcoord = textures[(i % 18) * 4 + 1];
            vertexData[i*4+2].a_texcoord = textures[(i % 18) * 4 + 2];
            vertexData[i*4+3].a_texcoord = textures[(i % 18) * 4 + 3];
        }
    }

    while(running)
    {
        SDL_Event e;
        while(SDL_PollEvent( &e ) != 0)
        {
            switch(e.type)
            {
                case SDL_WINDOWEVENT:
                    switch (e.window.event)
                    {
                        case SDL_WINDOWEVENT_CLOSE:
                        {
                            running = false;
                            break;
                        }
                        default:
                            break;
                    }
                    break;
                case SDL_QUIT:
                    running = false;
                    break;
                default:
                    break;
            }
        }

        for(size_t i = 0; i < NUM_IMG_ROW*NUM_IMG_ROW; ++i)
        {
            const float c = cosf(transforms[i].angle);
            const float s = sinf(transforms[i].angle);
            const float w = 128 * 7.5f / float(NUM_IMG_ROW);
            const float hw = w * 0.5f;
            const float x0 =   - hw;
            const float x1 = w - hw;
            const float cx0 = c * x0;
            const float cx1 = c * x1;
            const float sx1 = s * x1;
            const float sx0 = s * x0;

            // we to top left, bottom left, bottom right top right to assure counter-clock-wise as we're identifying CCW faces as the front face. 
            // the clockwise face is going to be our back face and we will be culling that face for performance reasons.
            vertexData[i * 4 + 0].a_position = glm::vec2(cx0 - sx0 + transforms[i].position.x, sx0 + cx0 + transforms[i].position.y); // top left
            vertexData[i * 4 + 1].a_position = glm::vec2(cx0 - sx1 + transforms[i].position.x, sx0 + cx1 + transforms[i].position.y); // bottom left
            vertexData[i * 4 + 2].a_position = glm::vec2(cx1 - sx1 + transforms[i].position.x, sx1 + cx1 + transforms[i].position.y); // bottom right
            vertexData[i * 4 + 3].a_position = glm::vec2(cx1 - sx0 + transforms[i].position.x, sx1 + cx0 + transforms[i].position.y); // top right

            transforms[i].angle += 0.05f;
        }

        renderer->renderFrame(vertexData);
    }

    renderer.reset();

    SDL_Quit();
}
