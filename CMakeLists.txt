cmake_minimum_required(VERSION 3.11)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

include(Hunter/HunterGate)
HunterGate(
    URL "https://github.com/ruslo/hunter/archive/v0.23.69.tar.gz"
    SHA1 "3d6b1267844f7bb64ba9981ac5a1ff78a0237d41"
    LOCAL
)

project(GLvsBGFX)

include(3rdparty)
include(shaderc)

################################################################################
# GLvsBGFX Build Options
################################################################################

set(GLVSBGFX_RENDERER gl CACHE STRING "GLvsBGFX Renderer")
set_property(CACHE GLVSBGFX_RENDERER PROPERTY STRINGS gl bgfx)


################################################################################
# GLvsBGFX executable
################################################################################

set(CMAKE_INCLUDE_CURRENT_DIR ON)

shaderc(default_vert INPUT ${CMAKE_SOURCE_DIR}/bgfx/default_vert.sc OUTPUT ${CMAKE_BINARY_DIR}/shaders TYPE v)
shaderc(default_frag INPUT ${CMAKE_SOURCE_DIR}/bgfx/default_frag.sc OUTPUT ${CMAKE_BINARY_DIR}/shaders TYPE f)

add_executable(GLvsBGFX
    main.cpp
    Texture.cpp
    Texture.h
    ${GLVSBGFX_RENDERER}/Renderer.cpp
    Renderer.h
    bgfx/default_vert.sc
    bgfx/default_frag.sc
    bgfx/varying.def.sc
    ${default_vert}
    ${default_frag}
)

target_compile_definitions(GLvsBGFX PRIVATE $<IF:$<STREQUAL:GLVSBGFX_RENDERER,gl>,RENDERER_GL=1,RENDERER_BGFX=1>)
target_link_libraries(GLvsBGFX PRIVATE GLvsBGFXDeps)

################################################################################
# GLvsBGFX assets
################################################################################

set(GLVSBGFX_ASSETS
   atlas.png
)

foreach(GLVSBGFX_ASSET IN LISTS GLVSBGFX_ASSETS)
    configure_file(
        ${PROJECT_SOURCE_DIR}/${GLVSBGFX_ASSETS}
        ${PROJECT_BINARY_DIR}/${GLVSBGFX_ASSETS}
        COPYONLY)
endforeach()
