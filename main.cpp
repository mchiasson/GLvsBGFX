#include <ctime>
#include <memory>

#include "Game.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>
#include <bgfx/platform.h>

static SDL_Window *window = nullptr;
static std::unique_ptr<Game> game;
static bool running = true;

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

    Uint32 window_flags = SDL_WINDOW_ALLOW_HIGHDPI |
                          SDL_WINDOW_SHOWN |
                          SDL_WINDOW_RESIZABLE;

    window = SDL_CreateWindow("Linayr",
                              SDL_WINDOWPOS_UNDEFINED,
                              SDL_WINDOWPOS_UNDEFINED,
                              1280,
                              720,
                              window_flags);
    if (!window)
    {
        throw std::runtime_error(SDL_GetError());
    }

    SDL_SysWMinfo wmi;

    SDL_VERSION(&wmi.version);

    if (!SDL_GetWindowWMInfo(window, &wmi) )
    {
        throw std::runtime_error(SDL_GetError());
    }

    bgfx::PlatformData pd;

#if defined(SDL_VIDEO_DRIVER_X11)
    if (wmi.subsystem == SDL_SYSWM_X11)
    {
        pd.ndt = wmi.info.x11.display;
        pd.nwh = reinterpret_cast<void*>(wmi.info.x11.window);
    }
#endif

#if defined(SDL_VIDEO_DRIVER_ANDROID)
    if (wmi.subsystem == SDL_SYSWM_X11)
    {
        pd.ndt = nullptr;
        pd.nwh = wmi.info.android.window;
    }
#endif

#if defined(SDL_VIDEO_DRIVER_COCOA)
    if (wmi.subsystem == SDL_SYSWM_COCOA)
    {
        pd.ndt = nullptr;
        pd.nwh = wmi.info.cocoa.window;
    }
#endif

#if defined(SDL_VIDEO_DRIVER_WINDOWS)
    if (wmi.subsystem == SDL_SYSWM_WINDOWS)
    {
        pd.ndt = nullptr;
        pd.nwh = wmi.info.win.window;
    }
#endif

#if defined(SDL_VIDEO_DRIVER_VIVANTE)
    if (wmi.subsystem == SDL_SYSWM_VIVANTE)
    {
        pd.ndt = wmi.info.vivante.display;
        pd.nwh = wmi.info.vivante.window;
    }
#endif

    pd.context      = nullptr;
    pd.backBuffer   = nullptr;
    pd.backBufferDS = nullptr;
    bgfx::setPlatformData(pd);

    int width, height;
    SDL_GetWindowSize(window, &width, &height);

    bgfx::Init init;
    init.type  = bgfx::RendererType::Count;
    init.vendorId = BGFX_PCI_ID_NONE;
    init.resolution.width  = uint32_t(width);
    init.resolution.height = uint32_t(height);
    init.resolution.reset  = BGFX_RESET_VSYNC;

    bgfx::init(init);
    bgfx::setDebug(BGFX_DEBUG_TEXT);
    bgfx::setViewClear(0, BGFX_CLEAR_COLOR, 0xffffffff);
    bgfx::setViewRect(0, 0, 0, uint16_t(width), uint16_t(height) );

    game = std::make_unique<Game>(glm::vec2(width, height));

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

        bgfx::touch(0);
        game->update();
        bgfx::frame();
    }

    game.reset();

    bgfx::shutdown();

    SDL_DestroyWindow(window);
    window = nullptr;

    SDL_Quit();
}
