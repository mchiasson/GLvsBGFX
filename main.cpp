#include <ctime>
#include <memory>

#include "Renderer.h"

#include <SDL2/SDL.h>

#include <glm/gtc/matrix_transform.hpp> 

#ifndef NUM_IMG_ROW
#define NUM_IMG_ROW 320
#endif

SDL_Window *window = nullptr;
static std::unique_ptr<Renderer> renderer;
static bool running = true;

struct Transform
{
    glm::vec2 translate = glm::vec2(0.0f);
    float rotateZ = 0.0f;
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
    
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    Uint32 window_flags = SDL_WINDOW_ALLOW_HIGHDPI |
                          SDL_WINDOW_SHOWN |
                          SDL_WINDOW_RESIZABLE |
                          SDL_WINDOW_OPENGL;

    window = SDL_CreateWindow("GLvsBGFX",
                              SDL_WINDOWPOS_UNDEFINED,
                              SDL_WINDOWPOS_UNDEFINED,
                              1280,
                              1024,
                              window_flags);
    if (!window)
    {
        throw std::runtime_error(SDL_GetError());
    }


    renderer = std::make_unique<Renderer>();
    
    glm::u16vec2 textures[18*4];

    uint16_t s0 = 1024;
    uint16_t t0 = 1024;
    uint16_t t1 = t0 + 4096;
    for (size_t i = 0; i < 10; ++i)
    {
        uint16_t s1 = s0 + 4096;
        
        textures[i*4+0].s = s0;
        textures[i*4+0].t = t0;
        textures[i*4+1].s = s1;
        textures[i*4+1].t = t0;
        textures[i*4+2].s = s1;
        textures[i*4+2].t = t1;
        textures[i*4+3].s = s0;
        textures[i*4+3].t = t1;
        
        s0 = s1 + 2048;
    }
    
    s0 = 1024;
    t0 = t1 + 2048;
    t1 = t0 + 4096;
    for (size_t i = 10; i < 18; ++i)
    {
        uint16_t s1 = s0 + 4096;
        uint16_t t1 = t0 + 4096;
        
        textures[i*4+0].s = s0;
        textures[i*4+0].t = t0;
        textures[i*4+1].s = s1;
        textures[i*4+1].t = t0;
        textures[i*4+2].s = s1;
        textures[i*4+2].t = t1;
        textures[i*4+3].s = s0;
        textures[i*4+3].t = t1;
        
        s0 = s1 + 2048;
    }
    
    std::vector<Transform> transforms(NUM_IMG_ROW*NUM_IMG_ROW, Transform());
    
    std::vector<Vertex> vertexData;
    vertexData.resize(NUM_IMG_ROW*NUM_IMG_ROW*4);
    
    int width, height;
    SDL_GetWindowSize(window, &width, &height);
    
    const float xspace = float(width) / (float(NUM_IMG_ROW)+1.0f);
    const float ySpace = float(height) / (float(NUM_IMG_ROW)+1.0f);

    for (int y = 0; y < NUM_IMG_ROW; ++y)
    {
        for (int x = 0; x < NUM_IMG_ROW; ++x)
        {
            int i = (x+(y*NUM_IMG_ROW));
            transforms[i].translate.x = xspace*(x+1);
            transforms[i].translate.y = ySpace*(y+1);
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
            const float c = cosf(transforms[i].rotateZ);
            const float s = sinf(transforms[i].rotateZ);
            const float w = 128 * 7.5f / float(NUM_IMG_ROW);
            const float hw = w * 0.5f;
            const float x0 =   - hw;
            const float x1 = w - hw;
            const float cx0 = c * x0;
            const float cx1 = c * x1;
            const float sx1 = s * x1;
            const float sx0 = s * x0;
            
            vertexData[i*4+0].a_position = glm::vec2(cx0 - sx0 + transforms[i].translate.x, sx0 + cx0 + transforms[i].translate.y);
            vertexData[i*4+1].a_position = glm::vec2(cx1 - sx0 + transforms[i].translate.x, sx1 + cx0 + transforms[i].translate.y);
            vertexData[i*4+2].a_position = glm::vec2(cx1 - sx1 + transforms[i].translate.x, sx1 + cx1 + transforms[i].translate.y);
            vertexData[i*4+3].a_position = glm::vec2(cx0 - sx1 + transforms[i].translate.x, sx0 + cx1 + transforms[i].translate.y);
            
            transforms[i].rotateZ += 0.05f;
        }

        renderer->renderFrame(vertexData);
    }

    renderer.reset();

    SDL_DestroyWindow(window);
    window = nullptr;

    SDL_Quit();
}
