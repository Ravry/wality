#pragma once
#include <vector>
#include <memory>
#include "glad/glad.h"

#include "shader.hpp"
#include "vao.hpp"
#include "vbo.hpp"
#include "ebo.hpp"

class Renderer
{
public:
    Renderer();
    ~Renderer() = default;

    void render();
    void cleanup();
};