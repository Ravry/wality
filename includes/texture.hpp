#pragma once
#include <iostream>
#include <string_view>
#include <glad/glad.h>
#include <stb_image.h>
#include "media.hpp"

class Texture {
private:
    unsigned int m_id;
    GLenum m_target;

public:
    Texture(std::string_view path, GLenum target = GL_TEXTURE_2D, GLenum filter = GL_LINEAR);
    Texture(unsigned int width, unsigned int height, GLenum target = GL_TEXTURE_2D, GLenum filter = GL_LINEAR);
    Texture(Media* media);
    ~Texture() = default;
    
    void bind();
    void update(const Media_FrameData& frameData);
    void unbind();
    void destroy();

    inline unsigned int getID() const { return m_id; }
};