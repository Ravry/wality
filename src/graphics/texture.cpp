#define STB_IMAGE_IMPLEMENTATION
#include "texture.hpp"

Texture::Texture(std::string_view path, GLenum target, GLenum filter) : m_target(target) {
    glGenTextures(1, &m_id);
    bind();

    int width, height, channels;
    unsigned char* data = stbi_load(path.data(), &width, &height, &channels, 0);

    if (data) {
        GLenum format;
        if (channels == 1) {
            format = GL_RED;
        } else if (channels == 3) {
            format = GL_RGB;
        } else if (channels == 4) {
            format = GL_RGBA;
        }

        glTexImage2D(m_target, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(m_target);

        glTexParameteri(m_target, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(m_target, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(m_target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(m_target, GL_TEXTURE_MAG_FILTER, filter);
    } else {
        std::cerr << "Failed to load texture: " << path << std::endl;
    }

    stbi_image_free(data);
    unbind();
}

Texture::Texture(unsigned int width, unsigned int height, GLenum target, GLenum filter) : m_target(target){
    glGenTextures(1, &m_id);
    bind();
    
    if (target == GL_TEXTURE_2D_MULTISAMPLE) {
        glTexImage2DMultisample(m_target, 4, GL_RGB, width, height, GL_TRUE);
    } else {
        glTexImage2D(m_target, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, (void*)0);
        glGenerateMipmap(m_target);

        glTexParameteri(m_target, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(m_target, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(m_target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(m_target, GL_TEXTURE_MAG_FILTER, filter);
    }
    unbind();
}

Texture::Texture(Media* media) : m_target(GL_TEXTURE_2D) {
    glGenTextures(1, &m_id);
    bind();

    glTexStorage2D(m_target, 1, GL_RGBA8, 1920, 1080);

    glTexParameteri(m_target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(m_target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(m_target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(m_target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    unbind();
}

void Texture::bind() {
    glBindTexture(m_target, m_id);
}

// Improved PBO update method with better synchronization
void Texture::update(const Media_FrameData& frameData) {
    bind();
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 
                    frameData.width, frameData.height, 
                    GL_RGBA, GL_UNSIGNED_BYTE, frameData.data.data());
    unbind();
}

void Texture::unbind() {
    glBindTexture(m_target, 0);
}

void Texture::destroy() {
    glDeleteTextures(1, &m_id);
}