#include "Renderer.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>
#include <bgfx/platform.h>
#include <bimg/decode.h>
//#include <bx/allocator.h>

#include <stdexcept>
#include <fstream>

#ifndef RENDERER_VERTEX_MAX
#define RENDERER_VERTEX_MAX 65536
#endif

//static bx::DefaultAllocator s_allocator;

static bgfx::VertexDecl ms_decl;
static bgfx::DynamicVertexBufferHandle VBO;
static bgfx::IndexBufferHandle IBO;
static bgfx::ShaderHandle vert;
static bgfx::ShaderHandle frag;
static bgfx::ProgramHandle program;
static bgfx::TextureHandle atlasHandle;
static bgfx::UniformHandle u_texture0;
static uint64_t state = BGFX_STATE_BLEND_FUNC_SEPARATE(BGFX_STATE_BLEND_SRC_COLOR, BGFX_STATE_BLEND_INV_SRC_COLOR, BGFX_STATE_BLEND_SRC_ALPHA, BGFX_STATE_BLEND_INV_SRC_ALPHA) | BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A;

static const bgfx::Memory* ReadAllBytes(const std::string &filename)
{
    std::ifstream ifs(filename, std::ios::binary|std::ios::ate);
    int pos = ifs.tellg();

    const bgfx::Memory* mem = bgfx::alloc(pos);

    ifs.seekg(0, std::ios::beg);
    ifs.read(reinterpret_cast<char*>(mem->data), pos);

    return mem;
}

Renderer::Renderer() : atlas("atlas.png")
{
    Uint32 window_flags = SDL_WINDOW_ALLOW_HIGHDPI |
            SDL_WINDOW_SHOWN |
            SDL_WINDOW_RESIZABLE;

    window = SDL_CreateWindow("GLvsBGFX (bgfx)",
                              SDL_WINDOWPOS_UNDEFINED,
                              SDL_WINDOWPOS_UNDEFINED,
                              1024,
                              768,
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
    init.type = bgfx::RendererType::OpenGL; // forcing OpenGL to compare oranges vs oranges.
    init.vendorId = BGFX_PCI_ID_NONE;
    init.resolution.width  = uint32_t(width);
    init.resolution.height = uint32_t(height);
    init.resolution.reset  = BGFX_RESET_VSYNC;

    bgfx::init(init);

#ifndef NDEBUG
    bgfx::setDebug(BGFX_DEBUG_TEXT);
#endif

    ms_decl
            .begin()
            .add(bgfx::Attrib::Position, 2, bgfx::AttribType::Float)
            .add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Int16, true)
            .end();

    static uint16_t indices[RENDERER_VERTEX_MAX / 4 * 6];
    for (uint16_t i = 0; i < RENDERER_VERTEX_MAX / 4; ++i)
    {
        indices[i * 6 + 0] = i * 4 + 0;
        indices[i * 6 + 1] = i * 4 + 1;
        indices[i * 6 + 2] = i * 4 + 2;
        indices[i * 6 + 3] = i * 4 + 2;
        indices[i * 6 + 4] = i * 4 + 3;
        indices[i * 6 + 5] = i * 4 + 0;
    }

    // Create vertex and index buffer objects for the batches
    VBO = bgfx::createDynamicVertexBuffer(RENDERER_VERTEX_MAX, ms_decl);
    IBO = bgfx::createIndexBuffer(bgfx::makeRef(indices, sizeof(indices)));

    std::string shaderPath;

    switch (bgfx::getRendererType())
    {
        case bgfx::RendererType::Noop:
        case bgfx::RendererType::Direct3D9:  shaderPath = "shaders/dx9/";   break;
        case bgfx::RendererType::Direct3D11:
        case bgfx::RendererType::Direct3D12: shaderPath = "shaders/dx11/";  break;
        case bgfx::RendererType::Gnm:        shaderPath = "shaders/pssl/";  break;
        case bgfx::RendererType::Metal:      shaderPath = "shaders/metal/"; break;
        case bgfx::RendererType::OpenGL:     shaderPath = "shaders/glsl/";  break;
        case bgfx::RendererType::OpenGLES:   shaderPath = "shaders/essl/";  break;
        case bgfx::RendererType::Vulkan:     shaderPath = "shaders/spirv/"; break;
        default:
            throw std::runtime_error("Unsupported render type!");
            break;
    }

    const bgfx::Memory* default_vert_bin = ReadAllBytes(shaderPath + "default_vert.bin");
    const bgfx::Memory* default_frag_bin = ReadAllBytes(shaderPath + "default_frag.bin");

    vert = bgfx::createShader(default_vert_bin);
    bgfx::setName(vert, "default_vert");

    frag = bgfx::createShader(default_frag_bin);
    bgfx::setName(frag, "default_frag");

    program = bgfx::createProgram(vert, frag, true);

#if 0
    const bgfx::Memory* atlas_png = ReadAllBytes("atlas.png");
    bimg::ImageContainer* imageContainer = bimg::imageParse(&s_allocator, atlas_png->data, atlas_png->size, bimg::TextureFormat::RGBA8);
    atlasHandle = bgfx::createTexture2D(imageContainer->m_width,
                                        imageContainer->m_height,
                                        false, 1,
                                        bgfx::TextureFormat::RGBA8,
                                        BGFX_SAMPLER_NONE,
                                        bgfx::copy(imageContainer->m_data, imageContainer->m_size));
    bimg::imageFree(imageContainer);
#else
    atlasHandle = bgfx::createTexture2D(atlas.width,
                                        atlas.height,
                                        false, 1,
                                        bgfx::TextureFormat::RGBA8,
                                        BGFX_SAMPLER_NONE,
                                        bgfx::makeRef(atlas.buffer, atlas.width * atlas.height * 4));
#endif


    bgfx::setName(atlasHandle, "atlas.png");


    u_texture0 = bgfx::createUniform("u_texture0",  bgfx::UniformType::Int1);

    bgfx::setVertexBuffer(0, VBO);
    bgfx::setIndexBuffer(IBO);
    bgfx::setTexture(0, u_texture0, atlasHandle);
    bgfx::setState(state);

    bgfx::setViewClear(0, BGFX_CLEAR_COLOR, 0x000000ff);
    bgfx::setViewRect(0, 0, 0, uint16_t(width), uint16_t(height) );
}

Renderer::~Renderer()
{

    bgfx::destroy(VBO);
    bgfx::destroy(IBO);
    bgfx::destroy(program);
    bgfx::destroy(atlasHandle);
    bgfx::destroy(u_texture0);

    bgfx::shutdown();

    SDL_DestroyWindow(window);
    window = nullptr;
}

void Renderer::renderFrame(const std::vector<Vertex> &vertexData)
{
    bgfx::touch(0);

    bgfx::setIndexBuffer(IBO);
    bgfx::setVertexBuffer(0, VBO, 0, RENDERER_VERTEX_MAX);
    bgfx::setTexture(0, u_texture0, atlasHandle);
    bgfx::setState(state);

    size_t count = vertexData.size();
    size_t index = 0;

    while(count > RENDERER_VERTEX_MAX)
    {
        bgfx::update(VBO, 0, bgfx::makeRef(&vertexData[index], RENDERER_VERTEX_MAX * sizeof(Vertex)));
        bgfx::submit(0, program, 0, true);
        index += RENDERER_VERTEX_MAX;
        count -= RENDERER_VERTEX_MAX;
    }

    if (count > 0)
    {
        bgfx::setVertexBuffer(0, VBO, 0, count);
        bgfx::update(VBO, 0, bgfx::makeRef(&vertexData[index], count * sizeof(Vertex)));
        bgfx::submit(0, program, 0, false);
    }


    bgfx::frame();
}

