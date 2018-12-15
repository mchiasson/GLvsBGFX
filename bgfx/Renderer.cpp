#include "Renderer.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>
#include <bgfx/platform.h>

#include <stdexcept>

extern SDL_Window *window;

Renderer::Renderer()
{
    prev_t = std::chrono::high_resolution_clock::now();
    
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
    //bgfx::setDebug(BGFX_DEBUG_TEXT);
    bgfx::setViewClear(0, BGFX_CLEAR_COLOR, 0x000000ff);
    bgfx::setViewRect(0, 0, 0, uint16_t(width), uint16_t(height) );
}

Renderer::~Renderer()
{
    bgfx::shutdown();
}

void Renderer::renderFrame()
{
    bgfx::touch(0);


    bgfx::frame();
}

