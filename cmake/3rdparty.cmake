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
target_link_libraries(GLvsBGFXDeps INTERFACE bkaradzic::bgfx)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "AppleClang")

    find_library(COCOA_LIBRARY Cocoa)
    find_package(metal REQUIRED)
    find_package(quartzcore REQUIRED)
    mark_as_advanced(COCOA_LIBRARY)

    target_link_libraries(GLvsBGFXDeps
        PUBLIC
            ${COCOA_LIBRARY}
            metal::metal
            quartzcore::quartzcore
    )

elseif(UNIX AND NOT EMSCRIPTEN)

    find_package(OpenGL REQUIRED)
    #find_package(Vulkan)
    find_package(X11 REQUIRED)

    target_link_libraries(GLvsBGFXDeps INTERFACE ${X11_LIBRARIES})

    if(TARGET Vulkan::Vulkan)
        target_link_libraries(GLvsBGFXDeps INTERFACE Vulkan::Vulkan)
    elseif(TARGET OpenGL::OpenGL)
        target_link_libraries(GLvsBGFXDeps INTERFACE OpenGL::OpenGL)
    elseif(TARGET OpenGL::GL)
        target_link_libraries(GLvsBGFXDeps INTERFACE OpenGL::GL)
    endif()

    if(TARGET OpenGL::GLX)
        target_link_libraries(GLvsBGFXDeps INTERFACE OpenGL::GLX)
    elseif(TARGET OpenGL::EGL)
        target_link_libraries(GLvsBGFXDeps INTERFACE OpenGL::EGL)
    endif()

else()

    find_package(D3D12)
    if(D3D12_FOUND)
        target_link_libraries(GLvsBGFXDeps INTERFACE ${D3D12_LIBRARIES})
        target_include_directories(GLvsBGFXDeps INTERFACE ${D3D12_INCLUDE_DIRS} PRIVATE 3rdparty/dxsdk/include)
    endif()

endif()

