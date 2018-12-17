add_library(GLvsBGFXDeps INTERFACE)

##
# OpenGL Mathematics (GLM)
# https://glm.g-truc.net/
##
hunter_add_package(glm)
find_package(glm CONFIG REQUIRED)
target_link_libraries(GLvsBGFXDeps INTERFACE glm)

##
# Simple DirectMedia Layer
# https://www.libsdl.org/
##
hunter_add_package(SDL2)
find_package(SDL2 CONFIG REQUIRED)
target_link_libraries(GLvsBGFXDeps INTERFACE SDL2::SDL2 SDL2::SDL2main)

##
# bgfx
# https://github.com/bkaradzic/bgfx
##
hunter_add_package(bgfx)
find_package(bgfx CONFIG REQUIRED)
target_link_libraries(GLvsBGFXDeps INTERFACE bkaradzic::bgfx bkaradzic::bimg_decode)

##
# stb single-file public domain libraries for C/C++
# https://github.com/bkaradzic/bgfx
##
hunter_add_package(stb)
find_package(stb CONFIG REQUIRED)
target_link_libraries(GLvsBGFXDeps INTERFACE stb::stb)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "AppleClang")

    find_library(COCOA_LIBRARY Cocoa)
    find_package(metal REQUIRED)
    find_package(quartzcore REQUIRED)
    mark_as_advanced(COCOA_LIBRARY)

    target_link_libraries(GLvsBGFXDeps
        INTERFACE
            ${COCOA_LIBRARY}
            metal::metal
            quartzcore::quartzcore
    )

endif()

find_package(X11)
target_link_libraries(GLvsBGFXDeps INTERFACE ${X11_LIBRARIES})

#find_package(Vulkan)
if(TARGET Vulkan::Vulkan)
    target_link_libraries(GLvsBGFXDeps INTERFACE Vulkan::Vulkan)
endif()

find_package(OpenGL)
if(TARGET OpenGL::OpenGL)
    target_link_libraries(GLvsBGFXDeps INTERFACE OpenGL::OpenGL)
elseif(TARGET OpenGL::GL)
    target_link_libraries(GLvsBGFXDeps INTERFACE OpenGL::GL)
endif()

find_package(D3D12)
if(D3D12_FOUND)
    target_link_libraries(GLvsBGFXDeps INTERFACE ${D3D12_LIBRARIES})
endif()
