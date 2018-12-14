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

################################################################################
# GLvsBGFX Build Options
################################################################################

set(BLVSBGFX_RENDERER gl CACHE STRING "GLvsBGFX Renderer")
set_property(CACHE BLVSBGFX_RENDERER PROPERTY STRINGS gl bgfx)


################################################################################
# GLvsBGFX executable
################################################################################

set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_executable(GLvsBGFX
    main.cpp
    ${BLVSBGFX_RENDERER}/Renderer.cpp
)

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
