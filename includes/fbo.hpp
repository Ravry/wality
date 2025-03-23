#pragma once
#include <memory>
#include <glad/glad.h>
#include "texture.hpp"

class FBO {
private:
    unsigned int m_id;
public: 
    std::unique_ptr<Texture> g_Albedo;
    FBO(unsigned int width, unsigned int height, GLenum target);
    ~FBO() = default;
    void bind(GLenum target);
    void blit(int width, int height, GLenum source);
    void refactor(unsigned int width, unsigned int height, GLenum target);
    void unbind();
    void destroy();
};