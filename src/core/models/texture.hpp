#pragma once

class Texture
{
public:
    unsigned int id;
    unsigned int width, height;
    unsigned int internalFormat; // Format of texture object
    unsigned int imageFormat; // Format of loaded image
    unsigned int wrapS; // Texture wrapping mode for S axis (x axis)
    unsigned int wrapT; // Texture wrapping mode for T axis (y axis)
    unsigned int minFilter; // Texture filter for minification
    unsigned int magFilter; // Texture filter for magnification

    Texture();
    void Generate(unsigned int width, unsigned int height, unsigned char* data);
    void Bind() const;
};