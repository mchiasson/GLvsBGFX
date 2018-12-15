#ifndef TEXTURE_H
#define TEXTURE_H

#include <cstdint>

struct Texture
{
    Texture(const char *filename);
    ~Texture();
    
    int width = 0;
    int height = 0;
    int colorChannels = 0;
    uint8_t *buffer = nullptr;
};

#endif // TEXTURE_H