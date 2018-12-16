#include "Renderer.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>
#include <bgfx/platform.h>

#include <stdexcept>

#ifndef RENDERER_VERTEX_MAX
#define RENDERER_VERTEX_MAX 65536
#endif

extern SDL_Window *window;

Renderer::Renderer() : atlas("atlas.png")
{
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
    init.type = bgfx::RendererType::OpenGL; // forcing OpenGL to compare oranges vs oranges.
    init.vendorId = BGFX_PCI_ID_NONE;
    init.resolution.width  = uint32_t(width);
    init.resolution.height = uint32_t(height);
    init.resolution.reset  = BGFX_RESET_VSYNC;

    bgfx::init(init);
#ifndef NDEBUG
    bgfx::setDebug(BGFX_DEBUG_TEXT);
#endif
    bgfx::setViewClear(0, BGFX_CLEAR_COLOR, 0x000000ff);
    bgfx::setViewRect(0, 0, 0, uint16_t(width), uint16_t(height) );
}

Renderer::~Renderer()
{
    bgfx::shutdown();
}

void Renderer::renderFrame(const std::vector<Vertex> &vertexData)
{
    bgfx::touch(0);


    bgfx::frame();
}

