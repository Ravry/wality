#pragma once
#include <vector>
#include <memory>
#include "glad/glad.h"

#include "fbo.hpp"
#include "shader.hpp"
#include "texture.hpp"
#include "vao.hpp"
#include "vbo.hpp"
#include "ebo.hpp"
#include "gif.hpp"

class Renderer
{
private:
    unsigned int m_width, m_height;

public:
    Renderer();
    ~Renderer() = default;

    void render(float deltaTime);
    void refactor(unsigned int width, unsigned int height);
    void cleanup();
};