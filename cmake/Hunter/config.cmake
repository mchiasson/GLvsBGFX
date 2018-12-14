hunter_config(bx
    URL https://github.com/mchiasson/bx/archive/v1.0.0-p7.tar.gz
    SHA1 e8735ca5ca598cbce9afec747b205c47ca627836
)

hunter_config(bimg
    URL https://github.com/mchiasson/bimg/archive/v1.0.0-p7.tar.gz
    SHA1 2e2a2f3895a2da1123116efc68615b3dc764730c
)

hunter_config(bgfx
    URL https://github.com/mchiasson/bgfx/archive/v1.0.0-p2.tar.gz
    SHA1 603e39302a8dd688d40158cfb3dbf39ca8931b63
    CMAKE_ARGS
        BGFX_BUILD_TOOLS=1
        BGFX_BUILD_EXAMPLES=0
)

