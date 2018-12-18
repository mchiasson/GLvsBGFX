#include "Renderer.h"

#include <stdexcept>
#include <memory>
#include <vector>

#include <SDL2/SDL.h>

#define BREBIS_GL_IMPLEMENTATION
#include "BrebisGL.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#ifndef RENDERER_VERTEX_MAX
#define RENDERER_VERTEX_MAX 65536
#endif

enum BufferType {
    VBO = 0,
    IBO,
};

static GLuint program = 0;
static GLuint vao = 0;
static GLuint atlasHandle = 0;
static GLuint buffers[2] = {0, 0};

static GLint a_position = 0;
static GLint a_texcoord = 0;
static GLint u_viewSize = 0;


Renderer::Renderer() : atlas("atlas.png")
{
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

    window = SDL_CreateWindow("GLvsBGFX (gl)",
                              SDL_WINDOWPOS_UNDEFINED,
                              SDL_WINDOWPOS_UNDEFINED,
                              1024,
                              768,
                              window_flags);
    if (!window)
    {
        throw std::runtime_error(SDL_GetError());
    }

    context = SDL_GL_CreateContext(window);
    if (!context)
    {
        throw std::runtime_error(SDL_GetError());
    }

    brebisGLInit();

    int width, height;
    SDL_GetWindowSize(window, &width, &height);

    uint16_t indices[RENDERER_VERTEX_MAX / 4 * 6];
    for (uint16_t i = 0; i < RENDERER_VERTEX_MAX / 4; ++i)
    {
        indices[i * 6 + 0] = i * 4 + 0;
        indices[i * 6 + 1] = i * 4 + 1;
        indices[i * 6 + 2] = i * 4 + 2;
        indices[i * 6 + 3] = i * 4 + 2;
        indices[i * 6 + 4] = i * 4 + 3;
        indices[i * 6 + 5] = i * 4 + 0;
    }

    if (brebisGLSupport(GL_VERSION_3_0))
    {
        // Create a dummy vertex array object (mandatory since GL Core profile)
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);
    }

    // Create vertex and index buffer objects for the batches
    glGenBuffers(2, buffers);
    glBindBuffer(GL_ARRAY_BUFFER, buffers[VBO]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[IBO]);

    glBufferData(GL_ARRAY_BUFFER, RENDERER_VERTEX_MAX * sizeof(Vertex), nullptr, GL_DYNAMIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), &indices, GL_STATIC_DRAW);

    const char * default_vert =
        "#if defined(GL_ES)"
        "\nprecision highp float;"
        "\n#endif"
        "\n"
        "\nuniform vec2 u_viewSize;"
        "\n"
        "\nattribute vec2 a_position;"
        "\nattribute vec2 a_texcoord;"
        "\n"
        "\nvarying vec2 v_texcoord;"
        "\n"
        "\nvoid main()"
        "\n{"
        "\n    v_texcoord  = vec2(1.0, 1.0) + a_texcoord;"
        "\n    gl_Position = vec4(2.0 * a_position.x / u_viewSize.x - 1.0,"
        "\n                       1.0 - 2.0 * a_position.y / u_viewSize.y,"
        "\n                       0,"
        "\n                       1);"
        "\n}"
        "\n";

    const char * default_frag =
        "#if defined(GL_ES)"
        "\nprecision highp float;"
        "\n#endif"
        "\n"
        "\nvarying vec2 v_texcoord;"
        "\n"
        "\nuniform sampler2D u_texture0;"
        "\n"
        "\nvoid main()"
        "\n{"
        "\n    gl_FragColor = texture2D(u_texture0, v_texcoord);"
        "\n}"
        "\n";

    int default_vert_len = strlen(default_vert);
    int default_frag_len = strlen(default_frag);

    GLuint vert = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vert, 1, &default_vert, &default_vert_len);
    glCompileShader(vert);

    GLuint frag = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(frag, 1, &default_frag, &default_frag_len);
    glCompileShader(frag);

    program = glCreateProgram();
    glAttachShader(program, vert);
    glAttachShader(program, frag);
    glLinkProgram(program);

    brebisGLCheckError();

    glDeleteShader(vert);
    glDeleteShader(frag);

    a_position = glGetAttribLocation(program, "a_position");
    a_texcoord  = glGetAttribLocation(program, "a_texcoord");
    u_viewSize = glGetUniformLocation(program, "u_viewSize");

    glUseProgram(program);
    glVertexAttribPointer(static_cast<GLuint>(a_position), 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<const void *>(offsetof(Vertex, a_position)));
    glVertexAttribPointer(static_cast<GLuint>(a_texcoord), 2, GL_SHORT, GL_TRUE,  sizeof(Vertex), reinterpret_cast<const void *>(offsetof(Vertex, a_texcoord0)));
    glEnableVertexAttribArray(static_cast<GLuint>(a_position));
    glEnableVertexAttribArray(static_cast<GLuint>(a_texcoord));

    glBindBuffer(GL_ARRAY_BUFFER, buffers[VBO]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[IBO]);

    glUniform2f(u_viewSize, static_cast<GLfloat>(width), static_cast<GLfloat>(height));

    glGenTextures(1, &atlasHandle);
    glBindTexture(GL_TEXTURE_2D, atlasHandle);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, atlas.width, atlas.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, atlas.buffer);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_SCISSOR_TEST);
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

    glClearColor(0, 0, 0, 1);
    glViewport(0, 0, width, height);

}

Renderer::~Renderer()
{
    glDeleteBuffers(2, buffers);
    buffers[VBO] = 0;
    buffers[IBO] = 0;

    if (brebisGLSupport(GL_VERSION_3_0))
    {
        glDeleteVertexArrays(1, &vao);
        vao = 0;
    }

    glDeleteProgram(program);
    program = 0;
    a_position = 0;
    a_texcoord = 0;
    u_viewSize = 0;

    brebisGLShutdown();

    SDL_GL_DeleteContext(context);
    context = nullptr;

    SDL_DestroyWindow(window);
    window = nullptr;

}

void Renderer::renderFrame(const std::vector<Vertex> &vertexData)
{
    glClear(GL_COLOR_BUFFER_BIT);

    size_t count = vertexData.size();
    size_t index = 0;

    while(count > RENDERER_VERTEX_MAX)
    {
        glBufferSubData(GL_ARRAY_BUFFER, 0, RENDERER_VERTEX_MAX * sizeof(Vertex), &vertexData[index]);
        glDrawElements(GL_TRIANGLES, RENDERER_VERTEX_MAX/4*6, GL_UNSIGNED_SHORT, 0);
        index += RENDERER_VERTEX_MAX;
        count -= RENDERER_VERTEX_MAX;
    }
    if (count > 0)
    {
        glBufferSubData(GL_ARRAY_BUFFER, 0, count * sizeof(Vertex), &vertexData[index]);
        glDrawElements(GL_TRIANGLES, count/4*6, GL_UNSIGNED_SHORT, 0);
    }

    SDL_GL_SwapWindow(window);
}

