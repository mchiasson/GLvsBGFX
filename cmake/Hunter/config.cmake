hunter_config(bx
    KEEP_PACKAGE_SOURCES
    GIT_SUBMODULE 3rdparty/bx
    #URL https://github.com/mchiasson/bx/archive/v1.0.0-p8.tar.gz
    #SHA1 1884503ecfe6570814d191e5176fccc44e16db60
)

hunter_config(bimg
    KEEP_PACKAGE_SOURCES
    GIT_SUBMODULE 3rdparty/bimg
    #URL https://github.com/mchiasson/bimg/archive/v1.0.0-p8.tar.gz
    #SHA1 d845316761d4276066eb05fe7b7aa47af59c9531
)

hunter_config(bgfx
    KEEP_PACKAGE_SOURCES
    GIT_SUBMODULE 3rdparty/bgfx
    #URL https://github.com/mchiasson/bgfx/archive/v1.0.0-p5.tar.gz
    #SHA1 d78e22a0af0f985524d0a8368a746a1dce3e700e
    CMAKE_ARGS
        BGFX_BUILD_TOOLS=1
        BGFX_BUILD_EXAMPLES=0
)
