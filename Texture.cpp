#include "Texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#include <stdexcept>

Texture::Texture(const char *filename)
{
    buffer = stbi_load(filename, &width, &height, &colorChannels, STBI_rgb_alpha);
    if (!buffer)
    {
        std::string reason = stbi_failure_reason();
        reason += " ";
        reason += filename;
        throw std::runtime_error(reason);
    }
}

Texture::~Texture()
{
    stbi_image_free(buffer);
    width = 0;
    height = 0;
    colorChannels = 0;
    buffer = nullptr;
}
